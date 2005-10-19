// File: purger.cpp
// Author: Salvatore Monforte <salvatore.monforte@ct.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html
//
// $Id$

#include <boost/filesystem/operations.hpp> 
#include <boost/filesystem/exception.hpp>
#include <boost/scoped_ptr.hpp>

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"

#include "glite/wms/common/utilities/classad_utils.h"

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include "glite/lb/Job.h"
#include "glite/lb/JobStatus.h"

#include "purger.h"
#include "jp_upload_files.h"
#include "glite/lb/context.h"
#include "glite/lb/producer.h"
#include "glite/lb/LoggingExceptions.h"
#include <string>
#include <time.h>

namespace fs            = boost::filesystem;
namespace jobid         = glite::wmsutils::jobid;
namespace logger	= glite::wms::common::logger::threadsafe;
namespace logging       = glite::lb;
namespace configuration = glite::wms::common::configuration;
namespace utilities     = glite::wms::common::utilities;
#define edglog_fn(name) glite::wms::common::logger::StatePusher    pusher(logger::edglog, #name);

namespace glite {
namespace wms {
namespace purger {

//bool purgeStorageEx(const fs::path& p, int purge_threshold, bool fake_rm, bool navigating_dag, wll_log_function_type wll_log_function);
void purgeQuota(const fs::path& p);

namespace 
{
  const configuration::NSConfiguration* f_ns_conf = 0;
  std::string extract_staging_path(const fs::path& p, const jobid::JobId& jobid) 
  {
    int staging_path_length = p.native_file_string().length() -
      (jobid::get_reduced_part(jobid).length()+jobid::to_filename(jobid).length()+2);
    return p.native_file_string().substr(0,staging_path_length);
 }

  bool list_directory(const fs::path& p, std::vector<std::string>& v)
  {
    if(!fs::is_directory(p)) return false;
 
    fs::directory_iterator end_itr; // default construction yields past-the-end
    for ( fs::directory_iterator itr( p );
          itr != end_itr; ++itr ) {
      if (fs::exists(*itr)) try {
          if (!fs::is_directory( *itr )) 
            v.push_back(itr->native_file_string());
      }
      catch( fs::filesystem_error& e) {
        std::cerr << e.what() << std::endl;
      }
    }
    return true;
  }

  bool upload_input_sandbox(const fs::path& p)
  {
    bool result = false;
    jobid::JobId jobid( jobid::from_filename( p.leaf() ) );
    logging::JobStatus job_status = logging::Job( jobid ).status(0);
    std::string dg_jobid( jobid.toString() );
    std::string jdlstr(job_status.getValString(logging::JobStatus::JDL));
    boost::scoped_ptr<classad::ClassAd> jdlad;
    try {
      jdlad.reset(utilities::parse_classad(jdlstr));
      std::string job_provenance(utilities::evaluate_attribute(*jdlad, "JobProvenance"));
      std::string proxy_file((p/"user.proxy").native_file_string());
      std::vector<std::string> isb_files;
      list_directory(p/"input", isb_files);
      jp_upload_files uploader(dg_jobid, proxy_file, "", job_provenance);
      uploader(isb_files);
      result = true;
    } 
    catch(utilities::CannotParseClassAd& cpc) {
      logger::edglog << "Cannot parse logging::JobStatus::JDL";
    }
    catch(utilities::InvalidValue& ive) {
      logger::edglog << "JobProvenance attribute not found in JDL";
    }
    catch(jp_upload_files::init_context_exception& ice) {
      logger::edglog << "Failed to init JobProvenance context";
    }
    catch(jp_upload_files::importer_upload_exception& iup) {
      logger::edglog << "ISB upload failure: " << iup.what();
    }
    return result; 
  }

  bool purgeStorage(const fs::path& p, const edg_wll_Context& log_ctx, wll_log_function_type wll_log_function)
  { 
    bool result = false;
    try {
      if( !upload_input_sandbox(p) ) {
        logger::edglog << "input sandbox upload to JP failure";
      }
      fs::remove_all(p);
      purgeQuota(p);
      if( !(result = !(wll_log_function( log_ctx ) != 0)) ) logger::edglog << "log event to LB failed";
      else logger::edglog << "Ok";
    } 
    catch(fs::filesystem_error& fse) {
      logger::edglog << " fail " << fse.what(); 
    }
    return result;
  }

  class purge_dag_storage {
    std::string m_staging_path;
    int m_purge_threshold;
    bool m_fake_rm;
  public:
    purge_dag_storage(const std::string& staging_path,int purge_threshold, bool fake_rm) : 
      m_staging_path(staging_path), m_purge_threshold(purge_threshold), m_fake_rm(fake_rm)  {}
    void operator()(const std::string& str_jobid) {
      jobid::JobId jobid(str_jobid);
      // Creates the absolute path to the job staging location
      fs::path p;
      p = fs::path(m_staging_path, fs::native) /
          jobid::get_reduced_part (jobid) /
          jobid::to_filename (jobid);
      purgeStorageEx(p,m_purge_threshold,m_fake_rm, true);
    }
  };
}
  
void purgeQuota(const fs::path& p)
{
  std::string uid;
  fs::path quotafile(p / ".quid");
  std::ifstream quidfilestream( quotafile.native_file_string().c_str() );
  if( quidfilestream ) {
    quidfilestream >> uid;
    std::string cmdline("edg-wl-quota-adjustment -d - ");
    cmdline.append(uid);
    if (!system(cmdline.c_str())) {
      logger::edglog << " Error during quota purging." << std::endl;
    }	
  } 
}

bool purgeStorage(const jobid::JobId& jobid, const std::string& sandboxdir)
{
  edglog_fn(purgeStorage);	
  fs::path p;
  try {
    if( sandboxdir.empty() ) {
      if( !f_ns_conf ) {
	f_ns_conf = configuration::Configuration::instance() -> ns();
      }
      p = fs::path(f_ns_conf->sandbox_staging_path(), fs::native);
    }
    else p = fs::path(sandboxdir, fs::native);
    p = p / jobid::get_reduced_part( jobid ) / jobid::to_filename( jobid );
  }
  catch( jobid::JobIdException& jide ){
    logger::edglog << jide.what() << std::endl;
    return false; 	
  }
  catch( fs::filesystem_error& fse ) {
    logger::edglog << fse.what() << std::endl;
    return false;	
  }
  
  try {
    if( !upload_input_sandbox(p) ) {
      logger::edglog << "input sandbox upload to JP failure" << std::endl;
    }
    fs::remove_all( p );
    purgeQuota(p);
    return true;
  } 
  catch( fs::filesystem_error& fse ) {
    logger::edglog << fse.what() << std::endl;
  }
  return false;
}

bool purgeStorageEx(const fs::path& p,wll_log_function_type wll_log_function)
{
  return purgeStorageEx(p, 0, false, false, wll_log_function);
}

bool purgeStorageEx(const fs::path& p, int purge_threshold, bool fake_rm, bool navigating_dag, wll_log_function_type wll_log_function)
{
  edglog_fn(purgeStorageEx);	
  bool result = false;
  
    try {
    jobid::JobId jobid( jobid::from_filename( p.leaf() ) );
    logging::JobStatus job_status = logging::Job( jobid ).status(logging::Job::STAT_CHILDSTAT | logging::Job::STAT_CHILDREN);
    std::string dg_jobid( jobid.toString() );
    std::string seqcode;
    seqcode.assign("UI=000003:NS=0000096669:WM=000000:BH=0000000000:JSS=000000:LM=000000:LRMS=000000:APP=000000");
    logger::edglog << dg_jobid << " ->";
    //                        
    // Initialize the logging context
    // 
    edg_wll_Context      log_ctx;
    if( edg_wll_InitContext( &log_ctx ) != 0 ||
	edg_wll_SetParam( log_ctx, EDG_WLL_PARAM_SOURCE,  EDG_WLL_SOURCE_NETWORK_SERVER  ) != 0 ||
	edg_wll_SetLoggingJob( log_ctx, jobid, seqcode.c_str(), EDG_WLL_SEQ_NORMAL ) != 0 )  {
      
      char 	*et,*ed;
      edg_wll_Error(log_ctx,&et,&ed);
      
      logger::edglog << " unable to initialize logging context " << et << ": " << ed;
      free(et); free(ed);
    }	  
    // Reads the TYPE of the JOB...
    bool is_jobtype_dag = job_status.getValInt(logging::JobStatus::JOBTYPE) == logging::JobStatus::JOBTYPE_DAG;
    bool has_parent_job = false;
    try {
      const jobid::JobId parent_jobid = job_status.getValJobId(logging::JobStatus::PARENT_JOB);
      if (parent_jobid != 0) {
        has_parent_job = true;
        // If the job is a dag node and we are not purging a dag the node should be skipped...
        if (!navigating_dag) {
          // Before skipping let's check if the node is an orphan...
          fs::path pp; 
          pp = fs::path(extract_staging_path(p,jobid), fs::native) /
               jobid::get_reduced_part(parent_jobid) / 
               jobid::to_filename(parent_jobid);
          if (!fs::exists(pp)) {
            logger::edglog << " removing orphan dag node: ";
            if (!fake_rm ) result = purgeStorage(p, log_ctx, wll_log_function);
            logger::edglog << std::endl;
            return result;
          }
          logger::edglog << " skipping dag node" << std::endl;
          return false;
        }
      }
    }
    catch(...) {
    }
    
    // if we are purging a dag node we have to force it's removal not depending
    // on its state...since the purging condition are met for the DAG.
    if (has_parent_job && navigating_dag) {
      	
      logger::edglog << " removing dag node: ";
      if( !fake_rm )  result = purgeStorage(p, log_ctx, wll_log_function);
      logger::edglog << std::endl;
      return result;
    }
        
    switch( job_status.status ) {

    case logging::JobStatus::CANCELLED:
    case logging::JobStatus::CLEARED:
	
      // If the job is a DAG we have to recursively purge (if necessary) each CHILD   	
      if (is_jobtype_dag) {
		
	std::vector<std::string> children = job_status.getValStringList(logging::JobStatus::CHILDREN);
	logger::edglog << " browsing: #" << children.size() << " node(s)" << std::endl;
	
	std::string staging_path(extract_staging_path(p, jobid));
	std::for_each(children.begin(), children.end(), 
		      purge_dag_storage(staging_path, purge_threshold,fake_rm));
	
	logger::edglog << dg_jobid << " ->";
      }
      
      logger::edglog << " removing ";

      switch(job_status.status) {
        case logging::JobStatus::CANCELLED: logger::edglog << "[CANCELLED] "; break;
        case logging::JobStatus::CLEARED:   logger::edglog << "[CLEARED]   "; break;
      }

      if( !fake_rm ) result = purgeStorage(p, log_ctx, wll_log_function);
    break;
    case logging::JobStatus::ABORTED:
    case logging::JobStatus::DONE: 
      {
	struct timeval last_update_time = 
	  job_status.getValTime(logging::JobStatus::LAST_UPDATE_TIME);
        time_t now;
	time( &now);
	if( now -  last_update_time.tv_sec > purge_threshold ) {		
	  
	  // If the job is a DAG we have to recursively purge (if necessary) each CHILD   	
	  if (is_jobtype_dag) {
	    
	    std::vector<std::string> children = job_status.getValStringList(logging::JobStatus::CHILDREN);
	    logger::edglog << " browsing #" << children.size() << " node(s)" << std::endl;
	    
	    std::string staging_path(extract_staging_path(p, jobid));
	    std::for_each(children.begin(), children.end(), 
			  purge_dag_storage(staging_path, purge_threshold,fake_rm));
	    
	    logger::edglog << dg_jobid << " ->";
	  }
	  
	  logger::edglog << " removing ";

	  switch(job_status.status) {
            case logging::JobStatus::ABORTED:   logger::edglog << "[ABORTED]   "; break;
            case logging::JobStatus::DONE:      logger::edglog << "[DONE]      "; break;
          }

	  if( !fake_rm ) result = purgeStorage(p, log_ctx, wll_log_function);
	}
	else logger::edglog << " skipping ";	
      }
    break;
    default:
      logger::edglog << " skipping ";
    }
    logger::edglog << std::endl;   
  }
  catch( fs::filesystem_error& fse ) {
    logger::edglog << fse.what() << std::endl;
  }
  catch( jobid::JobIdException& jide ) {
    logger::edglog << jide.what() << std::endl;
  }
  catch( std::exception&e ) {
    logger::edglog << e.what() << std::endl;
  }
  return result;
}

} // namespace purger
} // namespace wms
} // namesapce glite
