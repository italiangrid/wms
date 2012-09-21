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

// File: ism-cemon-purchaser.h
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2004 EU DataGrid.

// $Id: ism-cemon-asynch-purchaser.h,v 1.3.4.1.4.2 2010/04/08 13:54:43 mcecchi Exp $

#ifndef GLITE_WMS_ISM_PURCHASER_ISM_CEMON_ASYNCH_PURCHASER_H
#define GLITE_WMS_ISM_PURCHASER_ISM_CEMON_ASYNCH_PURCHASER_H

#include <string>
#include <vector>
#include "glite/wms/ism/purchaser/common.h"
#include "glite/wms/ism/purchaser/ism-purchaser.h"

class CEConsumer;

namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

class ism_cemon_asynch_purchaser : public ism_purchaser
{
public:
                
  ism_cemon_asynch_purchaser(
    std::string const& topic,
    int listening_port = 5120,
    exec_mode_t mode = loop,
    size_t interval = 30,
    exit_predicate_type exit_predicate = exit_predicate_type(),
    skip_predicate_type skip_predicate = skip_predicate_type()
  );

  void operator()();

private:
  int parse_classad_event_messages(boost::shared_ptr<CEConsumer>, gluece_info_container_type&);

private:                
  std::string m_topic;
  int m_listening_port;
  std::vector<std::string> m_multi_attributes;
};

namespace cemon_asynch {
// the types of the class factories
typedef ism_cemon_asynch_purchaser* create_t(
    std::string const& topic,
    int listening_port = 5120,
    exec_mode_t mode = loop,
    size_t interval = 30,
    exit_predicate_type exit_predicate = exit_predicate_type(),
    skip_predicate_type skip_predicate = skip_predicate_type()
);
typedef void destroy_t(ism_cemon_asynch_purchaser*);
}

} // namespace purchaser
} // namespace ism
} // namespace wms
} // namespace glite

#endif
