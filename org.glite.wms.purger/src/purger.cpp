// File: purger.cpp
// Author: Salvatore Monforte <salvatore.monforte@ct.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html
//
// $Id$

#include <boost/filesystem/operations.hpp> 
#include <boost/filesystem/exception.hpp>

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include "glite/lb/Job.h"
#include "glite/lb/JobStatus.h"
#include "glite/lb/context.h"
#include "glite/lb/producer.h"

#include <string>
#include <time.h>

namespace fs            = boost::filesystem;
namespace jobid         = glite::wmsutils::jobid;
namespace logger	= glite::wms::common::logger::threadsafe;
namespace logging       = glite::lb;
namespace configuration = glite::wms::common::configuration;

#define edglog_fn(name) glite::wms::common::logger::StatePusher    pusher(logger::edglog, #name);

namespace glite {
namespace wms {
namespace purger {

namespace 
{
  const configuration::NSConfiguration* f_ns_conf = 0;
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
	  p /= jobid::get_reduced_part( jobid );
	  p /= jobid::to_filename( jobid );
  }
  catch( jobid::JobIdException& jide )
  {
    logger::edglog << jide.what() << std::endl;
    return false; 	
  }
  catch( fs::filesystem_error& fse )
  {
     logger::edglog << fse.what() << std::endl;
     return false;	
  }
  
  if (fs::remove_all( p )) {
    purgeQuota( p ); 
    return true;
  }
  return false;
}


bool purgeStorageEx(const fs::path& p, int purge_threshold, bool fake_rm)
{
  edglog_fn(purgeStorageEx);	
  bool result = false;
  
  try {
			
    jobid::JobId jobid( jobid::from_filename( p.leaf() ) ); 	
    logging::JobStatus job_status = logging::Job( jobid ).status(0);
    std::string dg_jobid( jobid.toString() );
    std::string seqcode;
    logger::edglog << dg_jobid << " ->";
    fs::path     seqfile( p / ".edg_wll_seq" );
    std::ifstream seqfilestream( seqfile.native_file_string().c_str() );
    if( seqfilestream ) {
      seqfilestream >> seqcode;
    }	
    else {
      logger::edglog << " .edg_wll_seq does not exist " << std::endl; 
      bool ret = (!fake_rm && fs::remove_all( p ));
      purgeQuota( p );
      return ret;
    }
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
    
    switch( job_status.status ) {
  
    case logging::JobStatus::CLEARED:	
			
      logger::edglog << " removing";
      
      if( !fake_rm && fs::remove_all( p ) ) {	
	
        result = true;
	
	if( edg_wll_LogClearTIMEOUT( log_ctx ) != 0) logger::edglog << ": edg_wll_LogClearTIMEOUT failed";
	else {
		logger::edglog << ": Ok";
	}
      }
      else {
        logger::edglog << ": fail";	
      }		
      break;
    case logging::JobStatus::ABORTED:
    case logging::JobStatus::DONE: 
      {
	struct timeval last_update_time = 
	  job_status.getValTime(logging::JobStatus::LAST_UPDATE_TIME);
        time_t now;
	time( &now);
	
	if( now -  last_update_time.tv_sec > purge_threshold ) {		
	  
	  logger::edglog << " removing";
	  
	  if( !fake_rm && fs::remove_all( p ) ) {

      	    result = true;
  	    
	    if( edg_wll_LogClearTIMEOUT( log_ctx ) !=0 ) logger::edglog << ": edg_wll_LogClearTIMEOUT failed";
	    else {
		    logger::edglog << ": Ok";
	    }
	  }
	  else {	
	    logger::edglog << ": fail"; 
	  } 	
	}
	else logger::edglog << " skipping ";	
      }
    break;
    default: ; logger::edglog << " skipping ";
    }
    logger::edglog << std::endl;   
  }
  catch( fs::filesystem_error& fse ) 
    {
      logger::edglog << fse.what() << std::endl;
    }
  catch( jobid::JobIdException& jide )
    {
      logger::edglog << jide.what() << std::endl;
    }
  catch( std::exception&e ) 
    {
      logger::edglog << e.what() << std::endl;
    }
  if (result) purgeQuota( p ); 
  return result;
}

} // namespace purger
} // namespace wms
} // namesapce glite

