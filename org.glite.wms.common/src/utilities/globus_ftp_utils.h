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
// File: globus_utils.h
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2002 EU DataGrid.
// 
// $Id$

#include <globus_ftp_client.h>
#include <string>

namespace glite {
namespace wms {
namespace common {
namespace utilities {
namespace globus {	
	
extern bool mkdir(const std::string& dst);
extern bool exists(const std::string& dst);
extern bool put  (const std::string& src, const std::string& dst);
extern bool get  (const std::string& src, const std::string& dst);

}
}
}
}
}
