/*
 * File: RBSelectionSchema.h
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$

# ifndef GLITE_WMS_BROKER_RBSELECTIONSCHEMA_H_
# define GLITE_WMS_BROKER_RBSELECTIONSCHEMA_H_

# include <boost/utility.hpp>
# include <boost/thread/mutex.hpp>
# include <boost/shared_ptr.hpp>
# include <map>
# include <vector>
# include "matchmaking.h"

namespace glite {
namespace wms {		
namespace broker {
		
struct RBSelectionSchema 
{
   virtual ~RBSelectionSchema() {};
   virtual matchtable::const_iterator 
   selectBestCE(matchtable const& match_table) = 0;
};

typedef boost::shared_ptr<RBSelectionSchema> RBSelectionSchemaPtr;
	
class RBSelectionSchemaMap : boost::noncopyable
{
 public:
  RBSelectionSchemaMap();
  ~RBSelectionSchemaMap();
  static RBSelectionSchemaPtr getSchema(const std::string& name);
  static bool registerSchema(const std::string& name, RBSelectionSchemaPtr);
  static RBSelectionSchemaPtr unregisterSchema(const std::string& name); 

 private:
  static boost::mutex m_map_access_mutex;
  static std::map<std::string, RBSelectionSchemaPtr>* m_selection_schema_map;
};

namespace {
  static RBSelectionSchemaMap f_SelectionSchemaMapInit;
}

}; // namespace broker
}; // namespace wms
}; // namespace glite

#endif
