// File: RBSimpleISMImpl.cpp
// Author: Salvatore Monforte
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <functional>
#include <vector> 

#include <classad_distribution.h>

#include "matchmaking.h"
#include "simple_strategy.h"

namespace glite {
namespace wms {
namespace broker {

boost::tuple<
  boost::shared_ptr<matchtable>,
  boost::shared_ptr<filemapping>,
  boost::shared_ptr<storagemapping>
>
simple(
  classad::ClassAd const* requestAd
)
{
  if (!requestAd) {
    return boost::tuples::make_tuple(
      boost::shared_ptr<matchtable>(),
      boost::shared_ptr<filemapping>(),
      boost::shared_ptr<storagemapping>()
    );
  }

  classad::ClassAd jdl(*requestAd);

  boost::shared_ptr<matchtable> suitableCEs(
    new matchtable
  );

  match(jdl, *suitableCEs);

  return boost::tuples::make_tuple(
    suitableCEs,
    boost::shared_ptr<filemapping>(),
    boost::shared_ptr<storagemapping>()
  );
}

}}}
