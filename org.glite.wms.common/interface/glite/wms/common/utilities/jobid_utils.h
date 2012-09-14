// Copyright (c) Members of the EGEE Collaboration. 2004. 
// See http://www.eu-egee.org/partners/ for details on the copyright
// holders.  
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//
//     http://www.apache.org/licenses/LICENSE-2.0 
//
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

// $Id$

/**
   @file jobid_utils.h
   @brief Utilities involving JobId's used on the WMS
*/

#ifndef GLITE_WMS_COMMON_UTILITIES_JOBID_UTILS_H
#define GLITE_WMS_COMMON_UTILITIES_JOBID_UTILS_H

#include <string>
#include "glite/jobid/JobId.h"

namespace glite {
namespace wms {
namespace common {

/**
   @brief Generate a filename from a JobId

   The generated filename is build according to the following
   rules:

   - if n_prefix is positive, the filename includes two path components: the
   first is given by the first n_prefix characters of the id unique string; the
   second part is given but the id unique string itself

   - else the filename is given by the id unique string alone

   @param id the JobId
   @param n_prefix the size of the prefix
   @result a string representing a filename
*/
inline
std::string
to_filename(glite::jobid::JobId const& id, int n_prefix = 2)
{
  std::string const& u(id.unique());
  if (n_prefix > 0) {
    std::string result(u.substr(0, n_prefix));
    result += '/';
    result += u;
    return result;
  } else {
    return u;
  }
}

}}}

#endif
