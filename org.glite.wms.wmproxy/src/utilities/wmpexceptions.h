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
// File: wmpexceptions.h
// Author: Giuseppe Avellino <egee@datamat.it>
//

#ifndef GLITE_WMS_WMPROXY_WMPEXCEPTIONS_H
#define GLITE_WMS_WMPROXY_WMPEXCEPTIONS_H

#include "wmpexception_codes.h"
#include "glite/wmsutils/exception/Exception.h"

namespace glite {
namespace wms {
namespace wmproxy {
namespace utilities {

class JobException : public glite::wmsutils::exception::Exception
{
protected:
   JobException(const std::string& file, int line, const std::string& method,
                int code, const std::string& exception_name);
};

class JobTimeoutException : public JobException
{
public:
   JobTimeoutException(const std::string& file, int line,
                       const std::string& method, int code);
};

class JobTimedoutException : public JobException
{
public:
   JobTimedoutException(const std::string& file, int line,
                        const std::string& method, int code, const std::string& reason);
};

class JobOperationException : public JobException
{
public:
   JobOperationException(const std::string& file, int line,
                         const std::string& method, int code, const std::string& reason);
};

class ProxyOperationException : public JobException
{
public:
   ProxyOperationException(const std::string& file, int line,
                           const std::string& method, int code, const std::string& reason);
};

class NotAVOMSProxyException : public JobException
{
public:
   NotAVOMSProxyException(const std::string& file, int line,
                          const std::string& method, int code, const std::string& reason);
};

class FileSystemException : public JobException
{
public:
   FileSystemException(const std::string& file, int line,
                       const std::string& method, int code, const std::string& reason);
};

class AuthorizationException : public JobException
{
public:
   AuthorizationException(const std::string& file, int line,
                          const std::string& method, int code, const std::string& reason);
};

class AuthenticationException : public JobException
{
public:
   AuthenticationException(const std::string& file, int line,
                           const std::string& method, int code, const std::string& reason);
};

class GaclException : public JobException
{
public:
   GaclException(const std::string& file, int line,
                 const std::string& method, int code, const std::string& reason);
};

class LBException : public JobException
{
public:
   LBException(const std::string& file, int line,
               const std::string& method, int code, const std::string& reason);
};

class ServerOverloadedException : public JobException
{
public:
   ServerOverloadedException(const std::string& file, int line,
                             const std::string& method, int code, const std::string& reason);
};

class ConfigurationException : public JobException
{
public:
   ConfigurationException(const std::string& file, int line,
                          const std::string& method, int code, const std::string& reason);
};

}}}}

#endif // GLITE_WMS_WMPROXY_WMPEXCEPTIONS_H
