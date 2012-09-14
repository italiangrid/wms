// File: RBSimpleISMImpl.h
// Author: Salvatore Monforte
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#ifndef GLITE_WMS_BROKER_RBSIMPLEISMIMPL_H
#define GLITE_WMS_BROKER_RBSIMPLEISMIMPL_H

#include "ResourceBroker.h"

namespace glite {
namespace wms {
namespace broker {

struct RBSimpleISMImpl : ResourceBroker::Impl
{
  boost::tuple<
    boost::shared_ptr<matchtable>,
    boost::shared_ptr<filemapping>,
    boost::shared_ptr<storagemapping>
  > 
  findSuitableCEs(classad::ClassAd const*);
};

}}}

#endif
