// File: filelist_reader.h
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#ifndef GLITE_WMS_COMMON_UTILITIES_FILELISTREADER_H
#define GLITE_WMS_COMMON_UTILITIES_FILELISTREADER_H

#include "input_reader.h"
#include <boost/shared_ptr.hpp>
#include "glite/wms/common/utilities/FLExtractor.h"

namespace glite {
namespace wms {
namespace common {
namespace utilities {

class FileListItem: public InputItem
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;
  typedef FLExtractor<std::string> Extractor;
  typedef boost::shared_ptr<Extractor> ExtractorPtr;
  typedef Extractor::iterator extractor_iterator;
public:
  FileListItem(ExtractorPtr extractor, extractor_iterator it);
  std::string value() const;
  void remove_from_input();
};

class FileListReader: public InputReader
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  FileListReader(std::string const& source);
  std::string name() const;
  std::string source() const;
  InputItems read();
};

}}}}

#endif
