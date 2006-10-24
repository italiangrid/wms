// File: input_reader.h
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#ifndef GLITE_WMS_COMMON_UTILITIES_INPUTREADER_H
#define GLITE_WMS_COMMON_UTILITIES_INPUTREADER_H

#include <string>
#include <vector>
#include <utility>
#include <boost/function.hpp>

namespace glite {
namespace wms {
namespace common {
namespace utilities {

class InputReader
{
public:
  typedef boost::function<void()> cleanup_type;
  typedef std::vector<std::pair<std::string, cleanup_type> > requests_type;
  
  virtual ~InputReader();
  virtual std::string name() const = 0;
  virtual std::string source() const = 0;
  virtual requests_type read() = 0;
};

}}}}

#endif
