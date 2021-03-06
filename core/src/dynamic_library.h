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

// File: dynamic_library.h
// Author: Francesco Giacomini, INFN

// $Id: dynamic_library.h,v 1.1.2.1.4.2 2010/04/07 14:02:46 mcecchi Exp $

#ifndef GLITE_WMS_MANAGER_SERVER_DYNAMIC_LIBRARY_H
#define GLITE_WMS_MANAGER_SERVER_DYNAMIC_LIBRARY_H

#include <exception>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_pointer.hpp>

namespace glite {
namespace wms {
namespace manager {
namespace server {

class CannotLoadDynamicLibrary: public std::exception
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  CannotLoadDynamicLibrary(
    std::string const& filename,
    std::string const& error
  );
  virtual ~CannotLoadDynamicLibrary() throw()
  {
  }
  virtual char const* what() const throw()
  {
    return "CannotLoadDynamicLibrary";
  }
  std::string filename() const;
  std::string error() const;
};

class CannotLookupSymbol: public std::exception
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  CannotLookupSymbol(std::string const& symbol, std::string const& error);
  virtual ~CannotLookupSymbol() throw()
  {
  }
  virtual char const* what() const throw()
  {
    return "CannotLookupSymbol";
  }
  std::string symbol() const;
  std::string error() const;
};

class DynamicLibrary
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  enum {
    local_visibility = 1L << 1,
    global_visibility = 1L << 2,
    immediate_binding = 1L << 3,
    lazy_binding = 1L << 4
  };
    
  // load a dynamic library with the given filename
  // throw CannotLoadDynamicLibrary if the operation fails
  DynamicLibrary(
    std::string const& filename,
    int flags = lazy_binding | local_visibility
  );

  // lookup a symbol in the dynamic library
  // T must be a pointer type
  // throws CannotLookupSymbol in case of failure
  template<typename T>
  void lookup(std::string const& symbol, T& t) const
  {
    BOOST_STATIC_ASSERT(boost::is_pointer<T>::value);
    t = T(do_lookup(symbol));
  }

private:
  void* do_lookup(std::string const& symbol) const;
};

// returns the actual file name, which is system-dependent, of a dynamic
// library, given its logical name and an optional version
std::string
dynamic_library_filename(
  std::string const& name,
  std::string const& version = std::string()
);

}}}}

#endif
