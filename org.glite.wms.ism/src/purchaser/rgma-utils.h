/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// File: ism-rgma-purchaser.h
// Author: Enzo Martelli <enzo.martelli@mi.infn.it>
// Copyright (c) 2004 EU DataGrid.


#ifndef GLITE_WMS_ISM_PURCHASER_RGMA_UTILS_H
#define GLITE_WMS_ISM_PURCHASER_RGMA_UTILS_H

#include <string>
#include <boost/mem_fn.hpp>
#include "glite/wms/common/utilities/edgstrstream.h"
#include "glite/wms/ism/purchaser/common.h"
#include "glite/wms/ism/purchaser/ism-purchaser.h"


#include <classad_distribution.h>

#include "rgma/Tuple.h"
#include "rgma/Consumer.h"
#include "rgma/ResultSet.h"



namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

class query  { 

private:  
   glite::rgma::Consumer* m_consumer;  
   std::string m_table;
   bool m_query_status; 
public: 
   bool refresh_query(int rgma_query_timeout); 
   bool refresh_consumer(int rgma_consumer_ttl); 
   bool pop_tuples ( glite::rgma::ResultSet & out, int maxTupleNumber); 
//   glite::rgma::Consumer* get_consumer() { return m_consumer; }
//   bool get_query_status() { return m_query_status; } 
   query(const std::string & table) { m_table = table;
                                      m_consumer = NULL;
                                      m_query_status = false;}
   query& operator=( const query& q) {
      m_consumer = q.m_consumer;
      m_table = q.m_table;
      m_query_status = q.m_query_status;
      return *this;
   }
   query(const query& q) {
      m_consumer = q.m_consumer;
      m_table = q.m_table;
      m_query_status = q.m_query_status;
   }      
   query(){
     m_query_status = false;
     m_consumer = NULL;
   }
   ~query(); 
};


void 
prefetchGlueCEinfo( int rgma_query_timeout,
                    int rgma_consumer_ttl,
                    int rgma_cons_life_cycles,
                    gluece_info_container_type * gluece_info_container
);

void 
collect_acbr_info( int rgma_query_timeout,
                   int rgma_consumer_ttl, 
                   gluece_info_container_type * gluece_info_container);

void 
collect_sc_info(int rgma_query_timeout,
                int rgma_consumer_ttl, 
                gluece_info_container_type * gluece_info_container);
void 
collect_srte_info(int rgma_query_timeout,
                  int rgma_consumer_ttl, 
                  gluece_info_container_type * gluece_info_container);

void 
collect_bind_info(int rgma_query_timeout,
                  int rgma_consumer_ttl,
                  gluece_info_container_type * gluece_info_container);

void 
collect_voview_info(int rgma_query_timeout,
                    int rgma_consumer_ttl,
                    gluece_info_container_type * gluece_info_container);

void 
prefetchGlueSEinfo(int rgma_query_timeout,
                   int rgma_consumer_ttl,
                   gluese_info_container_type * gluese_info_container);

void 
collect_se_sa_info(int rgma_query_timeout,
                   int rgma_consumer_ttl,
                   gluese_info_container_type * gluese_info_container);

void 
collect_se_ap_info(int rgma_query_timeout,
                   int rgma_consumer_ttl,
                   gluese_info_container_type * gluese_info_container);

void collect_se_cp_info(int rgma_query_timeout,
                        int rgma_consumer_ttl,
                        gluese_info_container_type * gluese_info_container);



} // namespace purchaser
} // namespace ism
} // namespace wms
} // namespace glite

#endif
