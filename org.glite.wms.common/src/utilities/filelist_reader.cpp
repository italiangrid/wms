// File: filelist_reader.cpp
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#include "filelist_reader.h"
#include "glite/wms/common/utilities/FLExtractor.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/common/utilities/scope_guard.h"

namespace glite {
namespace wms {
namespace common {
namespace utilities {

namespace {

typedef glite::wms::common::utilities::FLExtractor<std::string> Extractor;
typedef boost::shared_ptr<Extractor> ExtractorPtr;

}

struct FileListReader::Impl
{
  ExtractorPtr extractor;
  std::string source;
};

FileListReader::FileListReader(std::string const& source)
  : m_impl(new Impl)
{
  m_impl->extractor.reset(new Extractor(source));
  m_impl->source = source;
}

std::string FileListReader::name() const
{
  return "filelist";
}

std::string FileListReader::source() const
{
  return m_impl->source;
}

namespace {

typedef Extractor::iterator extractor_iterator;
typedef std::vector<extractor_iterator> extracted_requests_type;

class CleanUp
{
  ExtractorPtr m_extractor;
  extractor_iterator m_it;

public:
  CleanUp(ExtractorPtr e, extractor_iterator i)
    : m_extractor(e), m_it(i)
  {
  }
  void operator()()
  {
    m_extractor->erase(m_it);
  }
};


}

FileListReader::requests_type FileListReader::read()
{
  requests_type result;

  std::vector<extractor_iterator> new_requests(
    m_impl->extractor->get_all_available()
  );

  std::vector<extractor_iterator>::iterator it = new_requests.begin();
  std::vector<extractor_iterator>::iterator const end = new_requests.end();
  for ( ; it != end; ++it) {
    extractor_iterator request_it = *it;
    std::string const& contents = *request_it;
    cleanup_type cleanup(CleanUp(m_impl->extractor, request_it));
    result.push_back(std::make_pair(contents, cleanup));
  }

  return result;
}

}}}}
