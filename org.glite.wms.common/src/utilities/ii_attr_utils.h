/*
Copyright (c) Members of the EGEE Collaboration. 2004. 
See http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

    http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
See the License for the specific language governing permissions and 
limitations under the License.
*/
// File: ii_attr_utils.h
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2002 EU DataGrid.
// $Id: ii_attr_utils.h,v 1.3.36.1 2010/04/08 12:49:02 mcecchi Exp $

#ifndef _GLITE_WMS_COMMON_UTILITIES_II_ATTR_UTILS_
#define _GLITE_WMS_COMMON_UTILITIES_II_ATTR_UTILS_

#include <string>
#include <vector>

#include <boost/utility.hpp>

namespace glite {
namespace wms {
namespace common {
namespace utilities {
namespace ii_attributes {

typedef std::string type;	
typedef std::vector<type> container_type;
typedef container_type::const_iterator const_iterator;
typedef const_iterator iterator;

extern std::pair<const_iterator, const_iterator> multiValued();
extern bool isGlueSchema();

} // namespace ii_attributes
} // namespace utilities
} // namespace common
} // namespace wms
} // namespace glite

#endif


