// File: WMReal.cpp
// Authors: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//          Marco Cecchi <Marco.Cecchi@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <algorithm>

#include <boost/bind.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "glite/lb/producer.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

#include "glite/wms/common/logger/logger_utils.h"

#include "glite/wms/common/utilities/boost_fs_add.h"
#include "glite/wms/common/utilities/FileList.h"
#include "glite/wms/common/utilities/FileListLock.h"
#include "glite/wms/common/utilities/scope_guard.h"

#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAttributes.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"

#include "glite/wms/helper/Request.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wmsutils/classads/classad_utils.h"

#include "glite/security/proxyrenewal/renewal.h"

#include "TaskQueue.hpp"
#include "plan.h"
#include "WMReal.h"
#include "lb_utils.h"

namespace utilities = glite::wms::common::utilities;
namespace configuration = glite::wms::common::configuration;
namespace jobid = glite::wmsutils::jobid;
namespace ca = glite::wmsutils::classads;
namespace jdl = glite::jdl;
namespace fs = boost::filesystem;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

std::string get_ice_input()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.ice()->input();
}

std::string get_jc_input()
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );
  return config.jc()->input();
}

fs::path
get_classad_file(std::string const& job_id)
{
  configuration::Configuration const& config(
    *configuration::Configuration::instance()
  );

  fs::path f_classad(
    fs::normalize_path(config.jc()->submit_file_dir()),
    fs::native
  );

  fs::path reduced(
    jobid::get_reduced_part(job_id),
    fs::native
  );
  f_classad /= reduced;

  fs::path cname(
    "ClassAd." + jobid::to_filename(job_id),
    fs::native
  );
  f_classad /= cname;

  return f_classad;
}

} // {anonymous}

CannotCreateWM::CannotCreateWM(std::string const& err)
  : m_message(err)
{
}

CannotCreateWM::~CannotCreateWM() throw()
{
}

char const* CannotCreateWM::what() const throw()
{
  return m_message.c_str();
}

struct WMReal::Impl {

  Impl() try
    : ice_output(get_ice_input()), to_ice(ice_output), to_ice_mx(to_ice),
      jc_output(get_jc_input()), to_jc(jc_output), to_jc_mx(to_jc)
  {
  } catch (utilities::FileContainerError const& e) {
    throw CannotCreateWM(e.string_error());
  }

  std::string ice_output;
  utilities::FileList<std::string> to_ice;
  utilities::FileListMutex to_ice_mx;

  std::string jc_output;
  utilities::FileList<std::string> to_jc;
  utilities::FileListMutex to_jc_mx;

};

WMReal::WMReal()
  : m_impl(new Impl)
{
}

namespace {

classad::ClassAd
submit_command_create(std::auto_ptr<classad::ClassAd> job_ad)
{
  classad::ClassAd result;

  result.InsertAttr("Protocol", std::string("1.0.0"));
  result.InsertAttr("Command", std::string("Submit"));
  result.InsertAttr("Source", 2);
  std::auto_ptr<classad::ClassAd> args(new classad::ClassAd);
  args->Insert("JobAd", job_ad.get());
  job_ad.release();
  result.Insert("Arguments", args.get());
  args.release();

  return result;
}

}

void
WMReal::submit(classad::ClassAd const& jdl, ContextPtr context)
{
  // we make a copy because we change the sequence code
  classad::ClassAd request_ad(jdl);

  std::string const sequence_code(get_lb_sequence_code(context));

  // before doing the planning, some helpers may need a
  // more recent sequence code than what appears in
  // the classad originally received
  jdl::set_lb_sequence_code(request_ad, sequence_code);

  std::auto_ptr<classad::ClassAd> planned_ad(Plan(request_ad));
  std::string const ce_id = jdl::get_ce_id(*planned_ad);
  log_match(context, ce_id);

  boost::regex const cream_ce_id(".+/cream-.+");
  bool const is_cream_ce = boost::regex_match(ce_id, cream_ce_id);

  std::string const output(
    is_cream_ce ? m_impl->ice_output : m_impl->jc_output
  );

  log_enqueued_start(context, output);

  std::string const sequence_code_(get_lb_sequence_code(context));
  jdl::set_lb_sequence_code(*planned_ad, sequence_code_);

  classad::ClassAd const cmd(submit_command_create(planned_ad));
  std::string const ad_str = ca::unparse_classad(cmd);

  try {

    if (is_cream_ce) {
      utilities::FileListLock lock(m_impl->to_ice_mx);
      m_impl->to_ice.push_back(ad_str);
    } else {
      utilities::FileListLock lock(m_impl->to_jc_mx);
      m_impl->to_jc.push_back(ad_str);
    }

  } catch (utilities::FileContainerError const& e) {

    log_enqueued_fail(context, output, ad_str, e.string_error());

  }

  log_enqueued_ok(context, output, ad_str);
}

namespace {

classad::ClassAd
cancel_command_create(
  std::string const& job_id,
  std::string const& sequence_code,
  std::string const& user_x509_proxy
)
{
  classad::ClassAd result;

  result.InsertAttr("Protocol", std::string("1.0.0"));
  result.InsertAttr("Command", std::string("Cancel"));
  result.InsertAttr("Source", 2);

  std::auto_ptr<classad::ClassAd> args(new classad::ClassAd);
  args->InsertAttr("Force", false);

  std::string const ad_file = get_classad_file(job_id).native_file_string();
  std::ifstream ifs(ad_file.c_str());
  classad::ClassAdParser parser;
  classad::ClassAd ad;
  if (parser.ParseClassAd(ifs, ad)) {
    bool good = false;
    std::string const log_file(jdl::get_log(ad, good));
    if (!log_file.empty() && good) {
      args->InsertAttr("LogFile", log_file);
    }
  }
  args->InsertAttr("ProxyFile", user_x509_proxy);
  args->InsertAttr("SequenceCode", sequence_code);
  args->InsertAttr("JobId", job_id);

  result.Insert("Arguments", args.get());
  args.release();

  return result;
}

}

void
WMReal::cancel(jobid::JobId const& id, ContextPtr context)
{
  std::string const sequence_code(get_lb_sequence_code(context));

  classad::ClassAd cmd(
    cancel_command_create(
      id.toString(),
      sequence_code,
      get_user_x509_proxy(id)
    )
  );

  std::string const ad_str(ca::unparse_classad(cmd));

  try {
    utilities::FileListLock lock(m_impl->to_ice_mx);
    m_impl->to_ice.push_back(ad_str);
  } catch (utilities::FileContainerError&) {
  }

  try {
    utilities::FileListLock lock(m_impl->to_jc_mx);
    m_impl->to_jc.push_back(ad_str);
  } catch (utilities::FileContainerError&) {
  }
}

}}}}
