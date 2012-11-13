// File: ism_utils.h
// Author: Francesco Giacomini, INFN

#ifndef GLITE_WMS_MANAGER_MAIN_ISM_UTILS_H
#define GLITE_WMS_MANAGER_MAIN_ISM_UTILS_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace glite {
namespace wms {
namespace manager {
namespace main {

class ISM_Manager: boost::noncopyable
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;
public:
  ISM_Manager();
  ~ISM_Manager();
};

}}}} // glite::wms::manager::main

#endif
