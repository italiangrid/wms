/*
 * common_dag.h
 *
 * Copyright (c) 2001, 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#ifndef _GLITE_WMS_MANAGER_NS_COMMANDS_COMMON_DAG_H_
#define _GLITE_WMS_MANAGER_NS_COMMANDS_COMMON_DAG_H_

#include "common.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

    struct computeFileSize
    {
      computeFileSize(off_t& offt, bool& r) : size(offt), result(r)
      {
        result = true;
      }

      void operator() (const std::string& file)
      {
        int fd = -1;
        char buffer[512]; 
        stripProtocol( file.c_str(), buffer ); 
	// edglog(debug) << "File: " << buffer << std::endl;

        fd = open(buffer, O_RDONLY);
        if (fd != -1) {
          struct stat buf;
          if ( !fstat(fd, &buf) ) {
            size += buf.st_size;
          }
        } else {
	  // edglog(debug) << "Job: " << JobId << ". File " << buffer << " not found." << std::endl;
          result = false;
        }
      }
      off_t& size;
      bool& result;
    };


} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif
