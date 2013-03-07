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
// File: dynamic_library.cpp
// Author: Francesco Giacomini, INFN

// $Id$

#include <dlfcn.h>
#include "glite/wms/common/utilities/dynamic_library.h"

namespace glite {
namespace wms {
namespace common {
namespace utilities {

struct CannotLoadDynamicLibrary::Implementation
{
  std::string filename;
  std::string error;
};

CannotLoadDynamicLibrary::CannotLoadDynamicLibrary(
  std::string const& filename,
  std::string const& error
)
  : m_impl(new Implementation)
{
  m_impl->filename = filename;
  m_impl->error = error;
}

std::string
CannotLoadDynamicLibrary::filename() const
{
  return m_impl->filename;
}

std::string
CannotLoadDynamicLibrary::error() const
{
  return m_impl->error;
}

struct CannotLookupSymbol::Implementation
{
  std::string symbol;
  std::string error;
};

CannotLookupSymbol::CannotLookupSymbol(
  std::string const& symbol,
  std::string const& error
)
  : m_impl(new Implementation)
{
  m_impl->symbol = symbol;
  m_impl->error = error;
}

std::string
CannotLookupSymbol::symbol() const
{
  return m_impl->symbol;
}

std::string
CannotLookupSymbol::error() const
{
  return m_impl->error;
}

struct DynamicLibrary::Implementation
{
  void* handle;
};

DynamicLibrary::DynamicLibrary(std::string const& filename)
  : m_impl(new Implementation)
{
  m_impl->handle = ::dlopen(filename.c_str(), RTLD_NOW | RTLD_GLOBAL);
  if (!m_impl->handle) {
    throw CannotLoadDynamicLibrary(filename, ::dlerror());
  }
}

void*
DynamicLibrary::lookup(std::string const& symbol) const
{
  void* result = ::dlsym(m_impl->handle, symbol.c_str());
  if (!result) {
    throw CannotLookupSymbol(symbol, ::dlerror());
  }
  return result;
}

namespace {

std::string const lib_prefix("lib");
std::string const so_suffix(".so");

}

std::string
dynamic_library_filename(std::string const& name, std::string const& version)
{
  std::string result = lib_prefix + name + so_suffix;
  if (!version.empty()) {
    result += '.' + version;
  }

  return result;
}

}}}} // glite::wms::common::utilities
