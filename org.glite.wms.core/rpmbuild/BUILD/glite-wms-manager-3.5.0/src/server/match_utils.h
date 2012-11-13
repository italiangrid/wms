// File: match_utils.h
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

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
  {
  }
  ~MatchError() throw()
  {
  }
  virtual char const* what() const throw()
  {
    return m_error.c_str();
  }
};

bool
fill_matches(
  classad::ClassAd const& match_response,
  matches_type& matches,
  bool include_brokerinfo = false
);

}}}} // glite::wms::manager::server

#endif
