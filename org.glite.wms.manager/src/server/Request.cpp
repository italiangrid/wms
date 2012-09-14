// File: Request.cpp
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#include "Request.hpp"
#include <map>
#include <boost/bind.hpp>
#include "lb_utils.h"
#include "glite/wms/common/utilities/wm_commands.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/lb/producer.h"

namespace jobid = glite::wmsutils::jobid;
namespace jdl = glite::jdl;
namespace utilities = glite::wms::common::utilities;
namespace configuration = glite::wms::common::configuration;

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

int get_expiry_period()
{
  configuration::WMConfiguration const& wm_config
    = *configuration::Configuration::instance()->wm();
  return wm_config.expiry_period();
}

std::string
aux_get_sequence_code(
  classad::ClassAd const& command_ad,
  std::string const& command
)
{
  std::string sequence_code;

  if (command == "jobsubmit") {
    classad::ClassAd const* job_ad = utilities::submit_command_get_ad(command_ad);
    sequence_code = jdl::get_lb_sequence_code(*job_ad);
  } else if (command == "jobcancel") {
    sequence_code = utilities::cancel_command_get_lb_sequence_code(command_ad);
  } else if (command == "jobresubmit") {
    sequence_code = utilities::resubmit_command_get_lb_sequence_code(command_ad);
  }

  return sequence_code;
}

std::string
aux_get_x509_proxy(
  classad::ClassAd const& command_ad,
  std::string const& command,
  jobid::JobId const& id
)
{
  std::string x509_proxy;

  if (command == "jobsubmit") {
    classad::ClassAd const* job_ad = utilities::submit_command_get_ad(command_ad);
    x509_proxy = jdl::get_x509_user_proxy(*job_ad);
  } else if (command == "jobcancel") {
    x509_proxy = get_user_x509_proxy(id);
  } else if (command == "jobresubmit") {
    x509_proxy = get_user_x509_proxy(id);
  }

  return x509_proxy;
}

glite::wmsutils::jobid::JobId
aux_get_id(classad::ClassAd const& command_ad, std::string const& command)
{
  if (command == "jobsubmit") {
    return jobid::JobId(
      jdl::get_edg_jobid(
        *utilities::submit_command_get_ad(command_ad)
      )
    );
  } else if (command == "jobresubmit") {
    return jobid::JobId(utilities::resubmit_command_get_id(command_ad));
  } else if (command == "jobcancel") {
    return jobid::JobId(utilities::cancel_command_get_id(command_ad));
  } else if (command == "match") {
    bool id_exists;
    jobid::JobId match_jobid;

    std::string jobidstr(
      jdl::get_edg_jobid(
        *utilities::match_command_get_ad(command_ad),
        id_exists
      )
    );

    if (id_exists) {
      match_jobid.fromString(jobidstr);
    } else {
      match_jobid.setJobId("localhost", 6000, "");
    }

    return match_jobid;
  }

  // the following is just to avoid a warning about "control reaches end of
  // non-void function", but there is no possibility to arrive here
  return jobid::JobId();
}

}

boost::tuple<
  std::string,                  // command
  jobid::JobId,                 // jobid
  std::string,                  // sequence code
  std::string                   // x509_proxy
>
parse_request(classad::ClassAd const& command_ad)
{
  std::string command;
  jobid::JobId id;
  std::string sequence_code;
  std::string x509_proxy;

  command = utilities::command_get_command(command_ad);
  id = aux_get_id(command_ad, command);
  sequence_code = aux_get_sequence_code(command_ad, command);
  x509_proxy = aux_get_x509_proxy(command_ad, command, id);

  return boost::make_tuple(command, id, sequence_code, x509_proxy);
}

Request::Request(
  classad::ClassAd& command_ad,
  std::string const& command,
  jobid::JobId const& id,
  boost::function<void()> const& cleanup
)
  : m_id(id),
    m_state(WAITING),
    m_last_processed(0),        // make it very old
    m_resubmitted(false),
    m_expiry_time(std::time(0) + get_expiry_period())
{
  std::string x509_proxy;
  std::string sequence_code;

  if (command == "jobsubmit") {

    std::auto_ptr<classad::ClassAd> ad(utilities::submit_command_remove_ad(command_ad));
    m_jdl = ad;

    bool exists = false;
    int expiry_time = jdl::get_expiry_time(*m_jdl, exists);
    if (exists) {
      m_expiry_time = expiry_time;
    }

    x509_proxy = jdl::get_x509_user_proxy(*m_jdl);
    sequence_code = jdl::get_lb_sequence_code(*m_jdl);
    m_lb_context = create_context(m_id, x509_proxy, sequence_code);

  } else if (command == "jobresubmit") {

    mark_resubmitted();

    x509_proxy = get_user_x509_proxy(m_id);
    sequence_code = utilities::resubmit_command_get_lb_sequence_code(command_ad);
    m_lb_context = create_context(m_id, x509_proxy, sequence_code);

  } else if (command == "jobcancel") {

    state(DELIVERED);

    x509_proxy = get_user_x509_proxy(m_id);
    sequence_code = utilities::cancel_command_get_lb_sequence_code(command_ad);
    m_lb_context = create_context(m_id, x509_proxy, sequence_code);

    mark_cancelled(m_lb_context);

  } else if (command == "match") {

    std::auto_ptr<classad::ClassAd> ad(utilities::match_command_remove_ad(command_ad));
    m_jdl = ad;

    std::string filename = utilities::match_command_get_file(command_ad);
    int number = utilities::match_command_get_number_of_results(command_ad);
    bool brokerinfo = utilities::match_command_get_include_brokerinfo(command_ad);

    m_match_parameters = boost::make_tuple(filename, number, brokerinfo);
  }

  m_input_cleaners.push_back(cleanup);

}

namespace {

typedef std::vector<boost::function<void()> > cleaners_type;

void apply(cleaners_type const& input_cleaners)
{
  for (cleaners_type::const_iterator it = input_cleaners.begin();
       it != input_cleaners.end(); ++it) {
    (*it)();
  }
}

}

Request::~Request()
{
  try {
    switch (m_state) {
    case Request::UNRECOVERABLE:
      log_abort(m_lb_context, m_message);
      apply(m_input_cleaners);
      break;

    case Request::DELIVERED:
      apply(m_input_cleaners);
      break;

    case Request::CANCELLED:
      log_cancelled(m_cancel_context);
      apply(m_input_cleaners);
      break;

    case Request::CANCEL_DELIVERED:
      apply(m_input_cleaners);
      break;

    default:
      // something strange has happened, e.g. a Control-C
      // do nothing
      break;
    }
  } catch (...) { // don't allow exceptions to escape
  }
}

void Request::jdl(std::auto_ptr<classad::ClassAd> jdl)
{
  m_jdl = jdl;

  // reset the expiry time if the corresponding jdl attribute exists
  bool exists;
  int expiry_time = jdl::get_expiry_time(*m_jdl, exists);
  if (exists) {
    m_expiry_time = expiry_time;
  }
}

}}}} // glite::wms::manager::server
