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

#include <iostream>

#include "boost/filesystem/operations.hpp"
#include "boost/version.hpp"

#include "listfiles.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

namespace logger = glite::wms::common::logger;

#define edglog(level) logger::threadsafe::edglog << logger::setlevel(logger::level)

namespace fs = boost::filesystem;

namespace glite {
namespace wms {
namespace wmproxy {
namespace commands {

void list_files( const fs::path& p, std::vector<std::string>& v)
{
   if ( fs::exists(p) && fs::is_directory( p ) ) {
      fs::directory_iterator end_iter;
      for ( fs::directory_iterator dir_itr( p );
            dir_itr != end_iter;
            ++dir_itr ) {
         try {
            if ( !fs::is_directory( *dir_itr ) ) {
#if BOOST_VERSION >= 103400
               v.push_back(dir_itr->path().file_string());
#else
               v.push_back(dir_itr->native_file_string());
#endif
            }
         } catch ( const std::exception& ex ) {
#if BOOST_VERSION >= 103400
            edglog(warning) << dir_itr->path().file_string() << ' ' << ex.what() << std::endl;
#else
            edglog(warning) << dir_itr->native_file_string() << ' ' << ex.what() << std::endl;
#endif
         }
      }
   }
}

}}}}
