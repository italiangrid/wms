/* Copyright (c) Members of the EGEE Collaboration. 2004. 
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
limitations under the License. */

// File: FLExtractor.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id$

#ifndef GLITE_WMS_JOBSUBMISSION_COMMON_FLEXTRACTOR_H
#define GLITE_WMS_JOBSUBMISSION_COMMON_FLEXTRACTOR_H

#include <string>
#include <utility>
#include <vector>

#include "common/filelist.h"
#include "common/filelist_lock.h"
#include "common/extractor.h"

#include "jobcontrol_namespace.h"

JOBCONTROL_NAMESPACE_BEGIN {
namespace jccommon {

template<typename T>
class FLExtractor
{
  typename FileList<T> m_filelist;
  FileListMutex m_mutex;
  ForwardExtractor<typename FileList<T> > m_extractor;
public:
  FLExtractor(const std::string& file)
    : m_filelist(file), m_mutex(m_filelist), m_extractor(m_filelist) { }
  ~FLExtractor() { }

  std::pair<ForwardExtractor<typename FileList<T> >::iterator, bool> try_get_one() {
    FileListLock lock(m_mutex);
    iterator next(m_extractor.get_next());
    return std::make_pair(next, next != m_filelist.end());
  }
  std::vector<iterator> get_all_available() {
    std::vector<iterator> result;
    FileListLock lock(m_mutex);
    iterator const end(m_filelist.end());
    for (iterator next(m_extractor.get_next());
         next != end;
         next = m_extractor.get_next()) {
      result.push_back(next);
    }

    return result;
  }
  void erase(const iterator& it) {
    FileListLock lock(m_mutex);
    m_extractor.erase(it);
  }
};

} // namespace jccommon
} JOBCONTROL_NAMESPACE_END

#endif
