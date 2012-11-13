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
// File: wmpexceptions.cpp
// Author: Giuseppe Avellino <egee@datamat.it>
//

#include "wmpexceptions.h"

namespace glite {
namespace wms {
namespace wmproxy {
namespace utilities {

using namespace std;
using namespace glite::wmsutils::exception;

JobException::JobException(const std::string& file, int line,
                           const std::string& method, int code, const std::string& exception_name)
   : Exception(file, line, method, code, exception_name)
{}

JobTimeoutException::JobTimeoutException(const std::string& file, int line,
      const std::string& method, int code)
   : JobException(file, line, method, code, "JobTimeoutException")
{
   error_message = "Submit notification timeout expired";
}

JobTimedoutException::JobTimedoutException(const std::string& file, int line,
      const std::string& method, int code, const std::string& reason)
   : JobException(file, line, method, code, "JobTimedoutException")
{
   error_message = " " + reason;
}

JobOperationException::JobOperationException(const std::string& file, int line,
      const std::string& method, int code, const std::string& reason)
   : JobException(file, line, method, code, "JobOperationException")
{
   error_message = "The Operation is not allowed: " + reason;
}

ProxyOperationException::ProxyOperationException(const std::string& file,
      int line, const std::string& method, int code, const std::string& reason)
   : JobException(file, line, method, code, "ProxyOperationException")
{
   error_message = "Proxy exception: " + reason;
}

NotAVOMSProxyException::NotAVOMSProxyException(const std::string& file,
      int line, const std::string& method, int code, const std::string& reason)
   : JobException(file, line, method, code, "NotAVOMSProxyException")
{
   error_message = reason;
}

FileSystemException::FileSystemException(const std::string& file,
      int line, const std::string& method, int code, const std::string& reason)
   : JobException(file, line, method, code, "FileSystemException")
{
   error_message += reason;
}

AuthorizationException::AuthorizationException(const std::string& file,
      int line, const std::string& method, int code, const std::string& reason)
   : JobException(file, line, method, code, "AuthorizationException")
{
   error_message += reason;
}

AuthenticationException::AuthenticationException(const std::string& file,
      int line, const std::string& method, int code, const std::string& reason)
   : JobException(file, line, method, code, "AuthenticationException")
{
   error_message += reason;
}

GaclException::GaclException(const std::string& file,
                             int line, const std::string& method, int code, const std::string& reason)
   : JobException(file, line, method, code, "GaclException")
{
   error_message += reason;
}

LBException::LBException(const std::string& file,
                         int line, const std::string& method, int code, const std::string& reason)
   : JobException(file, line, method, code, "LBException")
{
   error_message += reason;
}

ServerOverloadedException::ServerOverloadedException(const std::string& file,
      int line, const std::string& method, int code, const std::string& reason)
   : JobException(file, line, method, code, "ServerOverloadedException")
{
   error_message += reason;
}

ConfigurationException::ConfigurationException(const std::string& file,
      int line, const std::string& method, int code, const std::string& reason)
   : JobException(file, line, method, code, "ConfigurationException")
{
   error_message += reason;
}

}}}}
