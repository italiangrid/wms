// File: edg-wl-planner.cpp
// Author: Francesco Giacomini
// Author: Francesco Prelz
// Copyright (c) 2003 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <dlfcn.h>              // dlopen()
#include <sys/types.h>          // mkfifo()
#include <sys/stat.h>           // mkfifo()
#include <unistd.h>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/bind.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_smallint.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/program_options.hpp>
#include <classad_distribution.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

#include "signal_handling.h"
#include "match_utils.h"
#include "lb_utils.h"
#include "submission_utils.h"
#include "dagman_utils.h"
#include "glite/wms/common/utilities/scope_guard.h"
#include "glite/wms/common/utilities/wm_commands.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/wms/common/utilities/FileList.h"
#include "glite/wms/common/utilities/FileListLock.h"
#include "glite/wms/common/utilities/scope_guard.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/WMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"
#include "glite/jdl/convert.h"
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wms/helper/jobadapter/JobAdapter.h"
#include "SubmitAdapter.h"

using namespace glite::wms::manager::server;
namespace utilities = glite::wms::common::utilities;
namespace jobcontroller = glite::wms::jobsubmission::controller;
namespace jdl = glite::jdl;
namespace configuration = glite::wms::common::configuration;
namespace jobid = glite::wmsutils::jobid;
namespace ca = glite::wmsutils::classads;
namespace logger = glite::wms::common::logger;
namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace {

unsigned int const five_minutes = 300;

int get_expiry_period()
{
  configuration::WMConfiguration const& wm_config
    = *configuration::Configuration::instance()->wm();
  return wm_config.expiry_period();
}

std::string get_filelist_name()
{
  configuration::WMConfiguration const& wm_config
    = *configuration::Configuration::instance()->wm();
  return wm_config.input();
}

std::string get_matchpipe_dir()
{
  configuration::NSConfiguration const& ns_config
    = *configuration::Configuration::instance()->ns();
  return ns_config.list_match_root_path();
}

fs::path get_input_sandbox_path(jobid::JobId const& id)
{
  configuration::NSConfiguration const& ns_config
    = *configuration::Configuration::instance()->ns();

  fs::path result(ns_config.sandbox_staging_path(), fs::native);
  result /= fs::path(jobid::get_reduced_part(id), fs::native);
  result /= fs::path(jobid::to_filename(id), fs::native);
  result /= fs::path("input", fs::native);

  return result;
}

class planning_error: public std::exception
{
  std::string m_error;
public:
  planning_error(std::string const& error)
    : m_error(error)
  {
  }
  char const* what() const throw()
  {
    return m_error.c_str();
  }
  ~planning_error() throw()
  {
  }
};

ClassAdPtr
make_planned_jdl(
  classad::ClassAd const& jdl,
  match_type const& the_match
)
{
  ClassAdPtr result(new classad::ClassAd(jdl));

  static boost::regex expression("(.+/[^\\-]+-(.+))-(.+)");
  boost::smatch pieces;
  std::string ce_id(the_match.get<0>());

  if (boost::regex_match(ce_id, pieces, expression)) {

    jdl::set_globus_resource_contact_string(
      *result,
      std::string(pieces[1].first, pieces[1].second)
    );

    jdl::set_lrms_type(
      *result,
      std::string(pieces[2].first, pieces[2].second)
    );

    jdl::set_queue_name(
      *result,
      std::string(pieces[3].first, pieces[3].second)
    );

    jdl::set_ce_id(*result, ce_id);

  } else {
    throw planning_error("invalid ce_id: " + ce_id);
  }

  return result;
}

std::string
make_match_request(classad::ClassAd const& jdl, std::string const& output_file)
{
  int const number_of_results = 10;
  bool const include_brokerinfo = true;

  classad::ClassAd cmd(
    utilities::match_command_create(
      jdl,
      output_file,
      number_of_results,
      include_brokerinfo
    )
  );

  return ca::unparse_classad(cmd);
}

bool get_matches(std::string const& s, matches_type& matches)
try {
  ClassAdPtr response(ca::parse_classad(s));
  bool const include_brokerinfo = true;
  return response
    && fill_matches(*response, matches, include_brokerinfo);
} catch (MatchError& e) {
  throw planning_error(e.what());
}

void add_brokerinfo_to_isb(classad::ClassAd& jdl)
{
  bool isb_exists = false;
  std::vector<std::string> isb;
  jdl::get_input_sandbox(jdl, isb, isb_exists);
  bool isb_base_uri_exists = false;
  std::string isb_base_uri =
    jdl::get_wmpinput_sandbox_base_uri(jdl, isb_base_uri_exists);
  if (isb_base_uri_exists) {
    isb.push_back(isb_base_uri + "/input/.BrokerInfo");
  } else {
    isb.push_back(".BrokerInfo");
  }
  jdl::set_input_sandbox(jdl, isb);
}

bool
is_cream_ce(match_type const& ce)
{
  boost::regex const cream_re(".+/cream-.+");
  return boost::regex_match(ce.get<0>(), cream_re);
}

std::string read_file(std::string const& file_name)
{
  std::string result;

  std::ifstream is(file_name.c_str());
  std::string line;
  while (getline(is, line)) {
    result += line;
  }

  return result;
}

ClassAdPtr
Plan(
  classad::ClassAd const& jdl,
  jobid::JobId const& jobid,
  std::string const& matches_file
)
{
  std::string match_response(read_file(matches_file));

  if (match_response.empty()) {

    // generate named pipe name
    std::string output_file(get_matchpipe_dir());
    output_file += '/' + jobid.getUnique();

    // create fifo
    if (::mkfifo(output_file.c_str(), S_IRUSR | S_IWUSR) == -1) {
      throw planning_error(
        "mkfifo " + output_file + ": " + std::strerror(errno)
      );
    }

    // be sure the file is unlinked in any case
    utilities::scope_guard unlink_file(
      boost::bind(::unlink, output_file.c_str())
    );

    // open the pipe for reading, non blocking
    int fd = open(output_file.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
      throw planning_error("cannot open pipe for reading");
    }

    // create a match request and push it in the wm's input fl
    { // lock
      utilities::FileList<std::string> fl(get_filelist_name());
      utilities::FileListMutex mx(fl);
      utilities::FileListLock lock(mx);
      fl.push_back(make_match_request(jdl, output_file));
    }

    bool done = false;
    while (!done) {

      // wait for input to be available
      fd_set read_fds;
      FD_ZERO(&read_fds);
      FD_SET(fd, &read_fds);
      while (select(fd + 1, &read_fds, 0, 0, 0) < 0) {
        if (errno == EINTR) {
          continue;
        } else {
          throw planning_error(
            "select failed with errno " + boost::lexical_cast<std::string>(errno)
          );
        }
      }

      // read available input
      char buf[4096];
      int nread;
      while ((nread = read(fd, buf, 4096)) < 0) {
        if (errno == EINTR) {
          continue;
        } else {
          throw planning_error(
            "read failed with errno " + boost::lexical_cast<std::string>(errno)
          );
        }
      }
      if (nread == 0) {           // EOF
        done = true;
      } else {
        match_response.append(buf, buf + nread);
      }

    }

  }

  matches_type matches;
  if (!get_matches(match_response, matches)) {
    throw planning_error("no matching resource");
  }

  // CREAM CEs are not supported for DAG nodes

  matches.erase(
    std::remove_if(
      matches.begin(),
      matches.end(),
      is_cream_ce
    ),
    matches.end()
  );

  bool use_fuzzy_rank = false;
  jdl::get_fuzzy_rank(jdl, use_fuzzy_rank);
  matches_type::const_iterator the_match(
    select_best_ce(matches, use_fuzzy_rank)
  );
  assert(the_match != matches.end());

  // create a new jdl based on the chosen match
  ClassAdPtr planned_jdl(make_planned_jdl(jdl, *the_match));

  // dump the brokerinfo to file
  ClassAdPtr brokerinfo = the_match->get<2>();
  classad::ExprTree const* DAC = jdl.Lookup("DataAccessProtocol");
  if (DAC) {
    brokerinfo->Insert("DataAccessProtocol", DAC->Copy());
  }

  fs::path brokerinfo_file(get_input_sandbox_path(jobid));
  brokerinfo_file /= fs::path(".BrokerInfo", fs::native);
  std::ofstream brokerinfo_os(brokerinfo_file.native_file_string().c_str());
  assert(brokerinfo_os);

  brokerinfo_os << ca::unparse_classad(*brokerinfo) << std::endl;

  add_brokerinfo_to_isb(*planned_jdl);

  // job adapting
  ClassAdPtr result(
    glite::wms::helper::jobadapter::JobAdapter(planned_jdl.get()).resolve()
  );

  return result;
}

class RequestExpired {};
class ProxyExpired {};
class CannotGenerateSubmitJDL {};
class CannotGenerateSubmitFile {};

ClassAdPtr do_match(
  classad::ClassAd const& jdl,
  jobid::JobId const& jobid,
  ContextPtr context,
  std::string matches_file,
  std::string const& x509_user_proxy
)
{
  std::time_t timeout = std::time(0) + get_expiry_period();
  bool exists = false;
  int expiry_time = jdl::get_expiry_time(jdl, exists);
  if (exists) {
    timeout = expiry_time;
  }
  if (std::time(0) > timeout) {
    throw RequestExpired();
  }

  boost::shared_ptr<X509> proxy(read_proxy(x509_user_proxy));
  if (is_proxy_expired(proxy, jobid)) {
    throw ProxyExpired();
  }

  ClassAdPtr result = Plan(jdl, jobid, matches_file);
  matches_file.clear();

  return result;
}

void do_it(
  classad::ClassAd& jdl,
  std::string const& output_file,
  ContextPtr context,
  jobid::JobId const& jobid,
  std::string const& matches_file,
  std::string const& x509_user_proxy
)
{
#warning TODO provide return error for get_interesting_events

  LB_Events const lb_events(get_interesting_events(context, jobid));

  int deep_count;
  int shallow_count;
  boost::tie(deep_count, shallow_count) = get_retry_counts(lb_events);

  typedef std::vector<std::pair<std::string, int> > previous_matches_type;
  previous_matches_type const previous_matches(
    get_previous_matches(lb_events)
  );

  std::vector<std::string> previous_matches_simple;
  for (previous_matches_type::const_iterator it = previous_matches.begin();
       it != previous_matches.end(); ++it) {
    previous_matches_simple.push_back(it->first);
  }

  bool const shallow_resubmission_is_enabled(
    shallow_resubmission_is_enabled(jdl)
  );
  bool const is_resubmission = !previous_matches.empty();

  fs::path const token_file(get_reallyrunning_token(jobid));

  if (shallow_resubmission_is_enabled) {

    if (is_resubmission) {

      if (fs::exists(token_file)) { // shallow resubmission
        check_shallow_count(jdl, shallow_count);
        log_resubmission_shallow(context, token_file.native_file_string());
      } else {                  // deep resubmission
        check_deep_count(jdl, deep_count);
        log_resubmission_deep(context, token_file.native_file_string());
      } 

    }

    create_token(token_file);

  } else {

    if (is_resubmission) {
      check_deep_count(jdl, deep_count);
      log_resubmission_deep(context);
    }

  }

  utilities::scope_guard create_token_undo(
    boost::bind(fs::remove, token_file)
  );

  jdl::set_edg_previous_matches_ex(jdl, previous_matches);
  jdl::set_edg_previous_matches(jdl, previous_matches_simple);

  ClassAdPtr planned_ad = do_match(jdl,
    jobid,
    context,
    matches_file,
    x509_user_proxy
  );

  if (!planned_ad) {
    throw std::runtime_error("unexpected planning failure");
  }

  std::string const ce_id(jdl::get_ce_id(*planned_ad));

  log_match(context, ce_id);

  jobcontroller::SubmitAdapter submit_adapter(*planned_ad);
  ClassAdPtr submit_ad(
    submit_adapter.adapt_for_submission(get_lb_sequence_code(context))
  );
  if (!submit_ad) {
    throw CannotGenerateSubmitJDL();
  }

  std::string const tmp_file(output_file + ".tmp");
  std::ofstream os(tmp_file.c_str());
  if (!os) {
    Error("cannot write to " << tmp_file);
    throw CannotGenerateSubmitFile();
  }

  utilities::scope_guard tmp_file_undo(
    boost::bind(fs::remove, tmp_file)
  );

  jdl::to_submit_stream(os, *submit_ad);

  if (!os) {
    Error("cannot generate the submit file");
    throw CannotGenerateSubmitFile();
  }

  os.close();

  int e = rename(tmp_file.c_str(), output_file.c_str());
  if (e) {
    Error(
      "cannot rename " << tmp_file << " to " << output_file
      << " (errno = " << e << ')'
    );
    throw CannotGenerateSubmitFile();
  }

  tmp_file_undo.dismiss();
  create_token_undo.dismiss();
}

std::fstream log_stream;

bool
init_logger(
  char const* const log_file,
  configuration::Configuration const& config
)
try {

  if (log_file) {
    if (!std::ifstream(log_file)) {
      std::ofstream _(log_file);
    }
    log_stream.open(log_file, std::ios::in | std::ios::out | std::ios::ate);
    if (log_stream) {
      logger::threadsafe::edglog.open(
        log_stream,
        static_cast<logger::level_t>(config.wm()->log_level())
      );
    } else {
      std::cerr << "init_logger: cannot open the log file "
                << log_file << '\n';
      return EXIT_FAILURE;
    }
  } else {
    logger::threadsafe::edglog.open(
      std::cerr,
      static_cast<logger::level_t>(config.wm()->log_level())
    );
  }

  return true;

} catch (std::exception const& e) {
  std::cerr << "init_logger: " << e.what() << '\n';
  return false;
} catch (...) {
  std::cerr << "init_logger: unknown exception\n";
  return false;
}

} // {anonymous}

int
main(int argc, char* argv[])
try {

  std::string input_file;
  std::string output_file;
  std::string log_file;
  std::string matches_file;

  po::options_description desc("Usage");
  desc.add_options()
    ("help", "display this help and exit")
    (
      "input",
      po::value<std::string>(&input_file),
      "input file (in classad format)"
    )
    (
      "output",
      po::value<std::string>(&output_file),
      "output file (in condor submit file format)"
    )
    (
      "matches",
      po::value<std::string>(&matches_file),
      "matches file"
    )
    (
      "log",
      po::value<std::string>(&log_file),
      "log file"
    )
    ;
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << '\n';
    return EXIT_SUCCESS;
  }

  if (!vm.count("input")) {
    std::cerr << desc << '\n';
    return EXIT_ABORT_NODE;
  }

  if (!vm.count("output")) {
    std::cerr << desc << '\n';
    return EXIT_ABORT_NODE;
  }

#warning is signal handling needed?
#warning yes it is, probably we need to intercept the termination signal from condor_dagman; in that case abort the node
  // signal_handling();

  configuration::Configuration config(
    "glite_wms.conf",
    configuration::ModuleType::workload_manager
  );
#ifndef GLITE_WMS_HAVE_SYSLOG_LOGGING
  char const* const c_log_file = (log_file.empty()) ? 0 : log_file.c_str();
  if (!init_logger(c_log_file, config)) {
    return EXIT_FAILURE;
  }
#else
  boost::details::pool::singleton_default<
    logger::wms_log
  >::instance().init(
                 logger::wms_log::SYSLOG, 
                 (logger::wms_log::level)config.wm()->log_level()
  );
#endif

  Info("glite-wms-planner starting with pid " << getpid());

  std::ifstream is(input_file.c_str());
  if (!is) {
    Error("Cannot open input file " << input_file);
    return EXIT_FAILURE;
  }

  std::string const name("dag_node_planner");

  classad::ClassAd input_ad;
  classad::ClassAdParser parser;
  if (!parser.ParseClassAd(is, input_ad)) {
    Error("Cannot parse JDL from input file (" << input_file << ')');
    return EXIT_FAILURE;
  }
  is.close();

  std::string const sequence_code;

  jobid::JobId const jobid(jdl::get_edg_jobid(input_ad));
  std::string const x509_user_proxy(jdl::get_x509_user_proxy(input_ad));
  ContextPtr context(
    create_context(
      jobid,
      x509_user_proxy,
      sequence_code,
      EDG_WLL_SOURCE_BIG_HELPER
    )
  );

  log_helper_called(context, name);

  std::string error;
  bool pending_error = false;

  try {

    do_it(input_ad, output_file, context, jobid, matches_file, x509_user_proxy);

  } catch (HitMaxRetryCount& e) {
    error = "hit max retry count ("
      + boost::lexical_cast<std::string>(e.count()) + ')';
  } catch (HitJobRetryCount& e) {
    error = "hit job retry count ("
      + boost::lexical_cast<std::string>(e.count()) + ')';
  } catch (HitMaxShallowCount& e) {
    error = "hit max shallow retry count ("
      + boost::lexical_cast<std::string>(e.count()) + ')';
  } catch (HitJobShallowCount& e) {
    error = "hit job shallow retry count ("
      + boost::lexical_cast<std::string>(e.count()) + ')';
  } catch (RequestExpired&) {
    error = "request expired";
 } catch (MissingProxy&) {
    error = "X509 proxy not found or I/O error";
 } catch (InvalidProxy&) {
    error = "invalid X509 proxy";
  } catch (ProxyExpired&) {
    error = "X509 proxy expired";
  } catch (CannotGenerateSubmitJDL&) {
    error = "cannot generate the submission JDL";
  } catch (CannotGenerateSubmitFile&) {
    error = "cannot generate the submit file";
  } catch (planning_error const& e) {
    pending_error = true;
    error = e.what();
  }

  if (error.empty()) {
    Info("success");
    log_helper_return(context, name, EXIT_SUCCESS);
    return EXIT_SUCCESS;
  } else {
    if (pending_error) {
      Info("postponing (" << error << ')');
      log_pending(context, error);
      log_helper_return(context, name, EXIT_RETRY_NODE);
      return EXIT_RETRY_NODE;
    } else {
      Error(error);
      log_helper_return(context, name, EXIT_ABORT_NODE);
      log_abort(context, error);
      return EXIT_ABORT_NODE;
    }
  }

} catch (std::exception const& e) {

  Error(e.what());
  return EXIT_ABORT_NODE;

} catch (...) {

  Error("unknown exception");
  return EXIT_ABORT_NODE;

}
