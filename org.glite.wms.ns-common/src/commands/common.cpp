/*
 * common.cpp
 * 
 * Copyright (c) 2001, 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#include "Command.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "common.h"
#include "glite/lb/producer.h"

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"
#include "logging.h"

#include <vector>
#include <string>
#include <classad_distribution.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <fstream>
#include <iostream>
#include <boost/scoped_ptr.hpp>

namespace socket_pp = glite::wmsutils::tls::socket_pp; 
namespace utils = glite::wmsutils::classads; 
namespace logger    = glite::wms::common::logger;

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {
 
  bool stripProtocol (const char *source, char *dest) {
    char buff[512];
    
    strcpy(buff, source);

    if ( !strncmp(buff, "file://", 7) ) {
       strcpy(dest, buff + 7); 
    } else {
       strcpy(dest, buff); 
    }

    return true;
 }


 bool computeSandboxSize (Command* cmd) { 

   edglog_fn("CFCI::cSBSize"); 
   off_t total_size = 0;
   std::vector<std::string> files; 
 
   std::string jdl;
   if ( ! cmd -> getParam("jdl", jdl) ) {
     return false; 
   } 
   classad::ClassAdParser parser;
   boost::scoped_ptr<classad::ClassAd> jdlad(parser.ParseClassAd( jdl ));
   if (!(jdlad.get()) ) { 
     return false;
   } 

   if (!utils::EvaluateAttrListOrSingle(*jdlad.get(),"InputSandbox",files)) {
     // std::cout << "Read : " << files.size() << " files" << std::endl;
   } 
 
   if (!files.size()) {
     // std::cout << "Empty input sandbox." << std::endl;
   } 

   int i=0;

   for(std::vector<std::string>::const_iterator it=files.begin();it!=files.end();it++){
     int fd = -1; 

     char buffer[512];
     stripProtocol( (*it).c_str(), buffer );
     edglog(debug) << "InputSandbox file #" << ++i << ": " << buffer << std::endl;

     fd = open(buffer, O_RDONLY);
     if (fd != -1) {
       struct stat buf;
       if ( !fstat(fd, &buf) ) { 
	 total_size += buf.st_size;
       }
       close(fd);
     } else { 
       // File not Found: SandboxIOException
       // never Thrown
       edglog(fatal) << "File " << buffer << " not found." << std::endl; 
       return false;
     } 
   } 
 
   // std::cout << "Size: " << total_size << std::endl;
   cmd -> setParam("SandboxSize", (double)total_size );
   return true; 
 } 

  bool checkSpace(Command* cmd)
  { 
    // Retrieves the needed free space amount
    double needed_space; 
    if ( cmd -> getParam("SandboxSize", needed_space) ) {
      if ( needed_space == 0) {
	return ( cmd -> setParam("CheckPassed", true) );
      } else {
	struct statfs buf; 
	if ( !statfs(".", &buf) ){
	  if (((unsigned long)needed_space) <= (long long)buf.f_bsize * buf.f_bfree) {
	    return ( cmd -> setParam("CheckPassed", true) );
	  } else {
	    // Log Job Refused due to lack of space
	    int i = 0;
	    bool logged = false;
	    
	    for (; i < 3 && !logged ; i++ ) {
#ifdef GLITE_WMS_HAVE_LBPROXY
	      logged = !edg_wll_LogRefusedProxy (*(cmd->getLogContext()), 
						 EDG_WLL_SOURCE_USER_INTERFACE, 
						 "string_host",
						 "", 
						 "Lack of Space to transfer InputSandbox.");
#else
	      logged = !edg_wll_LogRefused (*(cmd->getLogContext()), 
					     EDG_WLL_SOURCE_USER_INTERFACE, 
					     "string_host",
					     "", 
					     "Lack of Space to transfer InputSandbox.");
#endif
	      if (!logged && (i<2)) {
		// edglog(info) << "Failed to log Refused Job due to lack of space." << std::endl;
		std::cout << "Failed to log Refused Job due to lack of space." << std::endl; 
		sleep(60);
	      }
	    }
	    
	    if (!logged){
	      // edglog(info) << "Error while logging Refused Job due to lack of space." << std::endl;
	      std::cout << "Error while logging Refused Job due to lack of space." << std::endl; 
	    }
          }
	}  
      }
    } 
    return false;
  } 

  void replace(std::string& where, const std::string& what, const std::string& with) { 
    while(where.find(what)!=std::string::npos) {
	where.replace(where.find(what),what.length(),with); 
    }
  }

  bool fcopy(const std::string& from, const std::string& to)
  {
    std::ifstream in ( from.c_str() );
    
    if( !in.good() ) {
      return false;  
    }
    std::ofstream out( to.c_str() );
    
    if( !out.good() ) {
      return false;
    }
    out << in.rdbuf(); // read original file into target
    
    struct stat from_stat;
    if(   stat(from.c_str(), &from_stat) ||
	  chown( to.c_str(), from_stat.st_uid, from_stat.st_gid ) ||
	  chmod( to.c_str(), from_stat.st_mode ) ) return false;
    
    return true;
  }
  


} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite 


