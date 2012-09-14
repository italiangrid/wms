// File: RequestHandler.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#ifndef GLITE_WMS_MANAGER_SERVER_REQUESTHANDLER_H
#define GLITE_WMS_MANAGER_SERVER_REQUESTHANDLER_H

#include <boost/shared_ptr.hpp>
#include "pipedefs.h"

namespace glite {
namespace wms {
namespace manager {
namespace server {

class RequestHandler:
    public glite::wms::common::task::PipeReader<RequestPtr, queue_type>
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  RequestHandler();
  void operator()();
};


}}}} // glite::wms::manager::server

#endif // GLITE_WMS_MANAGER_REQUESTHANDLER_H
