/*
 * File: logging_fn.cpp
 * Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
 * Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$ 
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "glite/wms/common/logger/edglog.h" 
#include "glite/wms/common/logger/manipulators.h" 
#include "glite/wms/common/configuration/CommonConfiguration.h" 
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/lb/producer.h" 
#include "glite/wms/ns-common/logging.h"
#include "glite/wms/common/utilities/boost_fs_add.h"

using namespace std;
namespace fs = boost::filesystem;
namespace configuration = glite::wms::common::configuration; 
namespace logger        = glite::wms::common::logger; 


namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace daemon {

  static void testAndLog( edg_wll_Context ctx, int &code, bool &with_hp, int &lap)
  { 
    edglog_fn("NS2WM::test&Log");
    const configuration::CommonConfiguration *conf = configuration::Configuration::instance()->common();
    int          ret;
    std::string  host_proxy; 
    
    if( code ) { 
 
      switch( code ) { 
      case EINVAL: 
	edglog(critical) << "Critical error in L&B calls: EINVAL." << std::endl;
	code = 0; // Don't retry... 
	break; 
	
      case EDG_WLL_ERROR_GSS:
	edglog(severe) << "Severe error in SSL layer while communicating with L&B daemons." << std::endl;
	
	if( with_hp ) { 
	  edglog(severe) << "The log with the host certificate has just been done. Giving up." << std::endl; 
	  code = 0; // Don't retry... 
	} 
	else { 
	  edglog(info) << "Retrying using host proxy certificate..." << std::endl; 
	  host_proxy = conf->host_proxy_file(); 
 
	  if( host_proxy.length() == 0 ) { 
	    edglog(warning) << "Host proxy file not set inside configuration file." << std::endl 
			    << "Trying with a default NULL and hoping for the best." << std::endl; 
	    ret = edg_wll_SetParam( ctx, EDG_WLL_PARAM_X509_PROXY, NULL ); 
	  } 
	  else { 
	    edglog(info) << "Host proxy file found = \"" << host_proxy << "\"." << std::endl; 
	    ret = edg_wll_SetParam( ctx, EDG_WLL_PARAM_X509_PROXY, host_proxy.c_str() ); 
	  } 
 
	  if( ret ) { 
	    edglog(severe) << "Cannot set the host proxy inside the context. Giving up." << std::endl;
	    code = 0; // Don't retry.
	  } 
	  else with_hp = true; // Set and retry (code is still != 0)
	}
	
	break; 
	
      default:
	if( ++lap > 3 ) {
	  edglog(error) << "L&B call retried " << lap << " times always failed." << std::endl
			<< "Ignoring." << std::endl; 
	  code = 0; // Don't retry anymore
	} 
	else {
	  edglog(warning) << "L&B call got a transient error. Waiting 60 seconds and trying again." << std::endl;
	  edglog(info) << "Try n. " << lap << "/3" << std::endl;
	  sleep( 60 ); 
	} 
	
	break; 
      } 
    }
    else // The logging call worked fine, do nothing
      edglog(debug) << "L&B call succeeded." << std::endl;
    
    return; 
  } 

  static void reset_user_proxy( edg_wll_Context ctx, const std::string &proxyfile ) 
  { 
    edglog_fn("CFSI::resetUserProxy");
    bool    erase = false;
    int     res;
 
    if( proxyfile.size() ) {
      fs::path pf(fs::normalize_path(proxyfile), fs::native);
 
      if( fs::exists(pf) ) {
	res = edg_wll_SetParam( ctx, EDG_WLL_PARAM_X509_PROXY, proxyfile.c_str() );
	if( res ) edglog(severe) << "Cannot set proxyfile path inside context." << std::endl;
      } 
      else erase = true; 
    }
    else if( proxyfile.size() == 0 ) erase = true;
 
    if( erase ) { 
      res = edg_wll_SetParam( ctx, EDG_WLL_PARAM_X509_PROXY, NULL ); 
      if( res ) edglog(severe) << "Cannot reset proxyfile path inside context." << std::endl;; 
    } 
 
  } 

  bool LogEnqueuedJob(edg_wll_Context ctx, std::string jdl, const char* file_queue,
		      bool mode, const char* reason, bool retry){
    int i=0;
    edglog_fn("NS2WM::LogEnqueuedJobN");
    edglog(fatal) << "Logging Enqueued Job." << std::endl;
    bool logged = false; 
 
    for (; i < 3 && !logged && retry; i++) {
#ifndef GLITE_WMS_HAVE_LBPROXY
      logged = !edg_wll_LogEnQueued (ctx,
				     file_queue,
				     jdl.c_str(),
				     (mode ? "OK" : "FAIL"),
				     reason);
#else
      logged = !edg_wll_LogEnQueuedProxy (ctx,
					  file_queue,
					  jdl.c_str(),
					  (mode ? "OK" : "FAIL"),
					  reason);
#endif
      if (!logged && (i<2) && retry) {
        edglog(info) << "Failed to log Enqueued Job. Sleeping 60 seconds before retry." << std::endl;
        sleep(60); 
      } 
    } 
 
    if ((retry && (i>=3)) || (!retry && (i>0)) ) { 
      edglog(severe) << "Error while logging Enqueued Job." << std::endl;
      return false; 
    } 
 
    edglog(debug) << "Logged." << std::endl;
    edg_wll_FreeContext(ctx); 
    return true; 
  } 

  bool LogEnqueuedJob(edg_wll_Context ctx, std::string jdl, 
		      std::string user_proxy, const char* file_queue,
		      bool mode, const char* reason, bool retry, bool test) {
 
    if (!test) return LogEnqueuedJob(ctx, jdl, file_queue, mode, reason, retry);
    edglog_fn("NS2WM::LogEnqueuedJobE"); 
    edglog(fatal) << "Logging Enqueued Job." << std::endl;
 
    int res; 
    bool with_hp = false;
    int lap = 0;
    reset_user_proxy(ctx, user_proxy);
 
    do { 
#ifndef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogEnQueued (ctx,
			         file_queue, 
			         jdl.c_str(), 
			         (mode ? "OK" : "FAIL"),
			         reason);
#else
      res = edg_wll_LogEnQueuedProxy (ctx,
				      file_queue, 
				      jdl.c_str(), 
				      (mode ? "OK" : "FAIL"),
				      reason);
#endif
      testAndLog( ctx, res, with_hp, lap ); 
    } while( res != 0 );
 
    return true;
  }

  bool LogEnqueuedJob_Start(edg_wll_Context ctx, std::string jdl, const char* file_queue,
		            const char* reason, bool retry){
    int i=0;
    edglog_fn("NS2WM::LogEnqueuedJobStartN");
    edglog(fatal) << "Logging Enqueued Job Start." << std::endl;
    bool logged = false; 
 
    for (; i < 3 && !logged && retry; i++) {
#ifndef GLITE_WMS_HAVE_LBPROXY
      logged = !edg_wll_LogEnQueued (ctx,
				     file_queue,
				     jdl.c_str(),
				     "START",
				     reason);
#else
      logged = !edg_wll_LogEnQueuedProxy (ctx,
					  file_queue,
					  jdl.c_str(),
					  "START",
					  reason);
#endif
      if (!logged && (i<2) && retry) {
        edglog(info) << "Failed to log Enqueued Job Start. Sleeping 60 seconds before retry." << std::endl;
        sleep(60); 
      } 
    } 
 
    if ((retry && (i>=3)) || (!retry && (i>0)) ) { 
      edglog(severe) << "Error while logging Enqueued Job." << std::endl;
      return false; 
    } 
 
    edglog(debug) << "Logged." << std::endl;
    edg_wll_FreeContext(ctx); 
    return true; 
  } 

  bool LogEnqueuedJob_Start(edg_wll_Context ctx, std::string jdl, 
		            std::string user_proxy, const char* file_queue,
		            const char* reason, bool retry, bool test) {
 
    if (!test) return LogEnqueuedJob_Start(ctx, jdl, file_queue, reason, retry);
    edglog_fn("NS2WM::LogEnqueuedJobStartE"); 
    edglog(fatal) << "Logging Enqueued Job Start." << std::endl;
 
    int res; 
    bool with_hp = false;
    int lap = 0;
    reset_user_proxy(ctx, user_proxy);
 
    do { 
#ifndef GLITE_WMS_HAVE_LBPROXY
      res = edg_wll_LogEnQueued (ctx,
			         file_queue, 
			         jdl.c_str(), 
			         "START",
			         reason);
#else
      res = edg_wll_LogEnQueuedProxy (ctx,
				      file_queue, 
				      jdl.c_str(), 
				      "START",
				      reason);
#endif
      testAndLog( ctx, res, with_hp, lap ); 
    } while( res != 0 );
 
    return true;
  }

} // namespace daemon
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite
