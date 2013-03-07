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

//
// File: exceptions.h
// Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
//

#ifndef _GLITE_WMS_WMPROXY_SERVER_EXCEPTIONS_H_
#define _GLITE_WMS_WMPROXY_SERVER_EXCEPTIONS_H_

#include <exception>
#include <string>

#include "glite/wmsutils/exception/Exception.h"

#define _CANNOT_START_ERR_ 1000

namespace glite {
namespace wms {
namespace wmproxy {
namespace server {

class Exception : public glite::wmsutils::exception::Exception
{
protected:
   Exception() {}
   Exception(const std::string& s, const std::string& m, int c, const std::string& n) :
      glite::wmsutils::exception::Exception(s, m, c, n) {}
};

class CannotStartException : public server::Exception
{
public:
   CannotStartException( const std::string& message ) :
      server::Exception( std::string(""), std::string(""), _CANNOT_START_ERR_, "CannotStartException" ) {
      this -> error_message = message;
   }
};

}}}}

#endif
