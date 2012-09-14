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


#ifndef GLITE_WMS_ISM_PURCHASER_ISM_RGMA_PURCHASER_H
#define GLITE_WMS_ISM_PURCHASER_ISM_RGMA_PURCHASER_H

#include <string>
#include <boost/mem_fn.hpp>
#include "glite/wms/common/utilities/edgstrstream.h"
#include "glite/wms/ism/purchaser/common.h"
#include "glite/wms/ism/purchaser/ism-purchaser.h"


#include <classad_distribution.h>

//#include "rgma/Tuple.h"
//#include "rgma/Consumer.h"
//#include "rgma/ResultSet.h"

//#include "rgma-utils.h"



namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

//typedef std::vector<std::string> RGMAMultiValue;

class ism_rgma_purchaser : public ism_purchaser
{
public:
                
   ism_rgma_purchaser(
     int rgma_query_timeout = 60,
     exec_mode_t mode = loop,
     int rgma_consumer_ttl = 300,
     int rgma_cons_life_cycles = 30,
     size_t interval = 120,
     exit_predicate_type exit_predicate = exit_predicate_type(),
     skip_predicate_type skip_predicate = skip_predicate_type()
   );
   
   void operator()();
/* 
   friend void prefetchGlueCEinfo(ism_rgma_purchaser * purchaser,
                      gluece_info_container_type * gluece_info_container);
   friend void collect_acbr_info( ism_rgma_purchaser * purchaser,
                      gluece_info_container_type * gluece_info_container);
   friend void collect_sc_info( ism_rgma_purchaser * purchaser,
                      gluece_info_container_type * gluece_info_container);
   friend void collect_srte_info( ism_rgma_purchaser * purchaser,
                      gluece_info_container_type * gluece_info_container);
   friend void collect_bind_info( ism_rgma_purchaser * purchaser,
                      gluece_info_container_type * gluece_info_container);
   friend void collect_voview_info( ism_rgma_purchaser * purchaser,
                      gluece_info_container_type * gluece_info_container);

   friend void prefetchGlueSEinfo(ism_rgma_purchaser * purchaser,
                      gluese_info_container_type * gluese_info_container);
   friend void collect_se_sa_info(ism_rgma_purchaser * purchaser,
                      gluese_info_container_type * gluese_info_container);
   friend void collect_se_ap_info(ism_rgma_purchaser * purchaser,
                      gluese_info_container_type * gluese_info_container);
   friend void collect_se_cp_info(ism_rgma_purchaser * purchaser,
                      gluese_info_container_type * gluese_info_container);

*/

private:                
   int m_rgma_query_timeout;
   int m_rgma_consumer_ttl;
   int m_rgma_cons_life_cycles;
/*
   query m_GlueCE;
   query m_GlueCEAccessControlBaseRule;
   query m_GlueSubCluster;
   query m_GlueSubClusterSoftwareRunTimeEnvironment;
   query m_GlueCESEBind;
   query m_GlueCEVOView;

   query m_GlueSE;
   query m_GlueSA;
   query m_GlueSEAccessProtocol;
   query m_GlueSEAccessProtocolCapability;
   query m_GlueSEAccessProtocolSupportedSecurity;
   query m_GlueSAAccessControlBaseRule;
   query m_GlueSEControlProtocol;
   query m_GlueSEControlProtocolCapability;
*/

};


class ism_rgma_purchaser_entry_update
{
public:

   ism_rgma_purchaser_entry_update() {}
   bool operator()(int a,boost::shared_ptr<classad::ClassAd>& ad);

};

namespace rgma {
// the types of the class factories
typedef ism_rgma_purchaser* create_t(
    int rgma_query_timeout = 30,
    exec_mode_t mode = loop,
    int rgma_consumer_ttl = 300,
    int rgma_cons_life_cycles = 30,
    size_t interval = 30,
    exit_predicate_type exit_predicate = exit_predicate_type(),
    skip_predicate_type skip_predicate = skip_predicate_type()
  );

typedef void destroy_t(ism_rgma_purchaser*);

// type of the entry update function factory
typedef boost::function<bool(int&, boost::shared_ptr<classad::ClassAd>)> create_entry_update_fn_t();
}

} // namespace purchaser
} // namespace ism
} // namespace wms
} // namespace glite

#endif
