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

// File: Helper.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id: Helper.h,v 1.1.1.1.28.4.2.2.4.2 2010/04/08 13:52:15 mcecchi Exp $

#ifndef GLITE_WMS_HELPER_HELPER_H
#define GLITE_WMS_HELPER_HELPER_H

#ifndef GLITE_WMS_X_STRING
#define GLITE_WMS_X_STRING
#include <string>
#endif
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

class HelperImpl;

class Helper: boost::noncopyable
{
  HelperImpl* m_impl;

public:
  Helper(std::string const& id);
  ~Helper();

  std::string id() const;

  std::string resolve(std::string const& input_file) const;
  classad::ClassAd* resolve(
    classad::ClassAd const* input_ad,
      boost::shared_ptr<std::string> jwt = boost::shared_ptr<std::string>()
  ) const;
};
 
}}} // glite::wms::helper

#endif // GLITE_WMS_HELPER_HELPER_H
