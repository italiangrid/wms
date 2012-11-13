// File: WMReal.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#ifndef GLITE_WMS_MANAGER_SERVER_WMREAL_H
#define GLITE_WMS_MANAGER_SERVER_WMREAL_H

#include <boost/shared_ptr.hpp>
#include "lb_utils.h"

namespace classad {
class ClassAd;
}

namespace glite {

namespace wmsutils {
namespace jobid {
class JobId;
}}

namespace wms {
namespace manager {
namespace server {
	
class CannotCreateWM : std::exception
{
public:
  CannotCreateWM(std::string const& url);
  ~CannotCreateWM() throw();

  char const* what() const throw();

private:
  std::string m_message;
};

class WMReal
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  WMReal();
  void submit(classad::ClassAd const& jdl, ContextPtr context);
  void cancel(
    glite::wmsutils::jobid::JobId const& request_id,
    ContextPtr context
  );
};

}}}} // glite::wms::manager::server

#endif
