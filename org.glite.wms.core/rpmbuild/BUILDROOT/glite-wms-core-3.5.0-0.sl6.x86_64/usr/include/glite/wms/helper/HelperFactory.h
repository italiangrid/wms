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

// File: HelperFactory.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id$

#ifndef GLITE_WMS_HELPER_HELPERFACTORY_H
#define GLITE_WMS_HELPER_HELPERFACTORY_H

#ifndef GLITE_WMS_X_VECTOR
#define GLITE_WMS_X_VECTOR
#include <vector>
#endif
#ifndef GLITE_WMS_X_BOOST_UTILITY_HPP
#define GLITE_WMS_X_BOOST_UTILITY_HPP
#include <boost/utility.hpp>
#endif

#include <exception>

namespace glite {
namespace wms {
namespace helper {

class HelperImpl;

struct NoCreateHelperException : public std::exception
{
    const char* what() const throw () 
    { 
       return "Unknown Helper"; 
    }
};

class HelperFactory: boost::noncopyable
{
  class Impl;

  Impl* m_impl;

  static HelperFactory* s_instance;

  HelperFactory();

  typedef HelperImpl product_type;
  typedef product_type* (*product_creator_type)();

public:
  static HelperFactory* instance();
  ~HelperFactory();

public:

  bool register_helper(std::string const& id, product_creator_type creator);
  bool unregister_helper(std::string const& id);
  product_type* create_helper(std::string const& id);
  std::vector<std::string> list() const;
};

}}} // glite::wms::helper

#endif
