/* File: globus_ftp_exists.cpp
 * Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

//  $Id$

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>

#include <iostream>
#include <string>

#include "globus_common.h"
#include "globus_io.h"
#include "globus_ftp_client.h"

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"
#include "globus_ftp_macros.h"

namespace glite {
namespace wms {
namespace common {
namespace utilities {
namespace globus {

namespace logger = glite::wms::common::logger;

namespace {	
static globus_mutex_t lock;
static globus_cond_t cond;
static globus_bool_t done;

// The complete_callback will be invoked with the result of the existence check passed as a globus error object, or GLOBUS_SUCCESS.
static void done_cb(void * user_arg, 
		    globus_ftp_client_handle_t * handle,
		    globus_object_t * err)
{
  if(err) {
	  edglog(warning) << globus_object_printable_to_string(err) << std::flush;
          *(globus_bool_t*)user_arg = GLOBUS_FALSE;
  }
  else    *(globus_bool_t*)user_arg = GLOBUS_TRUE;

  globus_mutex_lock(&lock);
  done = GLOBUS_TRUE;
  globus_cond_signal(&cond);
  globus_mutex_unlock(&lock);
}

}

bool exists(const std::string& dst)
{ 
  globus_ftp_client_handle_t              handle;
  globus_result_t                         result;
  globus_bool_t                           operation_succeeded = GLOBUS_FALSE;
      
  //globus_module_activate(GLOBUS_FTP_CLIENT_MODULE);
  globus_mutex_init(&lock, GLOBUS_NULL);
  globus_cond_init(&cond, GLOBUS_NULL);
  if( (result = globus_ftp_client_handle_init(&handle,  GLOBUS_NULL)) == GLOBUS_SUCCESS ) {
          
	  done = GLOBUS_FALSE;
    	  globus_ftp_client_operationattr_t src;
	  globus_ftp_client_operationattr_init(&src);
		  
	  result = globus_ftp_client_exists(&handle,
			  dst.c_str(),
			  &src,
			  done_cb,
			  (void *)&operation_succeeded);
  }
  if(result != GLOBUS_SUCCESS) {
    globus_object_t* err;
    err = globus_error_get(result);
    edglog(warning) << globus_object_printable_to_string(err) << std::flush;
    done = GLOBUS_TRUE;
  }
  
  globus_mutex_lock(&lock);
  while(!done) {
    globus_cond_wait(&cond, &lock);
  }
  globus_mutex_unlock(&lock);
  
  globus_ftp_client_handle_destroy(&handle);
//  globus_module_deactivate(GLOBUS_FTP_CLIENT_MODULE);
  return  (operation_succeeded == GLOBUS_TRUE);
}
} // globus namespace closure
} // utilities namespace closure
} // common namespace closure
} // wms namespace closure
} // glite namespace closure
