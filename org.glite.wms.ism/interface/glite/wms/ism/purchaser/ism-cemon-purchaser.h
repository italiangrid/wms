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

// $Id$

#ifndef GLITE_WMS_ISM_PURCHASER_ISM_CEMON_PURCHASER_H
#define GLITE_WMS_ISM_PURCHASER_ISM_CEMON_PURCHASER_H

#include <string>
#include <vector>
#include "glite/wms/ism/purchaser/common.h"
#include "glite/wms/ism/purchaser/ism-purchaser.h"

namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

class ism_cemon_purchaser : public ism_purchaser
{
public:
  ism_cemon_purchaser(
    std::string const& certfile,
    std::string const& certpath,
    std::vector<std::string> const& service,
    std::string const& topic,
    int rate = 30,
    exec_mode_t mode = loop,
    size_t interval = 30,
    exit_predicate_type exit_predicate = exit_predicate_type(),
    skip_predicate_type skip_predicate = skip_predicate_type()
  );

  void operator()();
private:
  std::string m_certfile;
  std::string m_certpath;              
  std::string m_topic;
  int m_rate;
  std::vector<std::string> m_multi_attributes;
  std::vector<std::string> m_services;
};

class ism_cemon_purchaser_entry_update
{
public:
  ism_cemon_purchaser_entry_update() {}
  bool operator()(int a,boost::shared_ptr<classad::ClassAd>& ad);
};

namespace cemon {
// the types of the class factories
typedef ism_cemon_purchaser* create_t(std::string const& certfile,
    std::string const& certpath,
    std::vector<std::string> const& service,
    std::string const& topic,
    int rate = 30,
    exec_mode_t mode = loop,
    size_t interval = 30,
    exit_predicate_type exit_predicate = exit_predicate_type(),
    skip_predicate_type skip_predicate = skip_predicate_type()
);
typedef void destroy_t(ism_cemon_purchaser*);

// type of the entry update function factory
typedef boost::function<bool(int&, ad_ptr)> create_entry_update_fn_t();
}

} // namespace purchaser
} // namespace ism
} // namespace wms
} // namespace glite

#endif
