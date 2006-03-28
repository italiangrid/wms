// File: FLExtractor.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

namespace glite {
namespace wms {
namespace common {
namespace utilities {

template<typename T>
FLExtractor<T>::FLExtractor(const std::string& file)
  : m_filelist(file), m_mutex(m_filelist), m_extractor(m_filelist)
{
}

template<typename T>
FLExtractor<T>::~FLExtractor()
{
}

template<typename T>
std::pair<typename FLExtractor<T>::iterator, bool>
FLExtractor<T>::try_get_one()
{
  FileListLock lock(m_mutex);
  iterator next(m_extractor.get_next());
  return std::make_pair(next, next != m_filelist.end());
}

template<typename T>
std::vector<typename FLExtractor<T>::iterator>
FLExtractor<T>::get_all_available()
{
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

template<typename T>
void
FLExtractor<T>::erase(const iterator& it)
{
  FileListLock lock(m_mutex);
  m_extractor.erase(it);
}

} // namespace utilities
} // namespace common
} // namespace wms
} // namespace glite

