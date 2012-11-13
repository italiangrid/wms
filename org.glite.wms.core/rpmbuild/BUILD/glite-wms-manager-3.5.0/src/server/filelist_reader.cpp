// File: filelist_reader.cpp
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#include "filelist_reader.h"
#include <classad_distribution.h>
#include "glite/wms/common/utilities/FLExtractor.h"
#include "glite/wms/common/logger/logger_utils.h"
#include "glite/wms/common/utilities/scope_guard.h"

namespace glite {
namespace wms {
namespace manager {
namespace server {

namespace {

typedef glite::wms::common::utilities::FLExtractor<std::string> extractor_type;
typedef boost::shared_ptr<extractor_type> ExtractorPtr;

}

struct FileListReader::Impl
{
  ExtractorPtr extractor;
  std::string source;
};

FileListReader::FileListReader(std::string const& source)
  : m_impl(new Impl)
{
  m_impl->extractor.reset(new extractor_type(source));
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

typedef extractor_type::iterator extractor_iterator;
typedef std::vector<extractor_iterator> extracted_requests_type;

class FLCleanUp
{
  ExtractorPtr m_extractor;
  extractor_iterator m_it;

public:
  FLCleanUp(ExtractorPtr e, extractor_iterator i)
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

  classad::ClassAdParser parser;
  std::vector<extractor_iterator>::iterator it = new_requests.begin();
  std::vector<extractor_iterator>::iterator const end = new_requests.end();
  for ( ; it != end; ++it) {

    extractor_type::iterator request_it = *it;
    cleanup_type cleanup(FLCleanUp(m_impl->extractor, request_it));
    // if the request is not valid, clean it up automatically
    glite::wms::common::utilities::scope_guard cleanup_guard(cleanup);

    std::string const& command_ad_str = *request_it;
    ClassAdPtr command_ad(parser.ParseClassAd(command_ad_str));
    if (command_ad) {
      cleanup_guard.dismiss();
      result.push_back(std::make_pair(command_ad, cleanup));
    } else {
      Info("invalid request");
    }

  }

  return result;
}

}}}} // glite::wms::manager::server
