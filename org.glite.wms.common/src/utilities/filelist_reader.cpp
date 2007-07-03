// File: filelist_reader.cpp
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#include "filelist_reader.h"
#include "glite/wms/common/utilities/FLExtractor.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "scope_guard.h"

namespace glite {
namespace wms {
namespace common {
namespace utilities {

namespace {

typedef FLExtractor<std::string> Extractor;
typedef boost::shared_ptr<Extractor> ExtractorPtr;
typedef Extractor::iterator extractor_iterator;

}

struct FileListItem::Impl
{
  ExtractorPtr extractor;
  extractor_iterator it;
};

FileListItem::FileListItem(ExtractorPtr extractor, extractor_iterator it)
  : m_impl(new Impl)
{
  m_impl->extractor = extractor;
  m_impl->it = it;
}

std::string FileListItem::value() const
{
  return *m_impl->it;
}

void FileListItem::remove_from_input()
{
  m_impl->extractor->erase(m_impl->it);
}

struct FileListReader::Impl
{
  std::string source;
  ExtractorPtr extractor;
};

FileListReader::FileListReader(std::string const& source)
  : m_impl(new Impl)
{
  m_impl->source = source;
  m_impl->extractor.reset(new Extractor(source));
}

std::string FileListReader::name() const
{
  return "filelist";
}

std::string FileListReader::source() const
{
  return m_impl->source;
}

FileListReader::InputItems FileListReader::read()
{
  InputItems result;

  //typedef std::vector<extractor_iterator> extracted_requests_type;
  std::vector<extractor_iterator> new_requests(
    m_impl->extractor->get_all_available()
  );

  std::vector<extractor_iterator>::iterator it = new_requests.begin();
  std::vector<extractor_iterator>::iterator const end = new_requests.end();
  for ( ; it != end; ++it) {
    extractor_iterator request_it = *it;
    InputItemPtr item(new FileListItem(m_impl->extractor, request_it));
    result.push_back(item);
  }

  return result;
}

}}}}
