/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
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

// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
#ifndef GLITE_WMS_ISM_PURCHASER_ISM_PURCHASER_H
#define GLITE_WMS_ISM_PURCHASER_ISM_PURCHASER_H

#include <string>
#include <boost/function.hpp>

#include "glite/wms/ism/purchaser/common.h"
 
namespace glite {
namespace wms {
namespace ism {
namespace purchaser {

inline bool false_(std::string const&) { return false; }

class ism_purchaser
{
public:
  ism_purchaser(
    exec_mode_t mode, 
    size_t interval, 
    exit_predicate_type exit_predicate = exit_predicate_type(),
    skip_predicate_type skip_predicate = skip_predicate_type())
      :  m_mode(mode),
         m_interval(interval),
         m_exit_predicate(exit_predicate),
         m_skip_predicate(skip_predicate) { }

  virtual ~ism_purchaser() { }
  virtual void operator()() = 0;

  exec_mode_t exec_mode() const
  {
    return m_mode;
  }
  size_t sleep_interval() const
  {
    return m_interval;
  }

  void exit_predicate(exit_predicate_type const& p) {
    m_exit_predicate = p; 
  }

  void skip_predicate(skip_predicate_type const &p) {
    m_skip_predicate = p;
  }

protected:               
  exec_mode_t m_mode;
  size_t m_interval;
  exit_predicate_type m_exit_predicate;
  skip_predicate_type m_skip_predicate;
};

} // namespace purchaser
} // namespace ism
} // namespace wms
} // namespace glite

#endif
