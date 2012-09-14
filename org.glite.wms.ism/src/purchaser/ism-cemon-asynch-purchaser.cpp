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

// File: ism-cemon-asynch-purchaser.cpp
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id$

#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>
#include "glite/wms/ism/ism.h"
#include "glite/wms/ism/purchaser/ism-cemon-asynch-purchaser.h"
//#include "glite/ce/monitor-client-api-c/CEMonitorBinding.nsmap"
#include "glite/ce/monitor-client-api-c/CEConsumer.h"
#include "glite/ce/monitor-client-api-c/Topic.h"

#include "glite/wms/common/ldif2classad/LDIFObject.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wmsutils/classads/classad_utils.h"

using namespace std;
namespace utils = glite::wmsutils::classads;
namespace utilities = glite::wms::common::utilities;

namespace glite {
namespace wms {

namespace ldif2classad  = common::ldif2classad;

namespace ism {
namespace purchaser {

ism_cemon_asynch_purchaser::ism_cemon_asynch_purchaser(
  std::string const& topic,
  int listening_port,
  exec_mode_t mode,
  size_t interval,
  exit_predicate_type exit_predicate,
  skip_predicate_type skip_predicate
)
  : ism_purchaser(mode, interval, exit_predicate, skip_predicate),
  m_topic(topic),
  m_listening_port(listening_port)
{
}

int ism_cemon_asynch_purchaser::parse_classad_event_messages(boost::shared_ptr<CEConsumer>consumer, gluece_info_container_type& gluece_info_container)
{
  int result = 0;
  const char* msg;
  // According to the CEMON dialect for ISM there is a message for every
  // CE whose classad schema is to be retrieved.
  while(msg=consumer->getNextEventMessage()) {
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
        ++result;
      }
      else {
        Warning("Unable to evaluate GlueCEUniqueID");
      }
    }
    catch(utils::CannotParseClassAd& e) {
      Warning("Error parsing CEMON classad..."  << e.what());
    }
  }
  return result;
}

void ism_cemon_asynch_purchaser::operator()()
{
  do {
    
     boost::shared_ptr<CEConsumer> consumer(new CEConsumer(m_listening_port));
     if (consumer->bind()) {

       int const local_socket = consumer->getLocalSocket();

       Debug(
         "CEConsumer socket connection successful on port "
         << consumer->getLocalPort() << " socket #" << local_socket
       );

       do {

         fd_set read_set;
         FD_ZERO(&read_set);
         FD_SET(local_socket, &read_set);
         struct timeval tv;
         tv.tv_sec = 1;
         tv.tv_usec = 0;
         int select_error = select(local_socket + 1, &read_set, 0, 0, &tv);
         if (select_error == -1) { // error
           ::sleep(1);
           continue;
         } else if (select_error == 0) { // timeout expired
           continue;
         }

         if (consumer->accept()) {

          Debug("CEConsumer accepted connection from " << consumer->getClientIP() 
                << " ("<< consumer->getClientName()<<")"<<endl);

          if (consumer->serve()) {

            // We have to check the dialect type the CEMON is speaking...
	    // ...and depending on the result parse opportunely the message
	    // supplied with...
	    Topic* topic = consumer->getEventTopic();
            if (!topic->getNumOfDialects()) {
              Warning("Received Topic contains no dialect");
              continue;
            }
            string dialect(topic->getDialectAt(0)->getDialectName());

	    gluece_info_container_type gluece_info_container;
            if ( (((!strcasecmp(dialect.c_str(), "ISM_CLASSAD")) ||
                   !strcasecmp(dialect.c_str(), "ISM_CLASSAD_GLUE_1.2")) && 
                  parse_classad_event_messages(consumer, gluece_info_container)) ) {
                 
              ism_mutex_type::scoped_lock l(get_ism_mutex(ism::ce));
              time_t const current_time( std::time(0) );
	      for (gluece_info_iterator it = gluece_info_container.begin();
                it != gluece_info_container.end(); ++it) {

                if ((m_skip_predicate.empty() || !m_skip_predicate(it->first))) {                 
                
                  insert_gangmatch_storage_ad(it->second);

                  if (expand_glueceid_info(it->second)) {
		    int TTLCEinfo = 0;
                    if (!it->second->EvaluateAttrNumber("TTLCEinfo", TTLCEinfo)) 
                      TTLCEinfo = 300;
                    it->second->InsertAttr("PurchasedBy","ism_cemon_asynch_purchaser");
                    ism_type::iterator ism_it = get_ism(ism::ce).find(it->first);
                    if (ism_it != get_ism(ism::ce).end()) {
                      ism_type::mapped_type& data = ism_it->second;
                      boost::tuples::get<0>(data) = current_time;
                      boost::tuples::get<2>(data) = it->second;
                    } 
                    else {
                      get_ism(ism::ce).insert(
                        make_ism_entry(
                          it->first,
                          current_time,
                          it->second,
                          update_function_type()
                        )
                      );
                    }
                  }
                }
              }
            }
          } // serve
          else {
            Error("CEConsumer::serve failure:" << consumer->getErrorMessage() << " #" << consumer->getErrorCode());
          }

        } // accept
      } while (m_mode && (m_exit_predicate.empty() || !m_exit_predicate()));
    } // bind
    else if (m_mode == loop) {
      Error(
        "CEConsumer::bind failure:" << consumer->getErrorMessage()
        << " #" << consumer->getErrorCode()
      );
      ::sleep(m_interval); 
    }
  } while (m_mode && (m_exit_predicate.empty() || !m_exit_predicate()));
}    

// the class factories

extern "C" ism_cemon_asynch_purchaser* create_cemon_asynch_purchaser(
    std::string const& topic,
    int listening_port,
    exec_mode_t mode,
    size_t interval,
    exit_predicate_type exit_predicate,
    skip_predicate_type skip_predicate
) {
    return new ism_cemon_asynch_purchaser(topic, listening_port, mode, interval, exit_predicate, skip_predicate);
}

extern "C" void destroy_cemon_asynch_purchaser(ism_cemon_asynch_purchaser* p) {
    delete p;
}

} // namespace purchaser
} // namespace ism
} // namespace wms
} // namespace glite
