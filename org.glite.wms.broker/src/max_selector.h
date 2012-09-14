/*
 * File: maxRankSelector.h
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */
 
#ifndef GLITE_WMS_BROKER_MAX_SELECTOR_H
#define GLITE_WMS_BROKER_MAX_SELECTOR_H

#include "glite/wms/broker/match.h"

namespace glite {
namespace wms {
namespace broker {

MatchTable::const_iterator max_selector(MatchTable const& matches);

}}}

#endif
