// File: purger.cpp
// Author: Salvatore Monforte <salvatore.monforte@ct.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html
//
// $Id$

#include <boost/filesystem/operations.hpp> 
#include <boost/filesystem/exception.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"

#include "glite/wmsutils/classads/classad_utils.h"

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/utilities/scope_guard.h"

#include "glite/lb/Job.h"
#include "lb_utils.h"
#include "purger.h"

#ifndef GLITE_WMS_DONT_HAVE_JP
#include "jp_upload_files.h"
#endif

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
namespace utils		= glite::wmsutils::classads;

#define edglog_fn(name) glite::wms::common::logger::StatePusher    pusher(logger::edglog, #name);

namespace glite {
namespace wms {
namespace purger {

void purgeQuota(const fs::path& p);

namespace 
{
  std::string const f_sequence_code(
    "UI=000009:NS=0000096669:WM=000000:BH=0000000000:JSS=000000:LM=000000:LRMS=000000:APP=000000" 
  );

  const configuration::NSConfiguration* f_ns_conf = 0;
  const configuration::Configuration* f_conf = 0;

  std::string 
  extract_staging_path(const fs::path& p, const jobid::JobId& jobid) 
  {
    int staging_path_length = p.native_file_string().length() -
      (jobid::get_reduced_part(jobid).length() + 
       jobid::to_filename(jobid).length()+2);
    return p.native_file_string().substr(0,staging_path_length);
 }
 
 std::string
 get_user_x509_proxy(jobid::JobId const& jobid)
 {
   if (!f_conf) {
     f_conf = configuration::Configuration::instance();
     assert(f_conf);
   } 
   if (!f_ns_conf) {
     f_ns_conf = f_conf->ns();
     assert(f_ns_conf);
   }

   std::string x509_proxy(f_ns_conf->sandbox_staging_path());
   x509_proxy += "/"
     + jobid::get_reduced_part(jobid)
     + "/"
     + jobid::to_filename(jobid)
     + "/user.proxy";

    return x509_proxy;
  }
  
  std::string 
  get_host_x509_proxy()
  {
   if (!f_conf) {
     f_conf = configuration::Configuration::instance();
     assert(f_conf);
   }
   return f_conf->common()->host_proxy_file();
  }
  
  bool create_context_from_user_x509_proxy(
    ContextPtr& log_ctx,
    jobid::JobId const& id,
    std::string const& sequence_code
  )
  {
    bool result = true;
    char* e = std::getenv("X509_USER_CERT");
      try {
        log_ctx = create_context(
          id,
          !e?get_user_x509_proxy(id):std::string(e),
          f_sequence_code
        );
      }
      catch (CannotCreateLBContext& e) {
        logger::edglog << id.toString()  << " ->"
          << "CannotCreateLBContext from user proxy, error code #" 
          << e.error_code() 
          << std::endl;
        result = false;
      }
    return result;
  }

  bool create_context_from_host_x509_proxy(
    ContextPtr& log_ctx,
    jobid::JobId const& id,
    std::string const& sequence_code
  )
  {
    bool result = true;
      try {
        log_ctx = create_context(
          id,
          get_host_x509_proxy(),
          f_sequence_code
        );
      }
      catch (CannotCreateLBContext& e) {
        logger::edglog << id.toString()  << " ->"
          << "CannotCreateLBContext from host proxy, error code #" 
          << e.error_code()
          << std::endl;
        result = false;
      }
    return result;
  }

  bool query_job_status(edg_wll_JobStat& job_status, jobid::JobId const& jobid, ContextPtr const& log_ctx)
  {
    edg_wll_InitStatus(&job_status);
    if (edg_wll_JobStatus(
        log_ctx.get(),jobid,
        EDG_WLL_STAT_CLASSADS | EDG_WLL_STAT_CHILDREN,
        &job_status
      )) {
        char  *etxt,*edsc;
        edg_wll_Error(log_ctx.get(),&etxt,&edsc);
        logger::edglog << jobid.toString()  << " -> "
          << "edg_wll_JobStat " 
          << etxt
          << " " << edsc
          << std::endl;
        free(etxt); free(edsc);
        return false;
    }
    return true;
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
#ifndef GLITE_WMS_DONT_HAVE_JP
    bool result = false;
    try {
      ContextPtr lb_context;
      jobid::JobId jobid( jobid::from_filename( p.leaf() ) );
      
      lb_context = create_context(
        jobid,
        get_user_x509_proxy(jobid),
        f_sequence_code
      );
   
      boost::scoped_ptr<classad::ClassAd> jdlad;
      std::string jdlstr(
        get_original_jdl(lb_context.get(), jobid)
      );
      jdlad.reset(utils::parse_classad(jdlstr));
      
      std::string job_provenance(
        utils::evaluate_attribute(*jdlad, "JobProvenance")
      );
      
      fs::path proxy_path(
        p / fs::path("user.proxy", fs::native)
      );
      std::string proxy_file(proxy_path.native_file_string());
      std::vector<std::string> isb_files;
      list_directory(p/fs::path("input", fs::native), isb_files);
      jp_upload_files uploader(
        jobid.toString(), proxy_file, "", job_provenance
      );
      fs::path jdl_original_path(
        p / fs::path("JDLOriginal", fs::native)
      );
      
      if (fs::exists(jdl_original_path)) {
        isb_files.push_back(jdl_original_path.native_file_string());
      }

      fs::path jdl_to_start_path(
        p / fs::path("JDLToStart", fs::native)
      );
      if (fs::exists(jdl_to_start_path)) {
        isb_files.push_back(jdl_to_start_path.native_file_string());
      }

      uploader(isb_files);
      result = true;
    }
    catch(utils::CannotParseClassAd& cpc) {
      logger::edglog << "Cannot parse logging::JobStatus::JDL";
    }
    catch(utils::InvalidValue& ive) {
      logger::edglog << "JobProvenance attribute not found in JDL";
    }
    catch(jp_upload_files::init_context_exception& ice) {
      logger::edglog << "Failed to init JobProvenance context";
    }
    catch(jp_upload_files::importer_upload_exception& iup) {
      logger::edglog << "ISB upload failure: " << iup.what();
    }
    return result;
#else
  return true; 
#endif
  }

  bool 
  purgeStorage(const fs::path& p, 
    const edg_wll_Context& log_ctx, wll_log_function_type wll_log_function
  )
  { 
    bool result = false;
    try {
      if( !upload_input_sandbox(p) ) {
        logger::edglog << " [JP ISB upload failed] ";
      }
      fs::remove_all(p);
      purgeQuota(p);
      if( !(result = !(wll_log_function( log_ctx ) != 0)) ) {
        logger::edglog << " [LB event logging failed] " << get_lb_message(log_ctx); 
      }
      else logger::edglog << " Ok ";
    } 
    catch(fs::filesystem_error& fse) {
      logger::edglog << " [" << fse.what() << "]"; 
    }
    return result;
  }

  class purge_dag_storage {
    std::string m_staging_path;
    int m_purge_threshold;
    bool m_fake_rm;
    wll_log_function_type m_wll_log_function;
  public:
    purge_dag_storage(const std::string& staging_path,
      int purge_threshold, bool fake_rm, wll_log_function_type wll_log_function
    ) : m_staging_path(staging_path), m_purge_threshold(purge_threshold),
      m_fake_rm(fake_rm), m_wll_log_function(wll_log_function)  {}
    
    void operator()(const std::string& str_jobid) {
      jobid::JobId jobid(str_jobid);
      // Creates the absolute path to the job staging location
      fs::path p(
        fs::path(m_staging_path, fs::native) /
        fs::path(jobid::get_reduced_part(jobid), fs::native) /
        fs::path(jobid::to_filename (jobid), fs::native)
      );
      purgeStorageEx(p,m_purge_threshold,m_fake_rm, true, m_wll_log_function);
    }
  };
}
  
void purgeQuota(const fs::path& p)
{
  std::string uid;
  fs::path quotafile(
     p / fs::path(".quid", fs::native) 
  );
  std::ifstream quidfilestream( quotafile.native_file_string().c_str() );
  if( quidfilestream ) {
    quidfilestream >> uid;
    std::string cmdline("edg-wl-quota-adjustment -d - ");
    cmdline.append(uid);
    if (!system(cmdline.c_str())) {
      logger::edglog << " [Error during quota purging] ";
    }	
  } 
}

bool purgeStorage(const jobid::JobId& jobid, const std::string& sandboxdir)
{
  edglog_fn(purgeStorage);
  logger::edglog << "Starting purging..." << std::endl;
  fs::path p;
  try {
    if( sandboxdir.empty() ) {

      if( !f_ns_conf ) {

	f_ns_conf = configuration::Configuration::instance() -> ns();
      }

      p = fs::path(f_ns_conf->sandbox_staging_path(), fs::native);
    }
    else {

      p = fs::path(sandboxdir, fs::native);
    }
    p = p / fs::path(jobid::get_reduced_part(jobid), fs::native) / 
      fs::path(jobid::to_filename(jobid), fs::native);
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
      logger::edglog << " [ ISB upload to JP failure" << std::endl;
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



  
bool 
purgeStorageEx(const fs::path& p, 
  int purge_threshold, bool fake_rm, bool navigating_dag, 
  wll_log_function_type wll_log_function)
{
  edglog_fn(purgeStorageEx);	
  bool result = false;
  try {
    jobid::JobId jobid( jobid::from_filename( p.leaf() ) );
   
    ContextPtr log_ctx;
    edg_wll_JobStat job_status;
    edg_wll_InitStatus(&job_status);

    bool status_retrieved = (
      create_context_from_user_x509_proxy(
        log_ctx, jobid, f_sequence_code
      ) 
      &&
      query_job_status(job_status, jobid, log_ctx)
    )
    ||
    (
      create_context_from_host_x509_proxy(
        log_ctx, jobid, f_sequence_code
      )
      &&
      query_job_status(job_status, jobid, log_ctx)
    );

    utilities::scope_guard free_job_status(
      boost::bind(edg_wll_FreeStatus, &job_status)
    );
  
    logger::edglog << jobid.toString()  << " ->";
    
    // Reads the TYPE of the JOB...
    bool is_jobtype_dag = 
      (job_status.jobtype == EDG_WLL_STAT_DAG ) || 
      (job_status.jobtype == EDG_WLL_STAT_COLLECTION);

    bool has_parent_job = false;
    try {
      const jobid::JobId parent_jobid = job_status.parent_job;
      if (parent_jobid != 0) {
        has_parent_job = true;
        // If the job is a dag node and we are not purging a dag the node should be skipped...
        if (!navigating_dag) {
          // Before skipping let's check if the node is an orphan...
          fs::path pp = 
            fs::path(extract_staging_path(p,jobid), fs::native) /
            fs::path(jobid::get_reduced_part(parent_jobid), fs::native) / 
            fs::path(jobid::to_filename(parent_jobid), fs::native);	
          
          if (!fs::exists(pp)) {
            logger::edglog << " removing orphan dag node: ";
            if (!fake_rm ) result = purgeStorage(p, log_ctx.get(), wll_log_function);
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
      if( !fake_rm )  result = purgeStorage(p, log_ctx.get(), wll_log_function);
      logger::edglog << std::endl;
      return result;
    }
        
    switch( job_status.state ) {

    case EDG_WLL_JOB_CANCELLED:
    case EDG_WLL_JOB_CLEARED:
	
      // If the job is a DAG we have to recursively purge (if necessary) each CHILD   	
      if (is_jobtype_dag) {
		
	std::vector<std::string> children;
	std::copy(
          &job_status.children[0],
          &job_status.children[job_status.children_num],
          std::back_inserter(children)
        );

        logger::edglog << " browsing: #" << children.size() << " node(s)" << std::endl;
	
	std::string staging_path(extract_staging_path(p, jobid));
	
        std::for_each(
          children.begin(), 
          children.end(), 
	  purge_dag_storage(staging_path, purge_threshold,fake_rm, wll_log_function)
        );
	
	logger::edglog << jobid.toString() << " ->";
      }
      
      logger::edglog << " removing ";

      switch(job_status.state) {
        case EDG_WLL_JOB_CANCELLED: logger::edglog << "[CANCELLED] "; break;
        case EDG_WLL_JOB_CLEARED:   logger::edglog << "[CLEARED]   "; break;
      }

      if( !fake_rm ) result = purgeStorage(p, log_ctx.get(), wll_log_function);
    break;
    case EDG_WLL_JOB_ABORTED:
    case EDG_WLL_JOB_DONE: 
      {
        time_t now;
	time( &now);
	if( now -  job_status.lastUpdateTime.tv_sec > purge_threshold ) {		
	  
	  // If the job is a DAG we have to recursively purge (if necessary) each CHILD   	
	  if (is_jobtype_dag) {
	    
            std::vector<std::string> children;
            std::copy(
              &job_status.children[0],
              &job_status.children[job_status.children_num],
              std::back_inserter(children)
            );

	    logger::edglog << " browsing #" << children.size() << " node(s)" << std::endl;
	    
	    std::string staging_path(extract_staging_path(p, jobid));
	    std::for_each(
              children.begin(), 
              children.end(), 
              purge_dag_storage(staging_path, purge_threshold,
                fake_rm,wll_log_function)
            );
	    
	    logger::edglog << jobid.toString() << " ->";
	  }
	  
	  logger::edglog << " removing ";

	  switch(job_status.state) {
            case EDG_WLL_JOB_ABORTED:   logger::edglog << "[ABORTED]   "; break;
            case EDG_WLL_JOB_DONE:      logger::edglog << "[DONE]      "; break;
          }

	  if( !fake_rm ) result = purgeStorage(p, log_ctx.get(), wll_log_function);
	}
	else logger::edglog << " skipping: purging threshold not overcome!";	
      }
    break;
    default:
      logger::edglog << " skipping: " << edg_wll_StatToString(job_status.state);
    }
    logger::edglog << std::endl;   
  }
  catch (CannotCreateLBContext& e) {
    logger::edglog << e.what() << std::endl;
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
