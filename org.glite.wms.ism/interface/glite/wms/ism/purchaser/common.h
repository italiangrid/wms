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

// File: common.h
// Author: Salvatore Monforte
// Author: Francesco Giacomini
// Copyright (c) 2002 EU DataGrid.

// $Id$

#ifndef GLITE_WMS_ISM_PURCHASER_COMMON
#define GLITE_WMS_ISM_PURCHASER_COMMON

#include <vector>
#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <boost/regex.hpp>

namespace classad {
class ClassAd;
}

namespace glite {
namespace wms {

namespace ism {
namespace purchaser {

typedef boost::function<bool(void)> exit_predicate_type;
typedef boost::function<bool(std::string const&)> skip_predicate_type;
typedef boost::shared_ptr<classad::ClassAd> ad_ptr;
typedef boost::function<bool(int&, ad_ptr)> update_function_type;

typedef std::map<
  std::string,
  ad_ptr
> PurchaserInfoContainer;

typedef boost::shared_ptr<classad::ClassAd>        gluece_info_type;
typedef std::map<std::string, gluece_info_type>    gluece_info_container_type;
typedef gluece_info_container_type::const_iterator gluece_info_const_iterator;
typedef gluece_info_container_type::iterator       gluece_info_iterator;

typedef boost::shared_ptr<classad::ClassAd>        gluese_info_type;
typedef std::map<std::string, gluese_info_type>    gluese_info_container_type;
typedef gluese_info_container_type::const_iterator gluese_info_const_iterator;
typedef gluese_info_container_type::iterator       gluese_info_iterator;

void apply_skip_predicate(
  gluece_info_container_type& gluece_info_container,
  std::vector<gluece_info_iterator>& gluece_info_container_updated_entries,
  skip_predicate_type skip,
  std::string const& purchased_by);
void populate_ism(
  std::vector<gluece_info_iterator>& gluece_info_container_updated_entries,
  size_t the_ism_index,
  update_function_type const& uf);
void tokenize_ldap_dn(std::string const& s, std::vector<std::string> &v);
bool expand_information_service_info(gluece_info_type& gluece_info);
bool insert_gangmatch_storage_ad(gluece_info_type& gluece_info);
bool expand_glueceid_info(gluece_info_type& gluece_info);
bool split_information_service_url(
  classad::ClassAd const&,
  boost::tuple<std::string, int, std::string>&);

enum exec_mode_t {
  once,
  loop
};

class regex_matches_string
{
  std::string m_string;

public:
  regex_matches_string(std::string const& s)
    : m_string(s)
  {
  }
  bool operator()(std::string const& regex)
  {
    try {
      boost::regex r(regex);
      boost::smatch s;
      return boost::regex_match(m_string, s, r);
    } catch (boost::bad_expression&) {
      return false;
    }
  }
};

class is_in_black_list
{
  std::vector<std::string> m_black_list;

public:
  is_in_black_list(std::vector<std::string> const& black_list)
    : m_black_list(black_list)
  {
  }
  bool operator()(std::string const& entry_id)
  {
    std::vector<std::string>::const_iterator first(m_black_list.begin());
    std::vector<std::string>::const_iterator last(m_black_list.end());

    return find_if(first, last, regex_matches_string(entry_id)) != last;
  }
};

}}}}

#endif
