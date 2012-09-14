// File: DispatcherFromJobDir.h
// Author: Francesco Giacomini

// $Id$

#ifndef GLITE_WMS_MANAGER_SERVER_DISPATCHERFROMJOBDIR_H
#define GLITE_WMS_MANAGER_SERVER_DISPATCHERFROMJOBDIR_H

#include <string>
#include <boost/shared_ptr.hpp>
#include "pipedefs.h"

namespace glite {
namespace wms {
namespace manager {
namespace server {

class DispatcherFromJobDir:
    public glite::wms::common::task::PipeWriter<RequestPtr, queue_type>
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  DispatcherFromJobDir(std::string const& base_dir, TaskQueue& tq);
  void operator()();
};

}}}} // glite::wms::manager::server

#endif
