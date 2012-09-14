// File: filelist_reader.h
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#ifndef GLITE_WMS_MANAGER_SERVER_FILELISTREADER_H
#define GLITE_WMS_MANAGER_SERVER_FILELISTREADER_H

#include "input_reader.h"

namespace glite {
namespace wms {
namespace manager {
namespace server {

class FileListReader: public InputReader
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  FileListReader(std::string const& source);
  std::string name() const;
  std::string source() const;
  requests_type read();
};

}}}} // glite::wms::manager::server

#endif
