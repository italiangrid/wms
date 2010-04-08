/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org\partners for details on the
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

#ifndef GLITE_WMS_COMMON_UTILITIES_QUOTA_H
#define GLITE_WMS_COMMON_UTILITIES_QUOTA_H

#include <string>

namespace glite {
namespace wms {
namespace common {
namespace utilities {
namespace quota {

  extern std::pair<long, long> getFreeQuota(const std::string&);
  extern std::pair<long, long> getQuota(const std::string&);

  extern uid_t user2uid(const char *name);

} // namespace quota
} // namespace utilities
} // namespace common
} // namespace wms
} // namespace glite

// Local Variables:
// mode: c++
// End:
// 

#endif /* GLITE_WMS_COMMON_UTILITIES_QUOTA_H */
