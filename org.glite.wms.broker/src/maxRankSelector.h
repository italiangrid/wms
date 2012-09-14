/*
 * File: maxRankSelector.h
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */
 
#ifndef GLITE_WMS_BROKER_MAXRANKSELECTOR_H_
#define GLITE_WMS_BROKER_MAXRANKSELECTOR_H_

#include "RBSelectionSchema.h"

namespace glite {
namespace wms {
namespace broker {

struct maxRankSelector : RBSelectionSchema
{
  typedef std::vector<
    matchtable::const_iterator
  > match_container_type;
  typedef std::map<
    double, 
    match_container_type
  > rank_to_match_container_map_type;
  
  maxRankSelector::maxRankSelector();
  maxRankSelector::~maxRankSelector();	
  matchtable::const_iterator 
  selectBestCE(matchtable const& match_table);
};	

}; // namespace broker
}; // namespace wms
}; // namespace glite

#endif
