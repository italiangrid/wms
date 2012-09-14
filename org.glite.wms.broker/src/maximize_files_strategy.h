// File: RBMaximizeFilesISMImpl.h
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#ifndef GLITE_WMS_BROKER_MAXIMIZE_FILES_H
#define GLITE_WMS_BROKER_MAXIMIZE_FILES_H

#include "storage_utils.h"

namespace glite {
namespace wms {
namespace broker {

boost::tuple<
  boost::shared_ptr<matchtable>,
  boost::shared_ptr<filemapping>,
  boost::shared_ptr<storagemapping>
>
maximize_files(const classad::ClassAd* requestAd);

}}}

#endif
