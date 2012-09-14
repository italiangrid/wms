// File: RBSelectionSchema.cpp
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$ 

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

namespace 
{
  unsigned int f_selection_map_instance_count;	
  boost::mutex RBSelectionSchemaMap::m_map_access_mutex;
  // leave as pointer to std::map
  std::map<std::string, RBSelectionSchemaPtr >* 
  RBSelectionSchemaMap::m_selection_schema_map;
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
