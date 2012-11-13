// File: RBSelectionSchema.cpp
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

// $Id: RBSelectionSchema.cpp,v 1.1.2.2 2012/09/12 10:02:12 mcecchi Exp $ 

#include "RBSelectionSchema.h"
#include "maxRankSelector.h"
#include "stochasticRankSelector.h"

#include <string>
#include <map>

using namespace std;

namespace glite {
namespace wms {
namespace broker {

typedef boost::shared_ptr<RBSelectionSchema> RBSelectionSchemaPtr;

  boost::mutex RBSelectionSchemaMap::m_map_access_mutex;
  double  RBSelectionSchema::FuzzyFactor = 1;

  // leave as pointer to std::map
  std::map<std::string, RBSelectionSchemaPtr >* 
  RBSelectionSchemaMap::m_selection_schema_map;

namespace 
{
  unsigned int f_selection_map_instance_count;
}
  
RBSelectionSchemaMap::RBSelectionSchemaMap()
{
  boost::mutex::scoped_lock lock(m_map_access_mutex);
  if( !f_selection_map_instance_count++ ) {   
    // register built-in schemas
  m_selection_schema_map = new std::map<std::string, RBSelectionSchemaPtr>(); 
      (*m_selection_schema_map)["maxRankSelector"] = 
        boost::shared_ptr<RBSelectionSchema>(new maxRankSelector());
      (*m_selection_schema_map)["stochasticRankSelector"] = 
        boost::shared_ptr<RBSelectionSchema>(new stochasticRankSelector());
    }
  }
 
RBSelectionSchemaMap::~RBSelectionSchemaMap()
{
  boost::mutex::scoped_lock lock(m_map_access_mutex);
  if( !--f_selection_map_instance_count ) {
    delete m_selection_schema_map;
  }
}

bool 
RBSelectionSchemaMap::registerSchema(
  const std::string& name, 
  RBSelectionSchemaPtr schema
)
{
  boost::mutex::scoped_lock lock(m_map_access_mutex);
  std::map<std::string, RBSelectionSchemaPtr>::iterator it(
    m_selection_schema_map->find(name)
  );
  if( it != m_selection_schema_map->end() ) return false; 
  (*m_selection_schema_map)[name] = schema;
  return true;  
}

RBSelectionSchemaPtr 
RBSelectionSchemaMap::unregisterSchema(const std::string& name) 
{
  boost::mutex::scoped_lock lock(m_map_access_mutex);
  std::map<std::string, RBSelectionSchemaPtr>::iterator it(
    m_selection_schema_map->find(name)
  );
  if( it == m_selection_schema_map->end() ) {
    return RBSelectionSchemaPtr();
  }
  RBSelectionSchemaPtr schema = it->second;
  m_selection_schema_map->erase(it); 
  return schema;  
}

RBSelectionSchemaPtr 
RBSelectionSchemaMap::getSchema(const std::string& name)
{
 boost::mutex::scoped_lock lock(m_map_access_mutex);
 std::map<std::string, RBSelectionSchemaPtr>::const_iterator it(
   m_selection_schema_map->find(name)
 );
 if( it == m_selection_schema_map->end() ) return RBSelectionSchemaPtr();
 return it->second;
}

} // namespace broker
} // namespace wms
} // namespace glite
