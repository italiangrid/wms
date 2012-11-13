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

// File: dynamic_library.cpp
// Author: Francesco Giacomini, INFN

// $Id: dynamic_library.cpp,v 1.1.2.1.4.2 2010/04/07 14:02:46 mcecchi Exp $

#include <dlfcn.h>
#include "dynamic_library.h"

namespace glite {
namespace wms {
namespace manager {
namespace server {

struct CannotLoadDynamicLibrary::Impl
{
  std::string filename;
  std::string error;
};

CannotLoadDynamicLibrary::CannotLoadDynamicLibrary(
  std::string const& filename,
  std::string const& error
)
  : m_impl(new Impl)
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

struct CannotLookupSymbol::Impl
{
  std::string symbol;
  std::string error;
};

CannotLookupSymbol::CannotLookupSymbol(
  std::string const& symbol,
  std::string const& error
)
  : m_impl(new Impl)
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

struct DynamicLibrary::Impl
{
  void* handle;
  ~Impl()
  {
    if (handle) {
      ::dlclose(handle);
    }
  }
};

DynamicLibrary::DynamicLibrary(std::string const& filename, int flags)
  : m_impl(new Impl)
{
  int real_flags = 0;
  if (flags & global_visibility) {
    real_flags |= RTLD_GLOBAL;
  }
  if (flags & immediate_binding) {
    real_flags |= RTLD_NOW;
  } else if (flags & lazy_binding) {
    real_flags |= RTLD_LAZY;
  }

  m_impl->handle = ::dlopen(filename.c_str(), real_flags);
  if (!m_impl->handle) {
    throw CannotLoadDynamicLibrary(filename, ::dlerror());
  }
}

void*
DynamicLibrary::do_lookup(std::string const& symbol) const
{
  ::dlerror();                  // clear error state
  void* result = ::dlsym(m_impl->handle, symbol.c_str());
  char const* error = ::dlerror();
  if (error) {
    throw CannotLookupSymbol(symbol, error);
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

}}}}
