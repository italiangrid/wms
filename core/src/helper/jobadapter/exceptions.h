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

// $Id: exceptions.h,v 1.1.40.2 2010/04/08 13:52:15 mcecchi Exp $

#ifndef GLITE_WMS_HELPER_JOBADAPTER_EXCEPTIONS_H
#define GLITE_WMS_HELPER_JOBADAPTER_EXCEPTIONS_H

#include "glite/wms/helper/exceptions.h"

#include <string>
#include <boost/shared_ptr.hpp>

namespace glite {
namespace wms {
namespace helper {
namespace jobadapter {

class CannotCreateJobWrapper: public helper::HelperError
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  CannotCreateJobWrapper(std::string const& path);
  ~CannotCreateJobWrapper() throw();
  std::string path() const;
  char const* what() const throw();
};

} // namespace jobadapter
} // namespace helper
} // namespace wms
} // namespace glite

#endif

// Local Variables:
// mode: c++
// End:
