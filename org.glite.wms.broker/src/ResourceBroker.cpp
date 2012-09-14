/*
 * File: ResourceBroker.h
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */
 
#include "ResourceBroker.h"
#include "simple_strategy.h"
#include "max_selector.h"

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_smallint.hpp>
#include <boost/random/variate_generator.hpp>
#include <algorithm>

namespace glite {
namespace wms {
namespace broker {

struct ResourceBroker::impl : boost::noncopyable
{
  ResourceBroker::strategy _strategy;
  ResourceBroker::selector _selector;
  impl(
    ResourceBroker::strategy strategy, 
    ResourceBroker::selector selector
  ) : _strategy(strategy), _selector(selector)
  {
  }

  matchtable::iterator
  selectBestCE(matchtable& matches) 
  {
    return _selector(matches);  
  }
  
  boost::tuple<
    boost::shared_ptr<matchtable>,
    boost::shared_ptr<filemapping>,
    boost::shared_ptr<storagemapping>
  >
  findSuitableCEs(const classad::ClassAd* requestAd)
  {
    return _strategy(requestAd);
  }
};

ResourceBroker::ResourceBroker() :
  m_impl( new ResourceBroker::impl(simple, max_selector) )
{
} 

void 
ResourceBroker::changeStrategy(strategy s) 
{ 
  m_impl->_strategy = s;
}

void
ResourceBroker::changeSelector(selector s)
{
  m_impl->_selector = s;
}
  
matchtable::iterator 
ResourceBroker::selectBestCE(matchtable& matches)
{
  return m_impl->selectBestCE(matches); 
}

boost::tuple<
  boost::shared_ptr<matchtable>,
  boost::shared_ptr<filemapping>,
  boost::shared_ptr<storagemapping>
> 
ResourceBroker::findSuitableCEs(const classad::ClassAd* requestAd)
{ 
  return m_impl->findSuitableCEs(requestAd); 
}

}; // namespace broker
}; // namespace wms
}; // namespace glite
