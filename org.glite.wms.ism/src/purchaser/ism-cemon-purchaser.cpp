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

// File: ism-ii-purchaser.cpp
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id$

#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <glite/wms/ism/ism.h>
#include "glite/wms/ism/purchaser/ism-cemon-purchaser.h"
#include <glite/ce/monitor-client-api-c/CEEvent.h>
#include <glite/ce/monitor-client-api-c/Dialect.h>
#include <glite/ce/monitor-client-api-c/Topic.h>
#include <glite/ce/monitor-client-api-c/GenericException.h>
#include <glite/ce/monitor-client-api-c/TopicNotSupportedException.h>
#include <glite/ce/monitor-client-api-c/DialectNotSupportedException.h>
#include <glite/ce/monitor-client-api-c/ServiceNotFoundException.h>
#include <glite/ce/monitor-client-api-c/AuthenticationException.h>
#include <glite/wms/common/logger/logger_utils.h>
#include <glite/wmsutils/classads/classad_utils.h>

using namespace std;
namespace utils = glite::wmsutils::classads;

namespace glite {
namespace wms {
namespace ism {
namespace purchaser {
namespace {

vector<string> f_multi_attributes;

boost::condition f_purchasing_cycle_run_condition;
boost::mutex     f_purchasing_cycle_run_mutex;

void
parse_classad_event_messages(
 CEEvent* ceE, 
 gluece_info_container_type& gluece_info_container) 
{
  const char* msg;
  // According to the CEMON dialect for ISM there is a message for every
  // CE whose classad schema is to be retrieved.
  while((msg=ceE->getNextEventMessage())) {
    gluece_info_type ceAd;
    try {
      ceAd.reset(utils::parse_classad(string(msg)));
      string gluece_unique_id;
      string gluevoview_local_id;

      ceAd->EvaluateAttrString("GlueCEUniqueID", gluece_unique_id);
      ceAd->EvaluateAttrString("GlueVOViewLocalID", gluevoview_local_id);
      
      if (!gluece_unique_id.empty()) {
        
        if (!gluevoview_local_id.empty()) {
          gluece_unique_id.append("/"+gluevoview_local_id);
        }
        Debug("CEMonitor info for " << gluece_unique_id << "... Ok");
        gluece_info_container[gluece_unique_id] = ceAd;
      }
      else {
        Warning("Unable to evaluate GlueCEUniqueID");
      }
    }
    catch(utils::CannotParseClassAd& e) {
      Warning("Error parsing CEMON classad..."  << e.what());
    }
  }
}

} // anonymous namespace

bool 
ism_cemon_purchaser_entry_update::operator()(
  int a,boost::shared_ptr<classad::ClassAd>& ad)
{
  boost::mutex::scoped_lock l(f_purchasing_cycle_run_mutex);  

  f_purchasing_cycle_run_condition.notify_one();
  return false;
}

ism_cemon_purchaser::ism_cemon_purchaser(
  std::string const& certfile,
  std::string const& certpath,
  std::vector<std::string> const& services,
  std::string const& topic,
  int rate,
  exec_mode_t mode,
  size_t interval,
  exit_predicate_type exit_predicate,
  skip_predicate_type skip_predicate
)
  : ism_purchaser(mode, interval, exit_predicate, skip_predicate),
    m_certfile(certfile),
    m_certpath(certpath),
    m_topic(topic),
    m_rate(rate),
    m_services(services)
{
}

void ism_cemon_purchaser::operator()()
{
  do {
    
    gluece_info_container_type gluece_info_container;
    Topic ce_topic(m_topic);
 
    vector<string>::const_iterator service_it = m_services.begin();
    vector<string>::const_iterator const service_end = m_services.end();

    for(; service_it != service_end; ++service_it) {
      
      try {
        boost::scoped_ptr<CEEvent> ceE(new CEEvent(m_certfile,m_certpath));

        ceE->setServiceURL(*service_it);
        ceE->setRequestParam(&ce_topic);

        ceE->getEvent(); 
        parse_classad_event_messages(ceE.get(), gluece_info_container); 
      }
      catch(CEException& e) {
	Error("Unable to get event from CEMonitor:" << e.getMethod() << ", " <<
          e.getErrorCode() << ", " <<
	  e.getDescription() << ", " << e.getFaultCause() << endl
        );
      }
      catch (AbsException& e) {
	Error("Unable to get event from CEMonitor: " << e.what() << endl);
      }
    } // for_each service
    {
    ism_mutex_type::scoped_lock l(get_ism_mutex(ism::ce));
    gluece_info_iterator it = gluece_info_container.begin();
    gluece_info_iterator const e = gluece_info_container.end();
    time_t const current_time(std::time(0));
    for ( ;
         it != e; ++it) {
	
      if ((m_skip_predicate.empty() || !m_skip_predicate(it->first))) {

        insert_gangmatch_storage_ad(it->second);
	if (expand_glueceid_info(it->second)) {
       
          int TTLCEinfo = 0;
          if (!it->second->EvaluateAttrNumber("TTLCEinfo", TTLCEinfo)) 
            TTLCEinfo = 300;
          it->second->InsertAttr("PurchasedBy","ism_cemon_purchaser");
          ism_type::iterator ism_it = get_ism(ism::ce).find(it->first);
          if (ism_it != get_ism(ism::ce).end()) {
            ism_type::mapped_type& data = ism_it->second;
            boost::tuples::get<0>(data) = current_time;
            boost::tuples::get<2>(data) = it->second;
          } else {
            get_ism(ism::ce).insert(
              make_ism_entry(
                it->first,
                current_time,
                it->second,
                ism_cemon_purchaser_entry_update()
              )
            );
          }
        }
      }
    }
    } // end of scope needed to unlock the ism mutex

    if (m_mode) {
      boost::xtime xt;
      boost::xtime_get(&xt, boost::TIME_UTC);
      xt.sec += m_interval;
      boost::mutex::scoped_lock l(f_purchasing_cycle_run_mutex); 
      f_purchasing_cycle_run_condition.timed_wait(l, xt); 
    }

  } while (m_mode && (m_exit_predicate.empty() || !m_exit_predicate()));
}

// the class factories

extern "C" ism_cemon_purchaser* create_cemon_purchaser(
    std::string const& cert_file,
    std::string const& cert_path,
    std::vector<std::string> const& service,
    std::string const& topic,
    int rate,
    exec_mode_t mode,
    size_t interval,
    exit_predicate_type exit_predicate,
    skip_predicate_type skip_predicate
) {
    return new ism_cemon_purchaser(
      cert_file, cert_path,service, topic, rate, mode, interval, 
      exit_predicate, skip_predicate
    );
}

extern "C" void destroy_cemon_purchaser(ism_cemon_purchaser* p) {
    delete p;
}

// the entry update function factory
extern "C" boost::function<bool(int&, ad_ptr)> create_cemon_entry_update_fn()
{
  return ism_cemon_purchaser_entry_update();
}

} // namespace purchaser
} // namespace ism
} // namespace wms
} // namespace glite
