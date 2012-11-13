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

// File: exceptions.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2003 EU DataGrid.

// $Id$

#ifndef GLITE_WMS_HELPER_EXCEPTIONS_H
#define GLITE_WMS_HELPER_EXCEPTIONS_H

#include <exception>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/path.hpp>

namespace glite {
namespace jdl {
class CannotGetAttribute;
class CannotSetAttribute;
}

namespace wms {

namespace helper {

class HelperError: public std::exception
{
  std::string m_helper;

public:
  explicit HelperError(std::string const& helper);
  ~HelperError() throw();
  char const* what() const throw()
  {
    return "HelperError";
  }
  std::string helper() const;
};

class NoSuchHelper: public HelperError
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  NoSuchHelper(std::string const& helper);
  ~NoSuchHelper() throw();
  char const* what() const throw();
};

class CannotGetAttribute: public HelperError
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  CannotGetAttribute(std::string const& attribute,
                     std::string const& attribute_type,
                     std::string const& helper);
  CannotGetAttribute(glite::jdl::CannotGetAttribute const& e,
                     std::string const& helper);
  ~CannotGetAttribute() throw();
  char const* what() const throw();
  std::string attribute() const;
  std::string attribute_type() const;
};

class InvalidAttributeValue: public HelperError
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;
public:
  InvalidAttributeValue(std::string const& attribute,
                        std::string const& value,
                        std::string const& expected,
                        std::string const& helper);
  ~InvalidAttributeValue() throw();
  char const* what() const throw();
  std::string attribute() const;
  std::string value() const;
  std::string expected() const;
};

class CannotSetAttribute: public HelperError
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  CannotSetAttribute(std::string const& attribute, std::string const& helper);
  CannotSetAttribute(glite::jdl::CannotSetAttribute const& e,
                     std::string const& helper);
  ~CannotSetAttribute() throw();
  char const* what() const throw();
  std::string attribute() const;
};

class FileSystemError: public HelperError
{
  std::string m_error;
public:
  FileSystemError(std::string const& helper,
                  std::string const& error);
  ~FileSystemError() throw();
  char const* what() const throw();
};

} // namespace helper
} // namespace wms
} // namespace glite

#endif

// Local Variables:
// mode: c++
// End:
