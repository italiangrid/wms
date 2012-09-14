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
// File: FLExtractor.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id$

#ifndef GLITE_WMS_COMMON_UTILITIES_FLEXTRACTOR_H
#define GLITE_WMS_COMMON_UTILITIES_FLEXTRACTOR_H

#ifndef GLITE_WMS_X_STRING
#define GLITE_WMS_X_STRING
#include <string>
#endif
#ifndef GLITE_WMS_X_UTILITY
#define GLITE_WMS_X_UTILITY
#include <utility>
#endif
#ifndef GLITE_WMS_X_VECTOR
#define GLITE_WMS_X_VECTOR
#include <vector>
#endif
#ifndef GLITE_WMS_COMMON_UTILITIES_FILELIST_H
#include "glite/wms/common/utilities/FileList.h"
#endif
#ifndef GLITE_WMS_COMMON_UTILITIES_FILELISTLOCK_H
#include "glite/wms/common/utilities/FileListLock.h"
#endif
#ifndef GLITE_WMS_COMMON_UTILITIES_EXTRACTOR_H
#include "glite/wms/common/utilities/Extractor.h"
#endif

namespace glite {
namespace wms {
namespace common {
namespace utilities {

template<typename T>
class FLExtractor
{
  typedef typename glite::wms::common::utilities::FileList<T> FL;
  typedef glite::wms::common::utilities::FileListMutex FLM;
  typedef glite::wms::common::utilities::ForwardExtractor<FL> FE;

  FL m_filelist;
  FLM m_mutex;
  FE m_extractor;

public:
  typedef typename FE::iterator iterator;
  typedef typename FE::value_type value_type;

public:
  FLExtractor(const std::string& file);
  ~FLExtractor();

  std::pair<iterator, bool> try_get_one();
  std::vector<iterator> get_all_available();
  void erase(const iterator& it);

};

} // namespace utilities
} // namespace common
} // namespace wms 
} // namespace glite

#include "glite/wms/common/utilities/FLExtractor.cpp"

#endif
