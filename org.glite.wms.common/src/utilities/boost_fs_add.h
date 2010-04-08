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

}}}}

#endif /* GLITE_WMS_COMMON_UTILITIES_BOOST_FS_ADD_H */

// Local Variables:
// mode: c++
// End:
