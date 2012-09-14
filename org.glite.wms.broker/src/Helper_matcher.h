// File: Helper.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#ifndef GLITE_WMS_BROKER_HELPER_MATCHER_H
#define GLITE_WMS_BROKER_HELPER_MATCHER_H

#include "glite/wms/helper/HelperImpl.h"

namespace glite {
namespace wms {
namespace broker {
namespace helper {
namespace matcher {
class NoCompatibleCEs;
class Helper: public glite::wms::helper::HelperImpl
{
public:

  std::string id() const;
  std::string output_file_suffix() const;
  classad::ClassAd* resolve(classad::ClassAd const* input_ad) const;
};

}}}}} // glite::wms::helper::matcher

#endif
