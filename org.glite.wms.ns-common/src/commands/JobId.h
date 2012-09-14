/**
 * File: JobId.h
 *
 * @author Marco Pappalardo@ct.infn.it
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id: 

#ifndef _GLITE_WMS_NS_COMMON_JOBID_H_
#define _GLITE_WMS_NS_COMMON_JOBID_H_

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"

namespace glite {
namespace wms {
namespace ns {    
namespace jobid {

  inline std::string to_filename(glite::wmsutils::jobid::JobId j, 
				 int level = 0,
				 bool extended_path = true) {
    std::string path( get_reduced_part(j, level) );
    if ( extended_path ) path.append(std::string("/") + glite::wmsutils::jobid::to_filename(j));
    return path;
  }
		     
} // namespace jobid
} // namespace ns
} // namespace wms
} // namespace glite

#endif
