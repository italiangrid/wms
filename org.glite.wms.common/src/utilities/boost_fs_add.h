#ifndef GLITE_WMS_COMMON_UTILITIES_BOOST_FS_ADD_H
#define GLITE_WMS_COMMON_UTILITIES_BOOST_FS_ADD_H

#include <string>
#include <iostream>
#include <boost/filesystem/path.hpp>

/**
 *  This file adds some functions to the boost::filesystem
 *  namespace, so it does not follow the EDG naming directives.
 */

namespace fs = boost::filesystem;

namespace glite { 
namespace wms { 
namespace common {
namespace utilities {

class CannotCreateParents: public std::exception {
  std::string m_error;
public:
  CannotCreateParents(std::string const& error) : m_error(error) { }
  CannotCreateParents() throw() { }
  ~CannotCreateParents() throw() { }
  char const* what() const throw()
  {
    return m_error.c_str();
  }
};

void create_parents( const fs::path &dpath );
std::string normalize_path( const std::string &fpath );

}}}};

#endif /* GLITE_WMS_COMMON_UTILITIES_BOOST_FS_ADD_H */

// Local Variables:
// mode: c++
// End:
