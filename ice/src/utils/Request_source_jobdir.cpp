/* LICENSE:
Copyright (c) Members of the EGEE Collaboration. 2010. 
See http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

   http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. 
See the License for the specific language governing permissions and 
limitations under the License.

END LICENSE */


#include "Request_source_jobdir.h"
#include "Request_jobdir.h"
#include "glite/wms/common/utilities/scope_guard.h"
//#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include <vector>
#include <iostream>
#include <boost/tuple/tuple.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "utils/logging.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

//#include "glite/ce/cream-client-api-c/creamApiLogger.h"

namespace utilities=glite::wms::common::utilities;
namespace fs=boost::filesystem;
//namespace api_util  = glite::ce::cream_client_api::util;
using namespace glite::wms::ice::util;
using namespace std;

//__________________________________________________________________________________________
Request_source_jobdir::Request_source_jobdir( const std::string& jdir_name, bool create ) :
    Request_source( jdir_name ),
    m_jobdir( 0 )
{
    //static const char* method_name = "Request_source_jobdir::CTOR() - ";
    edglog_fn("Request_source_jobdir::Request_source_jobdir");
    try {
        if ( create ) {
            // Tries to create the directory structure
            utilities::JobDir::create( jdir_name );
        }
        m_jobdir = new utilities::JobDir( jdir_name );
    } catch( std::exception& ex ) {
        //CREAM_SAFE_LOG(api_util::creamApiLogger::instance()->getLogger()->fatalStream() 
                       //<< method_name << 
		    edglog(fatal) <<   "Error creating or opening path ["
                       << jdir_name << "]: " << ex.what() <<endl;
                       //);
        abort();
    } catch( ... ) {
        //CREAM_SAFE_LOG(api_util::creamApiLogger::instance()->getLogger()->fatalStream() 
                       //<< method_name 
		     edglog(fatal)   << "Error creating or opening path ["
                       << jdir_name << "]: Catched unknown exception" <<endl;
                       //);
        abort();
    }
}

//__________________________________________________________________________________________
Request_source_jobdir::~Request_source_jobdir( )
{
    delete m_jobdir; // FIXME: use a scoped_ptr instead?
}

//__________________________________________________________________________________________
list<Request*> Request_source_jobdir::get_requests( size_t max_size  )
{
    list< Request* > result;
    
    utilities::JobDir::iterator b, e;
    
    edglog_fn("Request_source_jobdir::get_requests");
    
    try {
      boost::tie(b, e) = m_jobdir->new_entries();
    } catch(exception& ex) {
      //CREAM_SAFE_LOG(api_util::creamApiLogger::instance()->getLogger()->errorStream()
                        edglog(fatal)  << "Request_source_jobdir::get_requests() - Error returned by method jobDir::new_entries(): "
                         << ex.what() <<endl;
        //                 );
    }
    for ( ; b != e && result.size() < max_size; ++b) {
        try {
	
          fs::path const& new_file = *b;
          fs::path const old_file = m_jobdir->set_old(new_file);
	  result.push_back( new Request_jobdir( old_file ) );
	  
    	} catch( exception& ex ) {
	  //CREAM_SAFE_LOG(api_util::creamApiLogger::instance()->getLogger()->fatalStream() 
                        edglog(fatal)  << "Request_source_jobdir::get_requests() - Error instantiating boost::path: "
                         << ex.what() <<endl;
            //             );
	}
        //result.push_back( new Request_jobdir( old_file ) );
    }
    
    return result;
}

//__________________________________________________________________________________________
void Request_source_jobdir::remove_request( Request* req )
{
    edglog_fn("Request_source_jobdir::remove_request");
    
    Request_jobdir* req_jobdir = dynamic_cast< Request_jobdir* >( req );
    if ( 0 != req_jobdir ) {
      try {
        fs::remove( req_jobdir->get_path() ); // FIXME? does it throw something?
      } catch( exception& ex ) {
	  //CREAM_SAFE_LOG(api_util::creamApiLogger::instance()->getLogger()->fatalStream() 
                        edglog(fatal) << "Request_source_jobdir::remove_request() - Error removing path ["
			 << req_jobdir->get_path().string() << "]: "
                         << ex.what() <<endl;
            //             );
      }
    }
}

//__________________________________________________________________________________________
void Request_source_jobdir::put_request( const string& ad )
{
    m_jobdir->deliver( ad );
}

//__________________________________________________________________________________________
size_t Request_source_jobdir::get_size( void )
{
    std::pair< utilities::JobDir::iterator, utilities::JobDir::iterator > entries = m_jobdir->new_entries();
    return entries.second - entries.first;
}
