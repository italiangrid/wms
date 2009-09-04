#ifndef GLITE_WMS_COMMON_UTILITIES_BOOST_FS_ADD_H
#define GLITE_WMS_COMMON_UTILITIES_BOOST_FS_ADD_H

#include <string>
#include <iostream>
#include <boost/filesystem/path.hpp>

/**
 *  This file adds some functions to the boost::filesystem
 *  namespace, so it does not follow the EDG naming directives.
 */

namespace boost { namespace filesystem {

void create_parents( const path &dpath );
std::string normalize_path( const std::string &fpath );

}};

#endif /* GLITE_WMS_COMMON_UTILITIES_BOOST_FS_ADD_H */

// Local Variables:
// mode: c++
// End:
