// File: DispatcherFromFileList.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#ifndef GLITE_WMS_MANAGER_SERVER_DISPATCHERFROMFILELIST_H
#define GLITE_WMS_MANAGER_SERVER_DISPATCHERFROMFILELIST_H

#include <string>
#include <boost/shared_ptr.hpp>
#include "pipedefs.h"

namespace glite {
namespace wms {
namespace manager {
namespace server {

class DispatcherFromFileList:
    public glite::wms::common::task::PipeWriter<RequestPtr, queue_type>
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  DispatcherFromFileList(std::string const& input_file, TaskQueue& tq);
  void operator()();
};

}}}} // glite::wms::manager::server

#endif
