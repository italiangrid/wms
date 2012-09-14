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

// File: exceptions.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2003 EU DataGrid.

// $Id$

#include "glite/wms/helper/exceptions.h"

#include <string>

#include "glite/jdl/ManipulationExceptions.h"

namespace glite {
namespace wms {
namespace helper {

namespace {

std::string const empty_string;

}

HelperError::HelperError(std::string const& helper)
  : m_helper(helper)
{
}

HelperError::~HelperError() throw()
{
}

std::string
HelperError::helper() const
{
  return m_helper;
}

struct NoSuchHelper::Impl
{
  std::string m_what;
};

NoSuchHelper::NoSuchHelper(std::string const& helper)
  : HelperError(helper)
{
  try {
    m_impl.reset(new Impl);
  } catch (...) {
    m_impl.reset();
  }
}

NoSuchHelper::~NoSuchHelper() throw()
{
}

char const*
NoSuchHelper::what() const throw()
{
  if (m_impl) {
    m_impl->m_what = helper() + ": no such helper";
    return m_impl->m_what.c_str();
  } else {
    return "HelperError: no such helper";
  }
}

struct CannotGetAttribute::Impl
{
  std::string m_what;
  std::string m_attribute;
  std::string m_type;
};

CannotGetAttribute::CannotGetAttribute(
  std::string const& attribute_name,
  std::string const& attribute_type,
  std::string const& helper
)
  : HelperError(helper)
{
  try {
    m_impl.reset(new Impl);
    m_impl->m_attribute = attribute_name;
    m_impl->m_type = attribute_type;
  } catch (...) {
    m_impl.reset();
  }
}

CannotGetAttribute::CannotGetAttribute(
  glite::jdl::CannotGetAttribute const& e,
  std::string const& helper
)
  : HelperError(helper)
{
  try {
    m_impl.reset(new Impl);
    m_impl->m_attribute = e.parameter();
    m_impl->m_type = "unknown";
  } catch (...) {
    m_impl.reset();
  }
}

CannotGetAttribute::~CannotGetAttribute() throw()
{
}

std::string
CannotGetAttribute::attribute() const
{
  return m_impl ? m_impl->m_attribute : empty_string;
}

std::string
CannotGetAttribute::attribute_type() const
{
  return m_impl ? m_impl->m_type : empty_string;
}

char const*
CannotGetAttribute::what() const throw()
{
  if (m_impl) {
    m_impl->m_what
      = helper() + ": attribute " + attribute()
      + " does not exist or has the wrong type (expected "
      + attribute_type() + ")";
    return m_impl->m_what.c_str();
  } else {
    return "HelperError: CannotGetAttribute";
  }
}

struct InvalidAttributeValue::Impl
{
  std::string m_what;
  std::string m_attribute;
  std::string m_value;
  std::string m_expected;
};

InvalidAttributeValue::InvalidAttributeValue(
  std::string const& attribute,
  std::string const& value,
  std::string const& expected,
  std::string const& helper
)
  : HelperError(helper)
{
  try {
    m_impl.reset(new Impl);
    m_impl->m_attribute = attribute;
    m_impl->m_value = value;
    m_impl->m_expected = expected;
  } catch (...) {
    m_impl.reset();
  }
}

InvalidAttributeValue::~InvalidAttributeValue() throw()
{
}

std::string
InvalidAttributeValue::attribute() const
{
  return m_impl ? m_impl->m_attribute : empty_string;
}

std::string
InvalidAttributeValue::value() const
{
  return m_impl ? m_impl->m_value : empty_string;
}

std::string
InvalidAttributeValue::expected() const
{
  return m_impl ? m_impl->m_expected : empty_string;
}

char const*
InvalidAttributeValue::what() const throw()
{
  if (m_impl) {
    m_impl->m_what
      = helper() + ": invalid value " + value()
      + " for attribute " + attribute() + " (expecting " + expected() + ")";
    return m_impl->m_what.c_str();
  } else {
    return "HelperError: InvalidAttributeValue";
  }
}

struct CannotSetAttribute::Impl
{
  std::string m_what;
  std::string m_attribute;
};

CannotSetAttribute::CannotSetAttribute(
  std::string const& attribute,
  std::string const& helper
)
  : HelperError(helper)
{
  try {
    m_impl.reset(new Impl);
    m_impl->m_attribute = attribute;
  } catch (...) {
    m_impl.reset();
  }
}

CannotSetAttribute::CannotSetAttribute(
  glite::jdl::CannotSetAttribute const& e,
  std::string const& helper
)
  : HelperError(helper)
{
  try {
    m_impl.reset(new Impl);
    m_impl->m_attribute = e.parameter();
  } catch (...) {
    m_impl.reset();
  }
}

CannotSetAttribute::~CannotSetAttribute() throw()
{
}

std::string
CannotSetAttribute::attribute() const
{
  return m_impl ? m_impl->m_attribute : empty_string;
}

char const*
CannotSetAttribute::what() const throw()
{
  if (m_impl) {
    m_impl->m_what = helper() + ": cannot set attribute " + attribute();
    return m_impl->m_what.c_str();
  } else {
    return "HelperError: CannotSetAttribute";
  }
}

FileSystemError::FileSystemError(
  std::string const& helper,
  std::string const& error 
)
  : HelperError(helper),
    m_error(error)
{
}

FileSystemError::~FileSystemError() throw()
{
}

char const*
FileSystemError::what() const throw()
{
  return m_error.c_str();
}

}}} // glite::wms::helper
