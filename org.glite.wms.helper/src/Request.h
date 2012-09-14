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

// File: Request.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>

// $Id$

#ifndef GLITE_WMS_HELPER_REQUEST_H
#define GLITE_WMS_HELPER_REQUEST_H

#ifndef GLITE_WMS_X_BOOST_UTILITY_HPP
#define GLITE_WMS_X_BOOST_UTILITY_HPP
#include <boost/utility.hpp>
#endif
#include <boost/shared_ptr.hpp>

namespace classad {
class ClassAd;
}

namespace glite {
namespace wms {
namespace helper {

class Request: boost::noncopyable
{
  class Impl;

  Impl* m_impl;

public:
  Request(
    classad::ClassAd const* ad,
    boost::shared_ptr<std::string> jw_template
  );
  ~Request();
  void resolve();
  bool is_resolved() const;
  classad::ClassAd const* original_ad() const;
  classad::ClassAd const* current_ad() const;
};

inline
classad::ClassAd const*
resolved_ad(Request const& request)
{
  return request.is_resolved() ? request.current_ad() : 0;
}

} // namespace helper
} // namespace wms
} // namespace glite

#endif // GLITE_WMS_HELPER_REQUEST_H

