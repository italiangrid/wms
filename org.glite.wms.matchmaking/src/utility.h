/*
 * File: utility.h
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

#ifndef _GLITE_WMS_MATCHMAKING_UTILITY_H_
#define _GLITE_WMS_MATCHMAKING_UTILITY_H_

#include <vector>
#include <numeric>
#include <algorithm>
#include "glite/wms/matchmaking/matchmaker.h"

using namespace std;

namespace glite {
namespace wms {
namespace matchmaking {

typedef std::vector< 
  std::pair<
    matchtable::key_type,
    matchtable::mapped_type
  > 
> matchvector;
	
struct rank_less_than_comparator : 
  binary_function< 
    matchtable::value_type&, 
    matchtable::value_type&, 
    bool
  >
{ 
  bool operator()(
    const matchtable::value_type& a, 
    const matchtable::value_type& b)
  {
    return getRank(a.second) < getRank(b.second);
  }
};

struct rank_greater_than_comparator :
  binary_function<
    matchtable::value_type&,
    matchtable::value_type&, 
    bool
  >
{
  bool operator()(
    const matchtable::value_type& a, 
    const matchtable::value_type& b)
  {
    return getRank(a.second) > getRank(b.second);
  }
};

}; // namespace matchmaking
}; // namespace wms
}; // namespace glite

#endif
