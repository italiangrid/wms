#ifndef GLITE_WMS_COMMON_UTILITIES_MANIPULATION_H
#define GLITE_WMS_COMMON_UTILITIES_MANIPULATION_H

#include <string>

namespace gljobid = glite::jobid; 

namespace glite {

namespace jobid {
class JobId;
}

namespace wms {
namespace common {
namespace utilities {

std::string get_reduced_part( const gljobid::JobId &id, int level = 0 );
std::string to_filename( const gljobid::JobId &id );
gljobid::JobId from_filename( const std::string &filename );

} // namespace utilities
} // namespace common
} // namespace wms
} // namespace glite

#endif /* GLITE_WMS_COMMON_UTILITIES_MANIPULATION_H */

// Local Variables:
// mode: c++
// End:
