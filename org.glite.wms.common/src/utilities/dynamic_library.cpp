// File: dynamic_library.cpp
// Author: Francesco Giacomini, INFN

// $Id$

#include <dlfcn.h>
#include "dynamic_library.h"

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

namespace {

std::string const prefix("lib");
std::string const suffix(".so");

std::string to_filename(std::string const& name, std::string const& version)
{
  std::string result = prefix + name + suffix;
  if (!version.empty()) {
    result += '.';
    result += version;
  }

  return result;
}

}

struct DynamicLibrary::Implementation
{
  void* handle;
};

DynamicLibrary::DynamicLibrary(
  std::string const& name,
  std::string const& version
)
  : m_impl(new Implementation)
{
  std::string filename(to_filename(name, version));
  m_impl->handle = ::dlopen(filename.c_str(), RTLD_NOW | RTLD_GLOBAL);
  if (!m_impl->handle) {
    throw CannotLoadDynamicLibrary(filename.c_str(), ::dlerror());
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

}}}} // glite::wms::common::utilities
