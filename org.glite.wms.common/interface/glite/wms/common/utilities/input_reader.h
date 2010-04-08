/*
Copyright (c) Members of the EGEE Collaboration. 2004. 
See http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

    http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
See the License for the specific language governing permissions and 
limitations under the License.
*/
// File: input_reader.h
// Author: Francesco Giacomini

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
