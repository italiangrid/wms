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

// File: ism_utils.h
// Author: Francesco Giacomini, INFN

#ifndef GLITE_WMS_MANAGER_MAIN_ISM_UTILS_H
#define GLITE_WMS_MANAGER_MAIN_ISM_UTILS_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "glite/wms/ism/purchaser/ism-purchaser.h"

namespace purchaser = glite::wms::ism::purchaser;

namespace glite {
namespace wms {
namespace manager {
namespace main {

class ISM_manager: boost::noncopyable
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;
public:
  std::vector<boost::shared_ptr<purchaser::ism_purchaser> >& purchasers();
  ISM_manager();
};

}}}} // glite::wms::manager::main

#endif
