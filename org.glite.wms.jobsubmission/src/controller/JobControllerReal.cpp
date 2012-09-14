// File: JobControllerReal.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//         Rosario Peluso <Rosario.Peluso@pd.infn.it>
//         Marco Cecchi <marco.cecchi@cnaf.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cctype>

#include <memory>
#include <deque>
#include <iostream>
#include <fstream>
#include <algorithm>
#ifdef HAVE_STRINGSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include <boost/regex.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

#include <classad_distribution.h>
#include <user_log.c++.h>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/JCConfiguration.h"
#include "glite/wms/common/configuration/LMConfiguration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/utilities/filecontainer.h"

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"

#include "glite/jdl/convert.h"
#include "glite/jdl/PrivateAdManipulation.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "common/IdContainer.h"
#include "common/RamContainer.h"
#include "common/JobFilePurger.h"
#include "common/ProxyUnregistrar.h"

#include "JobControllerReal.h"
#include "JobControllerExceptions.h"
#include "CondorG.h"
#include "glite/wms/jobsubmission/SubmitAdapter.h"
#include "SubmitAd.h"
#include "SubmitAdExceptions.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS(elog);

JOBCONTROL_NAMESPACE_BEGIN {
namespace controller {
namespace {

boost::regex single_submit_regex(
  "^.*[0-9]+ job\\(s\\) submitted to cluster ([0-9]+)\\.[0-9]*.*$"
);

boost::regex mult_submit_regex(
  "^.*([0-9]+ job\\(s\\) submitted to cluster ([0-9]+))\\..*$"
);

GenericEvent *createGenericEvent(int evn)
{
  GenericEvent    *event = (GenericEvent *) instantiateEvent( ULOG_GENERIC );
  string           info( "JC: " );

  info.append( boost::lexical_cast<string>(evn) );
  info.append( " - " );
  info.append( jccommon::generic_events_string[evn] );

  strncpy( event->info, info.c_str(), (sizeof(event->info) - 1) );

  return event;
}

void logGenericEvent(jccommon::generic_event_t ev, int condorid, const char *logfile)
{
  int                      evn = static_cast<int>( ev );
  auto_ptr<GenericEvent>   event( createGenericEvent(evn) );
  UserLog                  logger( "owner", logfile, boost::lexical_cast<int>(condorid), 0, 0 );

  logger.writeEvent( event.get() );

  return;
}

bool cancelJob(const string &condorid, string &info)
{
  int                      result;
  string                   parameters;
  logger::StatePusher      pusher( elog::cedglog, "cancelJob(...)" );

  elog::cedglog << logger::setlevel( logger::debug ) << "Condor id of job was: " << condorid << '\n';
  parameters.assign( "-constraint 'ClusterId==" );
  string::size_type pos = condorid.find('.');

  if( pos != std::string::npos ) { // procID is defined
    parameters.append( condorid.substr(0, pos) );
    parameters.append( " && ProcId==" );
    parameters.append( condorid.substr( pos+1 ) );
  } else { // procID is not define so put 0
    parameters.append( condorid );
    parameters.append( " && ProcId==0" );
  }
  // remove job only if is not in "removed" status
  parameters.append( " && JobStatus!=3'");
                                                                                                                                                            
  result = CondorG::instance()->set_command( CondorG::remove, parameters )->execute( info );
                                                                                                                                                            
  if( result ) { // normal cancellation has been refused, try to force it
    elog::cedglog << logger::setlevel( logger::severe ) << "Job cancellation refused.\n"
         << "Condor ID = " << condorid
         << "\nReason: \"" << info << "\".\n";
                                                                                                                                                            
    elog::cedglog << logger::setlevel( logger::info ) << "Try to force job removal  (only for _globus_ job).\n";
                                                                                                                                                            
    parameters.assign( "-f -constraint 'ClusterId==" );
                                                                                                                                                            
    string::size_type pos = condorid.find('.');
                                                                                                                                                            
    if( pos != std::string::npos ) { // procID is defined
      parameters.append( condorid.substr(0, pos) );
      parameters.append( " && ProcId==" );
      parameters.append( condorid.substr( pos+1 ) );
    } else { // procID is not define so put 0
      parameters.append( condorid );
      parameters.append( " && ProcId==0" );
    }
    // force removal must be used only for globus jobs
    parameters.append( " && JobUniverse==9 && JobGridType==\"globus\"'" );
                                                                                                                                                            
    result = CondorG::instance()->set_command( CondorG::remove, parameters )->execute( info );
  }

  if( !result )
    elog::cedglog << logger::setlevel( logger::info ) << "Job has been succesfully removed.\n";
                                                                                                                                                          
  return !result;
}

} // Anonymous namespace

void JobControllerReal::readRepository()
{
  const configuration::LMConfiguration   *lmconfig = configuration::Configuration::instance()->lm();
  string                                  repname( lmconfig->id_repository_name() );
  auto_ptr<jccommon::IdContainer>         repository;
  fs::path                                repfile( lmconfig->monitor_internal_dir(), fs::native );
  logger::StatePusher                     pusher( elog::cedglog, "JobControllerReal::readRepository()" );

  repfile /= repname;

  try {
    elog::cedglog << logger::setlevel( logger::medium )
		  << "Reading repository from LogMonitor file: " << repfile.native_file_string() << '\n';

    repository.reset(new jccommon::IdContainer(repfile.native_file_string().c_str()));
    this->jcr_repository->copy(*repository);
  } catch (utilities::FileContainerError const& err) {
    elog::cedglog << logger::setlevel( logger::null ) << "File container error: " << err.string_error() << '\n';

    throw CannotCreate( err.string_error() );
  }

  return;
}

JobControllerReal::JobControllerReal(boost::shared_ptr<jccommon::EventLogger> logger)
  : jcr_threshold(0), jcr_logger(logger)
{
  const configuration::LMConfiguration   *lmconfig = configuration::Configuration::instance()->lm();
  const configuration::JCConfiguration   *jcconfig = configuration::Configuration::instance()->jc();
  string                                  repname(lmconfig->id_repository_name());
  fs::path                                repfile(lmconfig->monitor_internal_dir(), fs::native);
  logger::StatePusher                     pusher(elog::cedglog, "JobControllerReal::JobControllerReal()");

  repfile /= repname;
  boost::shared_ptr<jccommon::IdContainer> repository(
    new jccommon::IdContainer(repfile.native_file_string().c_str())
  );
  try {
    this->jcr_repository.reset(new jccommon::RamContainer(*repository));
  } catch (utilities::FileContainerError &err) {
    elog::cedglog << logger::setlevel(logger::null) << "File container error: " << err.string_error() << '\n';

    throw CannotCreate(err.string_error());
  }

  this->jcr_threshold = jcconfig->container_refresh_threshold(jcr_s_threshold);
  if (this->jcr_threshold < jcr_s_threshold) {
    this->jcr_threshold = jcr_s_threshold;
  }

  elog::cedglog << logger::setlevel(logger::ugly) << "Controller created...\n";
}

int JobControllerReal::msubmit(
  std::vector<classad::ClassAd*>& classads
) try
{
  logger::StatePusher pusher(elog::cedglog, "JobControllerReal::msubmit(...)");

  std::string msubmit_grouped_file(
    "/tmp/msubmit.jc." +
    boost::lexical_cast<std::string>(std::rand())
  ); 
  ofstream ofs_multiple(msubmit_grouped_file.c_str());
  if (!ofs_multiple) {
    elog::cedglog << logger::setlevel(logger::null)
          << "Cannot open condor submit file for writing.\n"
          << "File: " << msubmit_grouped_file << '\n';
  
    throw CannotExecute("Cannot open condor submit file.");
  } 

  elog::cedglog << logger::setlevel(logger::info) << "Submitting "
    << classads.size() << " jobs in one shot ("
    << msubmit_grouped_file << ")\n";

  std::vector<classad::ClassAd*>::iterator it_end = classads.end();
  std::deque<std::string> ads;
  for (
    std::vector<classad::ClassAd*>::iterator it = classads.begin();
    it != it_end;
    ++it
  ) {
    SubmitAdapter sad(*it);
    sad.createFromAd(*it);
    ads.push_back(sad->log_file());

    //TODO handle errors
    bool good;
    string id(glite::jdl::get_edg_jobid(*(*it), good));
    string seq_code(glite::jdl::get_lb_sequence_code(*(*it), good));
#ifdef GLITE_WMS_HAVE_LBPROXY
    this->jcr_logger->set_LBProxy_context(
      id,
      seq_code,
      glite::jdl::get_x509_user_proxy(**it));
#else
    this->jcr_logger->reset_context(id, seq_code);
#endif
    this->jcr_logger->condor_submit_start_event(sad->log_file());

    sad.adapt_for_submission(seq_code);
    if (sad.good()) {

      elog::cedglog << logger::setlevel(logger::verylow) << "Submitting job \"" << sad->job_id() << "\"\n";

#ifdef HAVE_STRINGSTREAM
      ostringstream                      oss;
#else
      ostrstream                         oss;
#endif

      glite::jdl::to_submit_stream(ofs_multiple, sad->classad());
      ofs_multiple << '\n';
      if (!ofs_multiple) {
        jccommon::ProxyUnregistrar(sad->job_id()).unregister();
        jccommon::JobFilePurger(sad->job_id(), sad->is_dag()).do_purge(true);

        this->jcr_logger->job_abort_cannot_write_submit_file_event(
          sad->log_file(),
          msubmit_grouped_file,
          "Cannot open file"
        );

        throw CannotExecute("Cannot write condor submit file");
      }
    } else {
      string rsl = "(unavailable)";
      //TODO handle errors
      bool good;
      string id(glite::jdl::get_edg_jobid(*(*it), good));
      string type(glite::jdl::get_type(*(*it), good));
    
      elog::cedglog << logger::setlevel(logger::null)
        << "Received classad is invalid.\n"
        << "Reason: " << sad->reason() << '\n';

      this->jcr_logger->job_abort_classad_invalid_event(
       sad->log_file(),
        sad->reason()
      );

      transform(type.begin(), type.end(), type.begin(), ::tolower);
      jccommon::ProxyUnregistrar(id).unregister();
      jccommon::JobFilePurger(id, type == "dag").do_purge(true);
    }
  } // for (classads...)

  // group command
  string parameters = "-d " + msubmit_grouped_file + " 2>&1";
  int count = 10;
  string info;
  int result = 1;
  while (count > 0 && result) { // fix for bug #23401
    result = CondorG::instance()->set_command(
      CondorG::submit,
      parameters
    )->execute(info);
    if (result) {
      // TODO discern fatal errors: why does this have to block all the rest?
      static boost::regex cred_error_regex(
        "^.*Unable to read credential for import.*$"
      );
      if (regex_search(info, cred_error_regex)) {
        result = 0;
      }

      elog::cedglog << logger::setlevel(logger::verylow)
        << "ERROR Condor returned " << info << '\n';
      sleep(1 * count);
      --count;
    }
  }

  // split msubmit into single submissions in case of error for the whole bunch
  if (result) {
    for (
      std::vector<classad::ClassAd*>::iterator it = classads.begin();
      it != it_end;
      ++it
    ) {
      this->submit(*it);
    }
  }

  elog::cedglog << logger::setlevel(logger::info)
    << "Submission to Condor returned: " << info << endl;
  boost::match_results<string::const_iterator> pieces;
  for (
    std::vector<classad::ClassAd*>::iterator it = classads.begin();
    it != it_end;
    ++it
  ) {
    if (!boost::regex_match(
        info,
        pieces,
        mult_submit_regex))
    {
      elog::cedglog << logger::setmultiline(true);
      elog::cedglog << logger::setlevel(logger::null)
        << "Error submitting job.\n"
        << "condor_submit return code = " << result << '\n'
        << logger::setmultiline(true, "CE->" ) << "Given reason\n" << info << '\n';
      elog::cedglog << logger::setmultiline(false);
      SubmitAdapter sad(*it);
      this->jcr_logger->condor_submit_failed_event(
        "(unavailable)", // rsl
        info,
        sad->log_file()
      );
      jccommon::ProxyUnregistrar(sad->job_id()).unregister();
      jccommon::JobFilePurger(sad->job_id(), sad->is_dag()).do_purge(true);
    } else { // the condor command worked fine... do the rest
      string condorid = pieces.str(2);
      info.erase(pieces.position(1), pieces.length(1));

      if (this->jcr_repository->inserted() >= this->jcr_threshold) {
        this->readRepository();
      }

      bool good;
      string id(glite::jdl::get_edg_jobid(**it, good));
      this->jcr_repository->insert(id, condorid);

      string seq_code(glite::jdl::get_lb_sequence_code(**it, good));
#ifdef GLITE_WMS_HAVE_LBPROXY
      this->jcr_logger.set_LBProxy_context(
        id,
        seq_code,
        glite::jdl::get_x509_user_proxy(**it));
#else
      this->jcr_logger->reset_context(id, seq_code);
#endif
      this->jcr_logger->condor_submit_ok_event(
        "(unavailable)", // rsl
        condorid,
        ads.front());
      ads.pop_front();
    }
  }

  ::unlink(msubmit_grouped_file.c_str());
  return 0;
} catch (utilities::FileContainerError &error) {
  elog::cedglog << logger::setlevel(logger::null) << "Error handling the internal repository.\n";
  throw CannotExecute(error.string_error());
} catch (SubmitAdException &error) {
  throw CannotExecute(error.error());
}

int JobControllerReal::submit(classad::ClassAd *pad)
try {
  int                                result = 1;
  int  															 numberId = -1;
  string                             rsl, parameters, info, seqcode;
  SubmitAdapter                      sad(pad);
  sad.createFromAd(pad);
  logger::StatePusher                pusher( elog::cedglog, "JobControllerReal::submit(...)" );
  ofstream                           ofs;
#ifdef HAVE_STRINGSTREAM
  ostringstream                      oss;
#else
  ostrstream                         oss;
#endif

  sad.createFromAd(pad);
  this->jcr_logger->condor_submit_start_event(sad->log_file());
  seqcode = this->jcr_logger->sequence_code();
  sad.adapt_for_submission(seqcode);
  elog::cedglog << logger::setlevel(logger::info) << "Submitting job \"" << sad->job_id() << "\"\n";

  if (sad.good()) {
    glite::jdl::to_submit_stream( oss, sad->classad());

#ifndef HAVE_STRINGSTREAM
    if(oss.str() == NULL) {
      elog::cedglog << logger::setlevel( logger::verylow ) << "Submit file is empty, aborting job...\n";
      this->jcr_logger.job_abort_cannot_write_submit_file_event( sad->log_file(), sad->submit_file(), "Submit file empty" );

      jccommon::ProxyUnregistrar( sad->job_id() ).unregister();
      jccommon::JobFilePurger( sad->job_id(), sad->is_dag() ).do_purge( true );

      return 0;
    }
#endif

    rsl = oss.str();

#ifndef HAVE_STRINGSTREAM
    oss.freeze(false);
#else
    if (rsl.length() == 0) {
      elog::cedglog << logger::setlevel( logger::verylow ) << "Submit file is empty, aborting job...\n";
      this->jcr_logger->job_abort_cannot_write_submit_file_event(
        sad->log_file(), sad->submit_file(), "Submit file empty");

      jccommon::ProxyUnregistrar( sad->job_id() ).unregister();
      jccommon::JobFilePurger( sad->job_id(), sad->is_dag() ).do_purge( true );

      return 0;
    }
#endif

    boost::match_results<string::const_iterator> pieces;
    ofs.open(sad->submit_file().c_str());
    if (ofs.good()) {
      ofs << rsl << '\n';
      ofs.close();
      elog::cedglog << logger::setlevel(logger::medium) << "Submit file created...\n";

      parameters = "-d " + sad->submit_file() + " 2>&1";
      int count = 3;	      
      while (count > 0 && result) { // fix for bug #23401
        // TODO discern fatal errors: why does this have to block all the rest?
      	result =
          CondorG::instance()->set_command(
            CondorG::submit, parameters
        )->execute(info);
				--count;
        if (result) {
          sleep(1 * count);
        }
      }	

      elog::cedglog << logger::setlevel(logger::info)
        << "Submission to Condor returned: " << info << '\n';
      if (result || !boost::regex_match(info, pieces, single_submit_regex)) {
        // The condor command has failed... Do the right thing
        elog::cedglog << logger::setmultiline(true);
        elog::cedglog << logger::setlevel(logger::null)
		      << "Error submitting job.\n"
		      << "condor_submit return code = " << result 
		      << logger::setmultiline(true, "CE-> ") << "Given reason\n" << info << '\n';
        elog::cedglog << logger::setmultiline(false);
        this->jcr_logger->condor_submit_failed_event(rsl, info, sad->log_file());

        jccommon::ProxyUnregistrar(sad->job_id()).unregister();
        jccommon::JobFilePurger(sad->job_id(), sad->is_dag()).do_purge(true);
      } else {
        // The condor command worked fine... Do the right thing
        string condorid = pieces.str(1);
        numberId = boost::lexical_cast<int>(condorid);
        elog::cedglog << logger::setlevel(logger::verylow)
          << "Job submitted to Condor cluster: " << condorid << '\n';
        if (this->jcr_repository->inserted() >= this->jcr_threshold) {
          this->readRepository();
        }
        this->jcr_repository->insert(sad->job_id(), condorid);
        this->jcr_logger->condor_submit_ok_event(rsl, condorid, sad->log_file());
      }
    } else {
      elog::cedglog << logger::setlevel(logger::null)
        << "Cannot open condor submit file for writing.\n" 
		    << "File: \"" << sad->submit_file() << "\"\n";

      this->jcr_logger->job_abort_cannot_write_submit_file_event(
        sad->log_file(),
        sad->submit_file(),
        "Cannot open file"
      );

      jccommon::ProxyUnregistrar(sad->job_id()).unregister();
      jccommon::JobFilePurger(sad->job_id(), sad->is_dag()).do_purge(true);
      throw CannotExecute("Cannot open condor submit file.");
    }
  } else { // submit classad not good...
    bool      good;
    string    id(glite::jdl::get_edg_jobid(*pad, good)), type(glite::jdl::get_type(*pad, good));

    elog::cedglog << logger::setlevel(logger::null)
      << "Received classad is invalid.\n" 
		  << "Reason: \"" << sad->reason() << "\"\n";

    this->jcr_logger->job_abort_classad_invalid_event(
      sad->log_file(), sad->reason());

    transform(type.begin(), type.end(), type.begin(), ::tolower);
    jccommon::ProxyUnregistrar(id).unregister();
    jccommon::JobFilePurger(id, type == "dag").do_purge(true);
  }

  return numberId;
} catch (utilities::FileContainerError &error) {
  elog::cedglog << logger::setlevel(logger::null)
    << "Error handling the internal FileList.\n";
  throw CannotExecute( error.string_error() );
} catch (SubmitAdException &error) {
  throw CannotExecute(error.error());
}

bool JobControllerReal::cancel( const glite::wmsutils::jobid::JobId &id, const char *logfile )
{
  bool                  good = true;
  int                   icid = 0;
  string                sid(id.toString()), condorid, info;
  logger::StatePusher   pusher(elog::cedglog, "JobControllerReal::cancel(...)");

  elog::cedglog << logger::setlevel( logger::info )
		<< "Asked to remove job: " << id.toString() << '\n';

  condorid = this->jcr_repository->condor_id(sid);
  if (condorid.size() == 0) { // syncronize the "ram" repository with the LM's one
     this->readRepository();
     condorid = this->jcr_repository->condor_id(sid);
  }

  if (!condorid.empty()) {
    // Comunicate to LM that this request comes from the user
    if (logfile) {
      icid = boost::lexical_cast<int>(condorid);
      logGenericEvent(jccommon::user_cancelled_event, icid, logfile);
    }

    if ((good = cancelJob(condorid, info))) { // The condor command worked fine
      if (logfile) {
        logGenericEvent(jccommon::cancelled_event, icid, logfile);
      }

      elog::cedglog << logger::setlevel(logger::verylow) << "Job " << sid
        << " successfully marked for removal.\n";
      this->jcr_repository->remove_by_condor_id(condorid);
    } else if (logfile) {
      logGenericEvent(jccommon::cannot_cancel_event, icid, logfile);
      this->jcr_logger->job_cancel_refused_event(info);
    }
  } else {
    elog::cedglog << logger::setlevel( logger::null ) << "not able to retrieve the condor ID.\n";
    this->jcr_logger->job_cancel_refused_event( "not able to retrieve the condor ID.");
    good = false;
  }

  return good;
}

bool JobControllerReal::cancel(int condorid, const char *logfile)
{
  bool                  good;
  string                sid(boost::lexical_cast<string>(condorid)), info;
  logger::StatePusher   pusher(clog, "JobControllerReal::cancel(...)");

  clog << logger::setlevel(logger::info)
       << "Asked to remove job: " << sid << " (by condor ID).\n";

  if ((good = cancelJob(sid, info))) {
    clog << logger::setlevel( logger::info ) << "Job " << sid << " successfully marked for removal.\n";

    if (logfile) {
      logGenericEvent(jccommon::cancelled_event, condorid, logfile);
    }
  }
  else if (logfile) {
    logGenericEvent(jccommon::cannot_cancel_event, condorid, logfile);
  }

  return good;
}

}; // namespace controller
} JOBCONTROL_NAMESPACE_END;
