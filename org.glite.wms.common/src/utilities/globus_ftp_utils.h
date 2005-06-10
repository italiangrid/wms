// File: globus_utils.h
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html
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
