// File: FLExtractor.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

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
