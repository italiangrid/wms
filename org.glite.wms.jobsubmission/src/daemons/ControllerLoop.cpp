#include <cstring>
#include <cerrno>

#include <memory>
#include <iostream>
#include <vector>

#include <classad_distribution.h>

#include <boost/timer.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <signal.h>

#include "glite/wms/common/utilities/input_reader.h"
#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/boost_fs_add.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

#include "common/process.h"
#include "common/user.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/ManipulationExceptions.h"
#include "common/EventLogger.h"
#include "common/SignalChecker.h"
#include "controller/CondorG.h"
#include "controller/JobController.h"
#include "controller/JobControllerClient.h"
#include "controller/JobControllerExceptions.h"
#include "controller/Request.h"
#include "controller/RequestExceptions.h"

#include "ControllerLoop.h"
#include "exceptions.h"

using namespace std;
namespace fs = boost::filesystem;
USING_COMMON_NAMESPACE;

RenameLogStreamNS_ts(ts);
RenameLogStreamNS(elog);

namespace {
  std::string prec_user_subject;
  int msubmit_count = 0;
}

JOBCONTROL_NAMESPACE_BEGIN {

namespace daemons {

const int           ControllerLoop::cl_s_signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGTERM };
ControllerLoop     *ControllerLoop::cl_s_instance = NULL;

void deleter(std::pair<controller::Request, std::string>* r) {
  // TODO
  //fs::remove(r->second);
}

void ControllerLoop::createDirectories()
{
  const configuration::JCConfiguration        *jcconfig = configuration::Configuration::instance()->jc();
  const configuration::LMConfiguration        *lmconfig = configuration::Configuration::instance()->lm();
  fs::path                      partial;
  vector<fs::path>              paths;
  vector<fs::path>::iterator    pathIt;

  if (this->cl_verbose) {
    clog << "Checking for directories...\n";
  }

  try {
    paths.push_back(fs::path(jcconfig->input(), fs::native));
    paths.push_back(fs::path(jcconfig->log_file(), fs::native));
    paths.push_back(fs::path(jcconfig->log_rotation_base_file(), fs::native));
    paths.push_back(fs::path(jcconfig->submit_file_dir() + "/boh", fs::native));
    paths.push_back(fs::path(jcconfig->lock_file(), fs::native));
    paths.push_back(fs::path(lmconfig->monitor_internal_dir() + "/boh", fs::native));
    paths.push_back(fs::path(lmconfig->condor_log_dir() + "/boh", fs::native));

    for (pathIt = paths.begin(); pathIt != paths.end(); ++pathIt) {
      if ((!pathIt->native_file_string().empty()) && !fs::exists(pathIt->branch_path())) {
        if (this->cl_verbose) {
          clog << "\tCreating directory: " << pathIt->branch_path().native_file_string() << "...\n";
        }
        fs::create_parents(pathIt->branch_path());
      } else if (this->cl_verbose) {
        clog << "\tDirectory: " << pathIt->branch_path().native_file_string() << " exists...\n";
      }
    }
  } catch( fs::filesystem_error &err ) {
    clog << "Filesystem error: \"" << err.what() << "\"." << '\n';
    throw CannotStart( err.what() );
  }

  return;
}

void ControllerLoop::activateSignalHandling()
{
  vector<bool>::iterator     bIt;
  vector<int>::iterator      iIt;
  vector<bool>               results;
  vector<int>                signals( cl_s_signals, cl_s_signals + sizeof(cl_s_signals) / sizeof(int) );
  logger::StatePusher        pusher( this->cl_stream, "ControllerLoop::activateSignalHandling()" );

  this->cl_stream << logger::setlevel( logger::info ) << "Activating signal handlers." << '\n';
  results = jccommon::SignalChecker::instance()->add_signals( signals );

  this->cl_stream << logger::setlevel( logger::debug ) << "Signals trapped:" << '\n';
  for( bIt = results.begin(), iIt = signals.begin(); bIt != results.end(); ++bIt, ++iIt ) {
    if( *bIt )
      this->cl_stream << logger::setlevel( logger::debug )
#ifdef HAVE_STRSIGNAL
		      << strsignal( *iIt )
#else
		      << *iIt
#endif
		      << " handler successfully set.\n";
    else
      this->cl_stream << logger::setlevel( logger::severe ) << "Cannot set handler for signal: "
#ifdef HAVE_STRSIGNAL
		      << strsignal( iIt)
#else
		      << '(' << *iIt << ')'
#endif
		      << ", proceeding without it !!!\n";
  }

  return;
}

bool ControllerLoop::checkSignal(run_code_t &return_code)
{
  bool                  loop = true;
  int                   signal = jccommon::SignalChecker::instance()->check_signal();
  logger::StatePusher   pusher( this->cl_stream, "ControllerLoop::checkSignal(...)" );

  if( signal != 0 ) {
    this->cl_stream << logger::setlevel( logger::warning ) << "Got a "
#ifdef HAVE_STRSIGNAL
		    << '\"' << strsignal(signal) << '\"'
#endif
		    << '(' << signal << ") signal, checking what to do..." << '\n';

    switch( signal ) {
    case SIGHUP:
      this->cl_stream << logger::setlevel( logger::warning ) << "Try to restart the daemon." << '\n';

      loop = false;
      return_code = reload;

      break;
    case SIGINT:
    case SIGQUIT:
    case SIGTERM:
      this->cl_stream << logger::setlevel( logger::warning ) << "Ending signal, gracefully shutting down." << '\n';

      loop = false;
      return_code = shutdown;

      break;
    default:
      this->cl_stream << logger::setlevel( logger::error )
		    << "Unhandled signal received, continuing work..." << '\n';

      loop = true;
      return_code = do_nothing;
      break;
    }
  }

  return loop;
}

ControllerLoop::ControllerLoop(utilities::LineParser const& options) :
  cl_verbose(options.is_present('v')),
  cl_stream(elog::cedglog),
  cl_options(options)
{
  const configuration::JCConfiguration* config = configuration::Configuration::instance()->jc();
  const configuration::CommonConfiguration* common = configuration::Configuration::instance()->common();
  const char  *previous = NULL;
  fs::path listname(config->input(), fs::native);
  fs::path logname(config->log_file(), fs::native);
  process::User currentUser;

  if (cl_s_instance) {
    delete cl_s_instance;
  }

  if( currentUser.uid() == 0 ) {
    if( this->cl_verbose )
      clog << "Running as root, trying to lower permissions..." << '\n';

    if( process::Process::self()->drop_privileges_forever(common->dguser().c_str()) ) {
      if( this->cl_verbose )
	clog << "Failed: reason \"" << strerror(errno) << "\"" << '\n';

      if( !options.is_present('r') )
	throw CannotStart( "Cannot drop privileges, avoiding running as root." );
      else {
	previous = "Cannot drop privileges, running as root can be dangerous !";
	if( this->cl_verbose ) clog << previous << '\n';
      }
    }
    else if( this->cl_verbose )
      clog << "Running as user " << common->dguser() << "..." << '\n';
  }
  else if( this->cl_verbose )
    clog << "Running in an unprivileged account..." << '\n';

  this->createDirectories();

  this->cl_stream.open( logname.native_file_string(), (logger::level_t) config->log_level() );
  this->cl_stream.activate_log_rotation( config->log_file_max_size(), config->log_rotation_base_file(),
					 config->log_rotation_max_file_number() );
  if (this->cl_stream.good()) {
    if (this->cl_verbose) {
      clog << "Opened log file: " << logname.native_file_string() << '\n';
    }
  } else {
    string error( "Cannot open log file \"" );
    error.append( logname.native_file_string() ); error.append( "\"" );

    throw CannotStart( error );
  }

  logger::StatePusher pusher( this->cl_stream, "ControllerLoop::ControllerLoop(...)" );

  try {
    this->cl_logger.reset(
      this->cl_options.is_present('l') ? new jccommon::EventLogger(NULL) : new jccommon::EventLogger);
    this->cl_logger->initialize_jobcontroller_context();
  } catch(jccommon::LoggerException &error) {
    this->cl_stream << logger::setlevel( logger::fatal )
		    << "Cannot create LB logger object." << '\n'
		    << "Reason: \"" << error.what() << "\"." << '\n';

    throw CannotStart(error.what());
  }

  this->cl_stream << logger::setlevel(logger::high);
  this->cl_executer.reset(new controller::CondorG(config));
  this->cl_stream << logger::setlevel(logger::high) << "Created condor object." << '\n';

  if (previous) {
    this->cl_stream << logger::setlevel( logger::null ) << previous << '\n';
  }

  try {
    this->cl_queuefilename = listname.native_file_string();
    this->cl_client.reset(new controller::JobControllerClient());
  } catch( controller::CannotCreate &error ) {
    throw CannotStart( error.reason() );
  }

  cl_s_instance = this;
  ts::edglog.unsafe_attach( this->cl_stream ); // Attach edglog to the right stream
}

ControllerLoop::run_code_t ControllerLoop::run() try
{
  run_code_t ret = do_nothing;
  controller::Request::request_code_t command = controller::Request::unknown;
  controller::JobController controller(*this->cl_logger);
  logger::StatePusher pusher(this->cl_stream, "ControllerLoop::run()");
  std::vector<
    boost::shared_ptr<std::pair<controller::Request, std::string> >
  > submit_requests;
  std::vector<classad::ClassAd *> submit_classads;

  this->activateSignalHandling();
  bool loop = true;
  std::time_t t_start = std::time(0);
  while (true) {
    loop = this->checkSignal(ret);
    if (loop) {
      try {
        usleep(5000);
        int const wait_seconds = 30;
        if (t_start - std::time(0) > wait_seconds) {
          if (submit_requests.size()) {

            this->cl_stream << logger::setlevel(logger::info)
              << "releasing held requests after "
              << wait_seconds << "seconds...\n";

            controller.msubmit(submit_classads);
            submit_classads.clear();
            submit_requests.clear();
          }

          t_start = std::time(0);
        }

	      this->cl_client->extract_next_request();
        controller::Request const* req
          = this->cl_client->get_current_request();

        if (req == 0) {
          continue;
        }

        boost::shared_ptr<std::pair<controller::Request, std::string> > request(
          new std::pair<controller::Request, std::string>,
          &deleter
        );
        request->first = *req;
        request->second = this->cl_client->get_current_request_name();
        submit_requests.push_back(request);

        command = (request->first).get_command();
        configuration::ModuleType::module_type  source
	        = static_cast<configuration::ModuleType::module_type>(request->first.get_source());

	      switch (command) {
	        case controller::Request::submit: {

          this->cl_stream << logger::setlevel(logger::debug) << "Got submit request..." << '\n';

	        const controller::SubmitRequest *subreq
            = static_cast<const controller::SubmitRequest *>(&request->first);
	        classad::ClassAd *jobad = subreq->get_jobad();
          submit_classads.push_back(jobad);

#ifdef GLITE_WMS_HAVE_LBPROXY
          this->cl_logger->set_LBProxy_context(glite::jdl::get_edg_jobid(*jobad),
                                               glite::jdl::get_lb_sequence_code(*jobad),
                                               glite::jdl::get_x509_user_proxy(*jobad) );
#else
          this->cl_logger->reset_user_proxy(
            glite::jdl::get_x509_user_proxy(*jobad));
          this->cl_logger->reset_context(
            glite::jdl::get_edg_jobid(*jobad),
            glite::jdl::get_lb_sequence_code(*jobad));
#endif

          this->cl_logger->job_dequeued_event(this->cl_queuefilename);
          bool res = false;
          std::string current_user_subject =
            glite::jdl::get_user_subject_name(*jobad, res);
          int const bunch_size = 25;
          if (
            res
            && current_user_subject == prec_user_subject 
            && msubmit_count < bunch_size
          ) {
            ++msubmit_count;
          } else if (
            res
            && current_user_subject == prec_user_subject
            && msubmit_count >= bunch_size
          ) {
      	    controller.msubmit(submit_classads);
            submit_classads.clear();
            submit_requests.clear();
            msubmit_count = 0;
            t_start = std::time(0);
          } else if (res && current_user_subject != prec_user_subject) {
	          classad::ClassAd *jobad = subreq->get_jobad();
            submit_classads.push_back(jobad);
            if (submit_classads.size() == 1) {
      	      controller.submit(submit_classads.front());
            } else {
      	      controller.msubmit(submit_classads);
            }
            submit_classads.clear();
            submit_requests.clear();
            msubmit_count = 0;
            t_start = std::time(0);
          } else {
      	    controller.submit(jobad);
            submit_classads.clear();
            submit_requests.clear();
            msubmit_count = 0;
            t_start = std::time(0);
          }
          prec_user_subject = current_user_subject;
      	  break;
      	}
	      case controller::Request::remove: {

	        const controller::RemoveRequest *remreq
            = static_cast<const controller::RemoveRequest*>(&request->first);
	        string jobid(remreq->get_jobid());
          this->cl_stream << logger::setlevel(logger::info)
            << "Got remove request (JOB ID = " << jobid << ")..." << '\n';

#ifdef GLITE_WMS_HAVE_LBPROXY
          this->cl_logger->set_LBProxy_context(jobid, remreq->get_sequence_code(), remreq->get_proxyfile());
#else 
          this->cl_logger->reset_user_proxy(remreq->get_proxyfile());
          this->cl_logger->reset_context(jobid, remreq->get_sequence_code());
#endif
          if (source == configuration::ModuleType::workload_manager) {

            this->cl_logger->job_cancel_requested_event(
              configuration::ModuleType::module_name(source)
            );
          }
	  
          // See lcg2 bug: 3883
          fs::path logfile(remreq->get_logfile(), fs::native);
          this->cl_stream << logger::setlevel( logger::debug ) << "Executing remove request..." << '\n';
          if (!logfile.empty()) {
            controller.cancel(
              glite::wmsutils::jobid::JobId(jobid), logfile.native_file_string().c_str()
            );
          } else {
            controller.cancel(glite::wmsutils::jobid::JobId(jobid), 0);
          }
	      break;
	    }
	    case controller::Request::condorremove: {

	      const controller::CondorRemoveRequest *cremreq
          = static_cast<const controller::CondorRemoveRequest*>(&request->first);
	      int condorid(cremreq->get_condorid());
        this->cl_stream << logger::setlevel(logger::info) <<
          "Got remove request (condor ID = " << condorid << ")...\n" <<
          logger::setlevel(logger::debug) << "Executing remove request...\n";

        if (source == configuration::ModuleType::log_monitor) {
          fs::path logfile(cremreq->get_logfile(), fs::native);
          controller.cancel(condorid, logfile.native_file_string().c_str());
        } else {
          controller.cancel(condorid, 0);
        }
        break;
      }
      default:
        this->cl_stream << logger::setlevel(logger::null) << "unimplemented command: \"" 
          << controller::Request::string_command(command) << '\n';
        break;
        }
      } catch (jccommon::SignalChecker::Exception &signal) {
        this->cl_stream << logger::setlevel( logger::severe ) << "Ping\n";
        if (signal.signal() != 0) {
          this->cl_stream << logger::setlevel( logger::severe ) << "ifPing\n";
	        loop = this->checkSignal(ret);
        }
      } catch (controller::CannotExecute const& err) {
        this->cl_stream << logger::setmultiline(false)
          << logger::setlevel(logger::info) << "Ignoring request...\n";
      } catch (controller::MalformedRequest const& err) {
        this->cl_stream << logger::setlevel( logger::severe )
			    << "Got malformed request.\n"
			    << logger::setlevel(logger::debug) << logger::setmultiline( true, "--> " )
			    << "Broken classad = " << err.classad() << '\n';
        this->cl_stream << logger::setmultiline(false)
          << logger::setlevel(logger::info) << "Ignoring request...\n";
      } catch(controller::MismatchedProtocol const& err) {
        this->cl_stream << logger::setlevel( logger::severe )
			    << "Cannot execute command \"" << controller::Request::string_command( command ) << "\"." << '\n'
			    << "Got request with mismatching protocol." << '\n'
			    << "Must be: \"" << err.default_protocol() << "\", was \"" << err.current_protocol() << "\"." << '\n'
			    << logger::setlevel( logger::info )
			    << "Ignoring request..." << '\n';
      } catch (glite::jdl::ManipulationException const& par) {
        this->cl_stream << logger::setlevel(logger::severe)
			    << "Cannot execute command \""
          << controller::Request::string_command(command) << "\"." << '\n'
			    << "Reason: parameter \"" << par.parameter()
          << "\" not found in the classad." << '\n'
			    << logger::setlevel( logger::info )
			    << "Ignoring request..." << '\n';
      } catch (controller::ControllerError const& error) {
        this->cl_stream << logger::setlevel(logger::severe)
			    << "Cannot execute command \""
          << controller::Request::string_command(command) << "\"." << '\n'
			    << "Reason: " << error.reason() << '\n'
			    << logger::setlevel( logger::info )
			    << "Ignoring request..." << '\n';
      } catch (glite::wmsutils::jobid::JobIdException const& error) {
	      this->cl_stream << logger::setlevel(logger::severe)
			    << "Cannot execute command \""
          << controller::Request::string_command(command) << "\"." << '\n'
			    << "Error creating job id: \"" << error.what() << "\"" << '\n'
			    << logger::setlevel(logger::info)
			    << "Ignoring request..." << '\n';
      } catch (jccommon::LoggerException const& error) {
        this->cl_stream << logger::setlevel(logger::fatal)
			    << "Cannot log event to L&B service." << '\n'
			    << "Reason: " << error.what() << '\n'
			    << "Giving up." << '\n';

	      loop = false;
	      ret = shutdown;
      } catch (std::exception &error) {
        this->cl_stream << logger::setlevel( logger::null ) << "Got a standard exception..." << '\n'
          << "What = \"" << error.what() << "\"" << '\n';
        loop = false;
        ret = shutdown;
      } catch( ... ) {
        this->cl_stream << logger::setlevel( logger::null ) << "Got an unknown exception..." << '\n';

        loop = false;
        ret = shutdown;
      }
      this->cl_client->release_request();
    } // if (loop)
  }

  return ret;
} catch (exception &err) {
  logger::StatePusher     pusher( this->cl_stream, "ControllerLoop::run()" );

  this->cl_stream << logger::setlevel(logger::null)
		  << "Got an unhandled standard exception !!!" << '\n'
		  << "Namely: \"" << err.what() << "\"" << '\n'
		  << "Aborting daemon..." << '\n';

  throw;
}
}} JOBCONTROL_NAMESPACE_END;
