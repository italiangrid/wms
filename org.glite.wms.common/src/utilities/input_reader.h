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
#include <boost/shared_ptr.hpp>

namespace glite {
namespace wms {
namespace common {
namespace utilities {

class InputItem
{
public:
  virtual ~InputItem();
  virtual std::string value() const = 0;
  virtual void remove_from_input() = 0;
};

typedef boost::shared_ptr<InputItem> InputItemPtr;

class InputReader
{
public:
  typedef std::vector<InputItemPtr> InputItems;
  
  virtual ~InputReader();
  virtual std::string name() const = 0;
  virtual std::string source() const = 0;
  virtual InputItems read() = 0;
};

}}}}

#endif
