/*
 * File: ResourceBroker.h
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */
 
#ifndef _GLITE_WMS_BROKER_RESOURCEBROKER_H_
#define _GLITE_WMS_BROKER_RESOURCEBROKER_H_

#include "matchmaking.h"
#include "brokerinfo.h"

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>

namespace classad
{
  class ClassAd;
}

namespace glite {
namespace wms {
namespace broker {

class ResourceBroker : boost::noncopyable
{
  class impl;
  boost::shared_ptr<impl> m_impl;

public:

  typedef boost::function<
    matchtable::iterator(matchtable&)
  > selector;
 
  typedef boost::function< 
    boost::tuple<
      boost::shared_ptr<matchtable>,
      boost::shared_ptr<filemapping>,
      boost::shared_ptr<storagemapping>
    >(const classad::ClassAd*)
  > strategy;

  ResourceBroker();

  void changeSelector(selector);
  void changeStrategy(strategy);

  matchtable::iterator
  selectBestCE(matchtable&);
  
  boost::tuple<
    boost::shared_ptr<matchtable>,
    boost::shared_ptr<filemapping>,
    boost::shared_ptr<storagemapping>
  >
  findSuitableCEs(const classad::ClassAd*);
};

}}} 

#endif
