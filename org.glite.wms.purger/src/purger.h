// File: purger.h
// Author: Salvatore Monforte <salvatore.monforte@ct.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html
//
// $Id$

#ifndef GLITE_WMS_PURGER_PURGER_H
#define GLITE_WMS_PURGER_PURGER_H

#include <boost/utility.hpp>
#include <boost/type_traits.hpp>
#include <boost/shared_ptr.hpp>

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

 typedef boost::shared_ptr<boost::remove_pointer<edg_wll_Context>::type> ContextPtr;
 
 class Purger : public boost::noncopyable
 {
   bool m_have_lb_proxy;
   boost::function<int(edg_wll_Context)> m_logging_fn;
   time_t m_threshold;
   bool m_skip_status_checking;
   bool m_force_orphan_node_removal;
   bool m_force_dag_node_removal;

   bool remove_path(
     boost::filesystem::path const&,
     ContextPtr log_ctx
   );

 public:
   Purger(bool have_lb_proxy);
   Purger();
   bool operator()(glite::wmsutils::jobid::JobId const&);

   Purger& log_using(boost::function<int(edg_wll_Context)>);
   Purger& threshold(time_t);
   Purger& skip_status_checking(bool = true);
   Purger& force_orphan_node_removal(bool = true);
   Purger& force_dag_node_removal(bool = true);
 };

 
} // namespace purger
} // namespace wms
} // namespace glite

// Local Variables:
// mode: c++
// End:
// 

#endif /* GLITE_WMS_PURGER_PURGER_H */
