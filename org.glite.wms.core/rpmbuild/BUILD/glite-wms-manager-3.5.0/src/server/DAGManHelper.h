// File: DAGManHelper.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2003 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#ifndef GLITE_WMS_MANAGER_SERVER_DAGMANHELPER_H
#define GLITE_WMS_MANAGER_SERVER_DAGMANHELPER_H

#include "glite/wms/helper/HelperImpl.h"
#include "glite/wms/helper/exceptions.h"

namespace classad {
class ClassAd;
}

namespace glite {
namespace wms {
namespace manager {
namespace server {

class DAGManHelperError: public helper::HelperError
{
public:
  DAGManHelperError()
    : helper::HelperError("DAGManHelper")
  {
  }
  char const* what() const throw()
  {
    return "DAGManHelperError";
  }
};

class DAGManHelper: public helper::HelperImpl
{
public:
  std::string id() const;
  std::string output_file_suffix() const;
  classad::ClassAd* resolve(classad::ClassAd const* input_ad) const;
};

}}}} // glite::manager::wms::server


#endif

// Local Variables:
// mode: c++
// End:
