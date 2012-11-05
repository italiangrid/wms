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

// Author: Salvatore Monforte
// Author: Francesco Giacomini
// Author: Marco Cecchi
#ifndef GLITE_WMS_ISM_PURCHASER_COMMON
#define GLITE_WMS_ISM_PURCHASER_COMMON

#include <vector>
#include <string>
#include <map>
#include <ldap.h>
#include <lber.h>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <boost/regex.hpp>
#include "glite/wms/ism/ism.h"

namespace classad {
class ClassAd;
}

namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

class LDAPException: public std::exception
{
  std::string m_error;
public:
  LDAPException(std::string const& error)
    : m_error(error) { }
  ~LDAPException() throw() { }
  virtual char const* what() const throw()
  {
    return m_error.c_str();
  }
};

typedef boost::function<bool(void)> exit_predicate_type;
typedef boost::function<bool(std::string const&)> skip_predicate_type;
typedef boost::shared_ptr<classad::ClassAd> ad_ptr;
typedef boost::function<bool(int&, ad_ptr)> update_function_type;

typedef boost::shared_ptr<classad::ClassAd>        gluece_info_type;
typedef std::map<std::string, gluece_info_type>    gluece_info_container_type;
typedef gluece_info_container_type::const_iterator gluece_info_const_iterator;
typedef gluece_info_container_type::iterator       gluece_info_iterator;
typedef boost::shared_ptr<classad::ClassAd>        gluese_info_type;
typedef std::map<std::string, gluese_info_type>    gluese_info_container_type;
typedef gluese_info_container_type::const_iterator gluese_info_const_iterator;
typedef gluese_info_container_type::iterator       gluese_info_iterator;

inline bool iequals(std::string const& a, std::string const& b);
inline bool istarts_with(std::string const& a, std::string const& b);
inline std::string strip_prefix(std::string const& prefix, std::string const& s);
void insert_values(
  std::string const& name,
  boost::shared_array<struct berval*> values,
  std::list<std::string> const& prefix,
  classad::ClassAd& ad
);
classad::ClassAd*
create_classad_from_ldap_entry(
  LDAP* ld,
  LDAPMessage* lde,
  std::list<std::string> prefix,
  bool is_schema_version_20 = false
);
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
bool expand_glueceid_info(boost::shared_ptr<classad::ClassAd>& gluece_info);
void insert_gangmatch_storage_ad(ad_ptr glue_info);
bool expand_glueid_info(ad_ptr glue_info);
bool split_information_service_url(
  classad::ClassAd const&,
  boost::tuple<std::string, int, std::string>&
);

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
