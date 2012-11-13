/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/*
 * File: ResourceBroker.h
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 */
 
#ifndef _GLITE_WMS_BROKER_RESOURCEBROKER_H_
#define _GLITE_WMS_BROKER_RESOURCEBROKER_H_

#include <boost/utility.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#include "matchmaker.h"
#include "brokerinfo.h"
#include "RBSelectionSchema.h"	

namespace mm = glite::wms::matchmaking;
namespace bi = glite::wms::brokerinfo;

namespace classad
{
  class ClassAd;
}

namespace glite {
namespace wms {
namespace broker {

class ResourceBroker : boost::noncopyable
{
public: 
  class Impl;

  ResourceBroker();
  void changeImplementation(boost::shared_ptr<Impl>);
  void changeSelector(const std::string&); 
  
  mm::matchtable::const_iterator 
  selectBestCE(mm::matchtable const&); 
  
  boost::tuple<
    boost::shared_ptr<mm::matchtable>,
    boost::shared_ptr<bi::FileMapping>,
    boost::shared_ptr<bi::StorageMapping>
  >
  findSuitableCEs(const classad::ClassAd*);
  
private:
  boost::shared_ptr<Impl> m_impl;
  boost::weak_ptr<RBSelectionSchema> m_selection_schema;
};

struct ResourceBroker::Impl : boost::noncopyable
{
  virtual 
  boost::tuple<
    boost::shared_ptr<mm::matchtable>,
    boost::shared_ptr<bi::FileMapping>,
    boost::shared_ptr<bi::StorageMapping>
  >
  findSuitableCEs(const classad::ClassAd*) = 0;
  virtual ~Impl() {};
};

} // namespace broker
} // namespace wms
} // namespace glite

#endif
