// File: filelist_utils.h
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

#ifndef GLITE_WMS_MANAGER_SERVER_FILELIST_UTILS_H
#define GLITE_WMS_MANAGER_SERVER_FILELIST_UTILS_H

#include <string>
#include <boost/shared_ptr.hpp>
#include "glite/wms/common/utilities/FLExtractor.h"

namespace glite {
namespace wms {
namespace manager {
namespace server {

typedef glite::wms::common::utilities::FLExtractor<std::string> extractor_type;
typedef boost::shared_ptr<extractor_type> ExtractorPtr;
typedef extractor_type::iterator extractor_iterator;
typedef std::vector<extractor_iterator> requests_type;

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

}}}} // glite::wms::manager::server

#endif
