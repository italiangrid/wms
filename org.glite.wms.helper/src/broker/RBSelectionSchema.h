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
 * File: RBSelectionSchema.h
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 */

// $Id$

# ifndef GLITE_WMS_BROKER_SELECTORS_RBSELECTIONSCHEMA_H_
# define GLITE_WMS_BROKER_SELECTORS_RBSELECTIONSCHEMA_H_

# include <boost/utility.hpp>
# include <boost/thread/mutex.hpp>
# include <boost/shared_ptr.hpp>
# include <map>
# include <vector>
# include <matchmaker.h>

namespace mm = glite::wms::matchmaking;

namespace glite {
namespace wms {		
namespace broker {
		
struct RBSelectionSchema 
{
   virtual ~RBSelectionSchema() {};
   virtual mm::matchtable::const_iterator
   selectBestCE(mm::matchtable const& match_table) = 0;
   static double FuzzyFactor;
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

} // namespace broker
} // namespace wms
} // namespace glite

#endif
