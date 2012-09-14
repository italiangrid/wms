// File: jobdir_reader.h
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#ifndef GLITE_WMS_COMMON_UTILITIES_JOBDIRREADER_H
#define GLITE_WMS_COMMON_UTILITIES_JOBDIRREADER_H

#include "input_reader.h"
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>

namespace glite {
namespace wms {
namespace common {
namespace utilities {

class JobDirItem: public InputItem
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;
public:
  JobDirItem(boost::filesystem::path const&);
  std::string value() const;
  void remove_from_input();
};

class JobDirReader: public InputReader
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  JobDirReader(std::string const& source);
  std::string name() const;
  std::string source() const;
  InputItems read();
};

}}}}

#endif
