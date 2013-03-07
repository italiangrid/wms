/*
Copyright (c) Members of the EGEE Collaboration. 2004. 
See http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

    http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
See the License for the specific language governing permissions and 
limitations under the License.
*/
// File: dynamic_library.h
// Author: Francesco Giacomini, INFN

// $Id$

#ifndef DYNAMIC_LIBRARY_H
#define DYNAMIC_LIBRARY_H

#include <exception>
#include <string>
#include <boost/shared_ptr.hpp>

namespace glite {
namespace wms {
namespace common {
namespace utilities {

class CannotLoadDynamicLibrary: public std::exception
{
  class Implementation;
  boost::shared_ptr<Implementation> m_impl;

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
  class Implementation;
  boost::shared_ptr<Implementation> m_impl;

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
  class Implementation;
  boost::shared_ptr<Implementation> m_impl;

public:
  // load a dynamic library with the given filename
  // throw CannotLoadDynamicLibrary if the operation fails
  DynamicLibrary(std::string const& filename);

  // lookup a symbol in the dynamic library
  // throw CannotLookupSymbol if the operation fails
  void* lookup(std::string const& symbol) const;
};

// returns the actual file name, which is system-dependent, of a dynamic
// library, given its logical name and an optional version
std::string
dynamic_library_filename(
  std::string const& name,
  std::string const& version = std::string()
);

// lookup a symbol in the dynamic library in a type-safe manner
// throw CannotLookupSymbol if the operation fails
template<typename T>
T
lookup(DynamicLibrary const& dl, std::string const& symbol)
{
  return (T)dl.lookup(symbol);
}

}}}} // glite::wms::common::utilities

#endif
