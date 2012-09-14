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

// File: ism-rgma-purchaser.cpp
// Author: Enzo Martelli <enzo.martelli@mi.infn.it>
// Copyright (c) 2002 EU DataGrid.

#include <boost/mem_fn.hpp>
#include <boost/progress.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>

#include "glite/wms/ism/ism.h"
#include "rgma-utils.h"
#include "glite/wms/ism/purchaser/ism-rgma-purchaser.h"

#include "glite/wms/common/ldif2classad/exceptions.h"
#include "glite/wms/common/utilities/ii_attr_utils.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wmsutils/classads/classad_utils.h"

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

//RGMA headers
#include "rgma/ConsumerFactory.h"
#include "impl/ConsumerFactoryImpl.h"
#include "rgma/Consumer.h"
#include "rgma/QueryProperties.h"
#include "rgma/ResultSet.h"
#include "rgma/TimeInterval.h"
#include "rgma/Units.h"
#include "rgma/RemoteException.h"
#include "rgma/UnknownResourceException.h"
#include "rgma/RGMAException.h"
#include "rgma/ResourceEndpointList.h"
#include "rgma/ResourceEndpoint.h"
#include "rgma/ResultSetMetaData.h"
#include "rgma/Types.h"
#include "rgma/Tuple.h"

#include <string>
#include <vector>

#define edglog(level) logger::threadsafe::edglog << logger::setlevel(logger::level)
#define edglog_fn(name) logger::StatePusher    pusher(logger::threadsafe::edglog, #name);

//#define MAXDELAY 100000000;                                                                                                     
using glite::rgma::ConsumerFactory;
using rgma::impl::ConsumerFactoryImpl;
using glite::rgma::Consumer;
using glite::rgma::QueryProperties;
using glite::rgma::ResultSet;
using glite::rgma::TimeInterval;
using glite::rgma::Units;
using glite::rgma::RemoteException;
using glite::rgma::UnknownResourceException;
using glite::rgma::RGMAException;
using glite::rgma::ResourceEndpointList;
using glite::rgma::ResourceEndpoint;
using glite::rgma::ResultSetMetaData;
using glite::rgma::Types;
using glite::rgma::Tuple;

using namespace classad;
using namespace std;


namespace glite {

namespace utils = wmsutils::classads;

namespace wms {

namespace ldif2classad	= common::ldif2classad;
namespace logger        = common::logger;
namespace utilities     = common::utilities;

namespace ism {
namespace purchaser {

namespace{

boost::condition f_rgma_purchasing_cycle_run_condition;
boost::mutex     f_rgma_purchasing_cycle_run_mutex;

bool checkMainValue( ClassAd* ad ) {
   if ( //!ad->Lookup("GlueCEAccessControlBaseRule")               ||
        !ad->Lookup("GlueSubClusterUniqueID")
        //!ad->Lookup("GlueSubClusterSoftwareRunTimeEnvironment")  ||
        //!ad->Lookup("CloseStorageElements")
      )
      return false;
   else return true;
}

} //namespace

bool ism_rgma_purchaser_entry_update::operator()(int a,boost::shared_ptr<classad::ClassAd>& ad)
{
   boost::mutex::scoped_lock l(f_rgma_purchasing_cycle_run_mutex);
   f_rgma_purchasing_cycle_run_condition.notify_one();
   return false;
}

  
ism_rgma_purchaser::ism_rgma_purchaser(
   int rgma_query_timeout,
   exec_mode_t mode,
   int rgma_consumer_ttl,
   int rgma_cons_life_cycles,
   size_t interval,
   exit_predicate_type exit_predicate,
   skip_predicate_type skip_predicate
) : ism_purchaser(mode, interval, exit_predicate, skip_predicate),
    m_rgma_query_timeout(rgma_query_timeout),
    m_rgma_consumer_ttl(rgma_consumer_ttl),
    m_rgma_cons_life_cycles(rgma_cons_life_cycles)
{
/*
   m_GlueCE = query("GlueCE");
   m_GlueCEAccessControlBaseRule = query("GlueCEAccessControlBaseRule");
   m_GlueSubCluster = query("GlueSubCluster");
   m_GlueSubClusterSoftwareRunTimeEnvironment = 
                 query("GlueSubClusterSoftwareRunTimeEnvironment");
   m_GlueCESEBind = query("GlueCESEBind");
   m_GlueCEVOView = query("GlueCEVOView");

   m_GlueSE = query("GlueSE");
   m_GlueSA = query("GlueSA");
   m_GlueSEAccessProtocol =
      query("GlueSEAccessProtocol");
   m_GlueSEAccessProtocolCapability =
      query("GlueSEAccessProtocolCapability");
   m_GlueSEAccessProtocolSupportedSecurity = 
      query("GlueSEAccessProtocolSupportedSecurity");
   m_GlueSAAccessControlBaseRule =
      query("GlueSAAccessControlBaseRule");
   m_GlueSEControlProtocol =
      query("GlueSEControlProtocol");
   m_GlueSEControlProtocolCapability =
      query("GlueSEControlProtocolCapability");
*/

}

void ism_rgma_purchaser::operator()()
{
   do {

      gluece_info_container_type gluece_info_container;
      vector<gluece_info_iterator> gluece_info_container_updated_entries;

      gluese_info_container_type gluese_info_container;
      vector<gluese_info_iterator> gluese_info_container_updated_entries;

      prefetchGlueCEinfo(  m_rgma_query_timeout,
                           m_rgma_consumer_ttl,
                           m_rgma_cons_life_cycles,
                           &gluece_info_container);

      prefetchGlueSEinfo(  m_rgma_query_timeout,
                           m_rgma_consumer_ttl,
                           &gluese_info_container);

      boost::thread_group ce_group;
      boost::thread_group se_group;


      if ( ! gluece_info_container.empty() ) {

         ce_group.create_thread( boost::bind( &collect_acbr_info, 
                                              m_rgma_query_timeout,
                                              m_rgma_consumer_ttl,
                                              &gluece_info_container));

         ce_group.create_thread( boost::bind( &collect_sc_info,
                                              m_rgma_query_timeout,
                                              m_rgma_consumer_ttl,
                                              &gluece_info_container));

         ce_group.create_thread( boost::bind( &collect_srte_info,
                                              m_rgma_query_timeout,
                                              m_rgma_consumer_ttl,
                                              &gluece_info_container));

         ce_group.create_thread( boost::bind( &collect_bind_info,
                                              m_rgma_query_timeout,
                                              m_rgma_consumer_ttl,
                                              &gluece_info_container));
      }
      else Debug("No CEs found\n"<<"No attempt to collect information related to th CEs");

      if ( ! gluese_info_container.empty() ) {

         se_group.create_thread( boost::bind( &collect_se_sa_info,
                                              m_rgma_query_timeout,
                                              m_rgma_consumer_ttl,
                                              &gluese_info_container));

         se_group.create_thread( boost::bind( &collect_se_ap_info,
                                              m_rgma_query_timeout,
                                              m_rgma_consumer_ttl,
                                              &gluese_info_container));

         se_group.create_thread( boost::bind( &collect_se_cp_info,
                                              m_rgma_query_timeout,
                                              m_rgma_consumer_ttl,
                                              &gluese_info_container));


         se_group.join_all();
      }
      else Debug("No SEs found\n"<<"No attempt to collect information related to th SEs");

      if ( ! gluece_info_container.empty() ) {

         ce_group.join_all();
         collect_voview_info( m_rgma_query_timeout,
                              m_rgma_consumer_ttl,
                              &gluece_info_container );
      }

      try{
   
         for (gluece_info_iterator it = gluece_info_container.begin();
              it != gluece_info_container.end(); ++it) {
   
            if (m_skip_predicate.empty() || !m_skip_predicate(it->first)) {
   
               bool purchasing_ok = checkMainValue((it->second).get())     && 
                                    expand_glueceid_info(it->second)       &&
                                    insert_gangmatch_storage_ad(it->second);
   
               if (purchasing_ok) {
                  it->second->InsertAttr("PurchasedBy","ism_rgma_purchaser");
                  gluece_info_container_updated_entries.push_back(it);
/*
                  string GlueCEUniqueID="NO";
                  string GlueCEAccessControlBaseRule = "NO";
                  string GlueHostApplicationSoftwareRunTimeEnvironment = "NO";
                  string CloseStorageElements = "NO";
                  if ( it->second->EvaluateAttrString("GlueCEUniqueID", GlueCEUniqueID) ) {
                     if( it->second->Lookup("GlueCEAccessControlBaseRule") ) GlueCEAccessControlBaseRule="";
                     if( it->second->Lookup("GlueHostApplicationSoftwareRunTimeEnvironment") ) 
                                                           GlueHostApplicationSoftwareRunTimeEnvironment="";
                     if( it->second->Lookup("CloseStorageElements") ) CloseStorageElements="";
                  }
                  Debug("Purchased \""<<GlueCEUniqueID<<"\" CE with all SubCluster values"<<std::endl<<
                     "           with "<<GlueCEAccessControlBaseRule<<" GlueCEAccessControlBaseRule attribute"<<std::endl<<
                     "           with "<<GlueHostApplicationSoftwareRunTimeEnvironment
                                   <<" GlueHostApplicationSoftwareRunTimeEnvironment attribute"<< std::endl<<
                     "           with "<<CloseStorageElements<<" CloseStorageElements attribute");
*/
               }
            }
         } // for

         for (gluese_info_iterator it = gluese_info_container.begin();
              it != gluese_info_container.end(); ++it) {

            if (m_skip_predicate.empty() || !m_skip_predicate(it->first)) {
               it->second->InsertAttr("PurchasedBy","ism_rgma_purchaser");
               gluese_info_container_updated_entries.push_back(it);
            }
         }
   
         {
            ism_mutex_type::scoped_lock l(get_ism_mutex(glite::wms::ism::ce));	
            while(!gluece_info_container_updated_entries.empty()) {
   	  
               ism_type::value_type ism_entry = make_ism_entry(
                  gluece_info_container_updated_entries.back()->first, 
                  ::time(0), 
                  gluece_info_container_updated_entries.back()->second, 
                  ism_rgma_purchaser_entry_update() );
   
   	       get_ism(glite::wms::ism::ce).insert(ism_entry);
   
                  gluece_info_container_updated_entries.pop_back();            
            } // while
         } // unlock the mutex

         {
            ism_mutex_type::scoped_lock l(get_ism_mutex(glite::wms::ism::se));
            while(!gluese_info_container_updated_entries.empty()) {

               ism_type::value_type ism_entry = make_ism_entry(
                  gluese_info_container_updated_entries.back()->first,
                  ::time(0),
                  gluese_info_container_updated_entries.back()->second,
                  ism_rgma_purchaser_entry_update() );

               get_ism(glite::wms::ism::se).insert(ism_entry);

                  gluese_info_container_updated_entries.pop_back();
            } // while
         } // unlock the mutex

         if (m_mode) {
            boost::xtime xt;
            boost::xtime_get(&xt, boost::TIME_UTC);
            xt.sec += m_interval;
            boost::mutex::scoped_lock l(f_rgma_purchasing_cycle_run_mutex);
            f_rgma_purchasing_cycle_run_condition.timed_wait(l, xt);
         }
   
      }
      catch (...) { // TODO: Check which exception may arrive here... and remove catch all
         Warning("FAILED TO PURCHASE INFO FROM RGMA.");
      }
   } 
   while (m_mode && (m_exit_predicate.empty() || !m_exit_predicate()));


}


// the class factories
extern "C" ism_rgma_purchaser* create_rgma_purchaser(
                   int rgma_query_timeout,
                   exec_mode_t mode,
                   int rgma_consumer_ttl,
                   int rgma_cons_life_cycles,
                   size_t interval,
                   exit_predicate_type exit_predicate,
                   skip_predicate_type skip_predicate
                   ) 
{
   return new ism_rgma_purchaser(rgma_query_timeout, mode, rgma_consumer_ttl,
                                 rgma_cons_life_cycles, interval, 
                                 exit_predicate, skip_predicate
                                );
}

extern "C" void destroy_rgma_purchaser(ism_rgma_purchaser* p) {
    delete p;
}

// the entry update function factory
extern "C" boost::function<bool(int&, ad_ptr)> create_rgma_entry_update_fn() 
{
  return ism_rgma_purchaser_entry_update();
}


} // namespace purchaser
} // namespace ism
} // namespace wms
} // namespace glite
