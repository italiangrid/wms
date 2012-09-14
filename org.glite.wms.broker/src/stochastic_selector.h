/*
 * File: stochasticRankSelector.h
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

#ifndef GLITE_WMS_BROKER_STOCHASTIC_SELECTOR_H_
#define GLITE_WMS_BROKER_STOCHASTIC_SELECTOR_H_

#include "matchmaking.h"

namespace glite {
namespace wms {
namespace broker {

class stochastic_selector
{
  double m_fuzzy_factor;
public:
  stochastic_selector(double = 1.0);

  matchtable::iterator
  operator()(matchtable&);
};

}; // namespace broker
}; // namespace wms
}; // namespace glite

#endif
