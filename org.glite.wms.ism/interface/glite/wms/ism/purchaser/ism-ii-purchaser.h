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

// File: ism-ii-purchaser.h
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2004 EU DataGrid.

// $Id$

#ifndef GLITE_WMS_ISM_PURCHASER_ISM_II_PURCHASER_H
#define GLITE_WMS_ISM_PURCHASER_ISM_II_PURCHASER_H

#include <string>
#include "glite/wms/ism/purchaser/common.h"
#include "glite/wms/ism/purchaser/ism-purchaser.h"
 
namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

class ism_ii_purchaser : public ism_purchaser
{
public:
  ism_ii_purchaser(
    std::string const& hostname,
    int port,
    std::string const& distinguished_name,
    int timeout = 30,
    std::string const& ldap_ce_filter_g13 = std::string(),
    std::string const& ldap_ce_filter_g20 = std::string(),
    std::string const& ldap_se_filter_g20 = std::string(),
    bool ldap_search_async = false,
    exec_mode_t mode = loop,
    size_t interval = 30,
    exit_predicate_type exit_predicate = exit_predicate_type(),
    skip_predicate_type skip_predicate = false_	
  );
  
  void operator()();

private:                
  std::string m_hostname;
  int m_port;
  std::string m_dn;
  int m_timeout;
  std::string m_ldap_ce_filter_g13;
  std::string m_ldap_ce_filter_g20;
  std::string m_ldap_se_filter_g20;
  bool m_ldap_search_async;
};

class ism_ii_purchaser_entry_update
{
public:
  ism_ii_purchaser_entry_update() {}
  bool operator()(int a, boost::shared_ptr<classad::ClassAd>& ad);
};

namespace ii {
// the types of the class factories
typedef ism_ii_purchaser* create_t(std::string const& hostname,
    int port,
    std::string const& distinguished_name,
    int timeout = 30,
    std::string const& ldap_ce_filter_g13 = std::string(),
    std::string const& ldap_ce_filter_g20 = std::string(),
    std::string const& ldap_se_filter_g20 = std::string(),
    bool ldap_search_async = false,
    exec_mode_t mode = loop,
    size_t interval = 30,
    exit_predicate_type exit_predicate = exit_predicate_type(),
    skip_predicate_type skip_predicate = false_
  );

typedef void destroy_t(ism_ii_purchaser*);

// type of the entry update function factory
typedef boost::function<bool(int&, boost::shared_ptr<classad::ClassAd>)> create_entry_update_fn_t();
}

}}}}

#endif
