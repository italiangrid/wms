// File: purger.h
// Author: Salvatore Monforte <salvatore.monforte@ct.infn.it>

// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

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

namespace jobid {
class JobId;
} // namespace jobid

namespace wms {
namespace purger {

 typedef boost::shared_ptr<boost::remove_pointer<edg_wll_Context>::type> ContextPtr;
 
 class Purger : public boost::noncopyable
 {
   bool m_have_lb_proxy;
   time_t m_threshold;
   bool m_skip_status_checking;
   bool m_force_orphan_node_removal;
   bool m_force_dag_node_removal;
   boost::function<int(edg_wll_Context)> m_logging_fn;

   bool remove_path(
     boost::filesystem::path const&,
     ContextPtr log_ctx
   );

 public:
   Purger(bool have_lb_proxy);
   bool operator()(glite::jobid::JobId const&);

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
