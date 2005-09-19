// File: purger.h
// Author: Salvatore Monforte <salvatore.monforte@ct.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html
//
// $Id$

#ifndef GLITE_WMS_PURGER_PURGER_H
#define GLITE_WMS_PURGER_PURGER_H

#include <boost/filesystem/operations.hpp> 
#include <boost/function.hpp>
#include "glite/lb/context.h"
#include "glite/lb/producer.h"
namespace glite {

namespace wmsutils {
namespace jobid {
class JobId;
} // namespace jobid
} // namespace wmsutils

namespace wms {
namespace purger {
 typedef boost::function<int(edg_wll_Context)> wll_log_function_type;

 bool purgeStorageEx(const boost::filesystem::path&, 
   int purge_threshold = 0, 
   bool fake_rm = false, 
   bool navigating_dag = false,
   wll_log_function_type = edg_wll_LogClearTIMEOUT);

 bool purgeStorageEx(const boost::filesystem::path& p,wll_log_function_type wll_log_function);

 bool purgeStorage(const glite::wmsutils::jobid::JobId&, const std::string& sandboxdir = "");
 
} // namespace purger
} // namespace wms
} // namespace glite

// Local Variables:
// mode: c++
// End:
// 

#endif /* GLITE_WMS_PURGER_PURGER_H */
