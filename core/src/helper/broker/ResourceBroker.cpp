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

#include "ResourceBroker.h"
#include "RBSimpleISMImpl.h"

namespace glite {
namespace wms {
namespace broker {

ResourceBroker::ResourceBroker() 
{
  changeImplementation(
    boost::shared_ptr<Impl>(new RBSimpleISMImpl())
  ); 
  m_selection_schema = boost::weak_ptr<RBSelectionSchema>(
    RBSelectionSchemaMap::getSchema("maxRankSelector")
  );
} 

void 
ResourceBroker::changeImplementation(boost::shared_ptr<Impl> impl) 
{ 
  m_impl = impl; 
}
  
void
ResourceBroker::changeSelector(const std::string& name)
{
  boost::weak_ptr<RBSelectionSchema> s(
    RBSelectionSchemaMap::getSchema(name)
  );
  if(s.lock()) m_selection_schema = s;
}


matchmaking::matchtable::const_iterator 
ResourceBroker::selectBestCE(matchmaking::matchtable const& match_table)
{
  if(RBSelectionSchemaPtr schema = m_selection_schema.lock() ) {
    return schema -> selectBestCE( match_table );
  }
  return match_table.end();
}

boost::tuple<
  boost::shared_ptr<matchmaking::matchtable>,
  boost::shared_ptr<brokerinfo::FileMapping>,
  boost::shared_ptr<brokerinfo::StorageMapping>
> 
ResourceBroker::findSuitableCEs(const classad::ClassAd* requestAd)
{ 
  return m_impl -> findSuitableCEs( requestAd ); 
}

} // namespace broker
} // namespace wms
} // namespace glite

