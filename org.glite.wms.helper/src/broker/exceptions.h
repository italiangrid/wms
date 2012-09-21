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

// $Id: exceptions.h,v 1.1.2.2 2012/09/12 10:02:12 mcecchi Exp $

#ifndef GLITE_WMS_HELPER_BROKER_EXCEPTIONS_H
#define GLITE_WMS_HELPER_BROKER_EXCEPTIONS_H

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>

#include "glite/wms/helper/exceptions.h"
#include "exceptions.h"

namespace glite {
namespace wms {
namespace helper {
namespace broker {

class CannotCreateBrokerinfo: public helper::HelperError
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;
public:
  explicit CannotCreateBrokerinfo(boost::filesystem::path const& p);
  ~CannotCreateBrokerinfo() throw();
  boost::filesystem::path path() const;
  char const* what() const throw();
};

class NoAvailableCEs: public helper::HelperError
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;
public:
  explicit NoAvailableCEs(std::string const& reason);
  ~NoAvailableCEs() throw();
  std::string reason() const;
  char const* what() const throw();
};

class NoCompatibleCEs: public helper::HelperError
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;
public:
  typedef enum { FALSE_REQS, INVALID_REQS, ERROR_REQS, DONTKNOW } reason_type;
  NoCompatibleCEs();
  NoCompatibleCEs(std::string const& is_hostname, reason_type reason);
  ~NoCompatibleCEs() throw();
  std::string is_hostname() const;
  reason_type reason() const;
  char const* what() const throw();
};

}}}} // glite::wms::helper::broker

#endif

// Local Variables:
// mode: c++
// End:
