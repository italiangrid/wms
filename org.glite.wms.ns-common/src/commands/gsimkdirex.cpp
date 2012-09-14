/**
 * File: gsimkdirex.cpp
 * Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
 * Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
 * Copyright (c) 2003 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */
// $Id$

#include "glite/wms/common/utilities/globus_ftp_utils.h"
#include "glite/wms/common/logger/edglog.h"

namespace utilities     = glite::wms::common::utilities;
namespace logger        = glite::wms::common::logger::threadsafe;

using namespace std;
//using namespace utilities;

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

bool gsimkdirex(const string& destination, const string& stub)
{
  std::string dst(destination);
  size_t last_pos  = 0;
  size_t start_pos = 0;
  size_t next_pos = 0;
  size_t slash_count = 0;
  size_t slashes_to_ignore;
  bool result = true;

  // First of all to create the full destination directory.
  // If this fails, we fall back to creating the path.
  // This is a shortcut asked by LCG in order to reduce the
  // number of (expensive) gridftp client calls.

  logger::edglog << "Globus mkdir - first attempt: " << dst << " ...";
  if( utilities::globus::mkdir(string("gsiftp://") + dst) ) {

    logger::edglog << "Succeded." << std::endl;

    result = true;
    return(result);
  }

  logger::edglog << "Failed." << std::endl;

  // First count all the slashes in the filename.
  while( (last_pos = dst.find('/',start_pos)) != std::string::npos ) {
	  slash_count++;
	  start_pos = last_pos+1;
  }

  // As the 'exists' and 'mkdir' calls are quite slow, we (somewhat
  // artificially) limit ourselves to the last three path elements.

  slashes_to_ignore = slash_count - 3;
  start_pos = 0;
  slash_count = 0;
  size_t stub_pos = string::npos;  
  if(!stub.empty() && (stub_pos = dst.find(stub))!=string::npos) { slashes_to_ignore = 0; start_pos = stub_pos + stub.length() - 1; } 
  
  while( (last_pos = dst.find('/',start_pos)) != std::string::npos ) {

	  slash_count++;
	  start_pos = last_pos+1;
	  if (slash_count <= slashes_to_ignore) continue;

	  next_pos = dst.find('/',start_pos);
          std::string parent;
	  if (next_pos == std::string::npos) {
		  parent = dst;
	  } else {
		  parent = dst.substr(0, next_pos);
	  }
	  
//          if (!utilities::globus::exists(string("gsiftp://") + parent)) {
// -exists- check removed at the request of LCG. Now relying only on the
// return string check that is done inside the 'mkdir' call.
	    
	    logger::edglog << "Globus mkdir: " << parent << " ...";
	    if( !utilities::globus::mkdir(string("gsiftp://") + parent) ) {

	      logger::edglog << "Failed." << std::endl;

	      result = false;
	      break;
	    }
	    logger::edglog << "Succeded." << std::endl;
//          }
//          else {
#ifdef DEBUG
//	      logger::edglog << "[globus_ftp_client] gsiftp://" << parent << " already exists" << std::endl;
#endif
//          }

  }
  return result;
}

} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite
