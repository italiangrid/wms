/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the
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

// File: match_utils.h
// Author: Francesco Giacomini

// $Id: match_utils.h,v 1.1.2.1.4.2 2010/04/07 14:02:46 mcecchi Exp $

#ifndef GLITE_WMS_MANAGER_SERVER_MATCH_UTILS_H
#define GLITE_WMS_MANAGER_SERVER_MATCH_UTILS_H

#include <string>
#include <vector>
#include <exception>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>

namespace classad {
class ClassAd;
}

namespace glite {
namespace wms {
namespace manager {
namespace server {

typedef boost::shared_ptr<classad::ClassAd> ClassAdPtr;
typedef boost::tuple<std::string, double, ClassAdPtr> match_type;
typedef std::vector<match_type> matches_type;

matches_type::const_iterator
select_best_ce(matches_type const& matches, bool use_fuzzy_rank);

class MatchError: public std::exception
{
  std::string m_error;
public:
  MatchError(std::string const& error)
    : m_error(error)
  { }
  ~MatchError() throw()
  { }
  virtual char const* what() const throw()
  {
    return m_error.c_str();
  }
};

bool
fill_matches(
  classad::ClassAd const& match_response,
  matches_type& matches,
  bool include_brokerinfo = false,
  bool include_cream_resources = false
);

}}}} // glite::wms::manager::server

#endif
