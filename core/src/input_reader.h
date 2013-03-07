/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// File: input_reader.h
// Author: Francesco Giacomini

// $Id: input_reader.h,v 1.1.2.1.4.2 2010/04/07 14:02:46 mcecchi Exp $

#ifndef GLITE_WMS_MANAGER_SERVER_INPUTREADER_H
#define GLITE_WMS_MANAGER_SERVER_INPUTREADER_H

#include <string>
#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace classad {
class ClassAd;
}
typedef boost::shared_ptr<classad::ClassAd> ClassAdPtr;

namespace glite {
namespace wms {
namespace manager {
namespace server {

class InputReader
{
public:
  typedef boost::function<void()> cleanup_type;
  typedef std::vector<std::pair<ClassAdPtr, cleanup_type> > requests_type;
  
  virtual ~InputReader();
  virtual std::string name() const = 0;
  virtual std::string source() const = 0;
  virtual requests_type read() = 0;
};

}}}} // glite::wms::manager::server

#endif
