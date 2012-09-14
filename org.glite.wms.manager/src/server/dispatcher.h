// File: dispatcher.h
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#ifndef GLITE_WMS_MANAGER_SERVER_DISPATCHER_H
#define GLITE_WMS_MANAGER_SERVER_DISPATCHER_H

#include <string>
#include <boost/shared_ptr.hpp>
#include "pipedefs.h"

namespace glite {
namespace wms {
namespace manager {
namespace server {

class InputReader;

class Dispatcher:
    public glite::wms::common::task::PipeWriter<RequestPtr, queue_type>
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  Dispatcher(InputReader& reader, TaskQueue& tq);
  void operator()();
};

}}}} // glite::wms::manager::server

#endif
