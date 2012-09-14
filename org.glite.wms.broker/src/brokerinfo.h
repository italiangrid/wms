/*
 * File: BrokerInfo.h
 * Author: Monforte Salvatore
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

#ifndef _GLITE_WMS_BROKER_BROKERINFO_H_
#define _GLITE_WMS_BROKER_BROKERINFO_H_

#include "glite/wms/broker/match.h"

namespace classad
{
  class ClassAd;
}

namespace glite {
namespace wms {
namespace broker {

classad::ClassAd*
make_brokerinfo_ad(
  FileMapping const& fm,
  StorageMapping const& sm,
  classad::ClassAd const& ce_ad
);

}}}

#endif
