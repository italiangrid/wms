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

#ifndef GLITE_WMS_COMMON_UTILITIES_MANIPULATION_H
#define GLITE_WMS_COMMON_UTILITIES_MANIPULATION_H

#include <string>

namespace glite {

namespace jobid {
class JobId;
}

namespace wms {
namespace common {
namespace utilities {

std::string get_reduced_part( const glite::jobid::JobId &id, int level = 0 );
std::string to_filename( const glite::jobid::JobId &id );
glite::jobid::JobId from_filename( const std::string &filename );

} // namespace utilities
} // namespace common
} // namespace wms
} // namespace glite

#endif /* GLITE_WMS_COMMON_UTILITIES_MANIPULATION_H */

// Local Variables:
// mode: c++
// End:
