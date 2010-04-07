/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://project.eu-egee.org/partners/ for details on the
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

// File: ssl_utils.h
// Author: Salvatore Monforte

// $Id$

#include <string>

namespace sslutils
{

bool
proxy_expires_within(
  std::string const& x509_proxy, 
  time_t seconds
);

bool
proxy_init(
  std::string const& certfile,
  std::string const& keyfile,
  std::string const& outfile,
  time_t seconds
);

} // namespace sslutils closure
