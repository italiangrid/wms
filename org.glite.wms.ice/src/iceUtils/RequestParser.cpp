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

#include "RequestParser.h"
#include "ClassadSyntax_ex.h"
#include "JobRequest_ex.h"
#include "Request.h"

#include "boost/format.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/convenience.hpp"
#include "boost/thread/recursive_mutex.hpp"
#include "boost/filesystem/path.hpp"

using namespace glite::wms::ice;

//______________________________________________________________________
void util::RequestParser::unparse_request( void )
  throw(ClassadSyntax_ex&, JobRequest_ex&)
{
  //string protocolStr;
  string jdl, adl;
  {// Classad-mutex protected region  
    boost::recursive_mutex::scoped_lock M_classad( glite::wms::ice::util::CreamJob::s_classad_mutex );
	        
    classad::ClassAdParser parser;
    classad::ClassAd *rootAD = parser.ParseClassAd( m_request->to_string() );
	    
    if (!rootAD) {
      throw ClassadSyntax_ex( boost::str( boost::format( "RequestParser::unparse_request: ClassAd parser returned a NULL pointer parsing request: %1%" ) % m_request->to_string() ) );        
    }
	    
    boost::scoped_ptr< classad::ClassAd > classad_safe_ptr( rootAD );
	    
    // Parse the "command" attribute
    if ( !classad_safe_ptr->EvaluateAttrString( "command", m_command ) ) {
      throw JobRequest_ex( boost::str( boost::format( "RequestParser::unparse_request: attribute 'command' not found or is not a string in request: %1%") % m_request->to_string() ) );
    }
    boost::trim_if( m_command, boost::is_any_of("\"") );
	    
    if ( !boost::algorithm::iequals( m_command, "submit" ) 
         && !boost::algorithm::iequals( m_command, "cancel" )
	 && !boost::algorithm::iequals( m_command, "reschedule" )
	 ) 
    {
      throw JobRequest_ex( boost::str( boost::format( "RequestParser::unparse_request: wrong command parsed: %1%" ) % m_command ) );
    }
	    
    if( boost::algorithm::iequals( m_command, "cancel" ) )
      return;
 
    // Parse the "version" attribute
    if ( !classad_safe_ptr->EvaluateAttrString( "Protocol", m_protocol ) ) {
      throw JobRequest_ex("attribute \"Protocol\" not found or is not a string");
    }
    // Check if the version is exactly 1.0.0
    if ( m_protocol.compare("1.0.0") ) {
      throw JobRequest_ex("Wrong \"Protocol\" for jobRequest: expected 1.0.0, got " + m_protocol );
    }
	    
    classad::ClassAd *argumentsAD = 0; // no need to free this
    // Parse the "arguments" attribute
    if ( !classad_safe_ptr->EvaluateAttrClassAd( "arguments", argumentsAD ) ) {
      throw JobRequest_ex("attribute 'arguments' not found or is not a classad");
    }

    classad::ClassAd *adAD = 0; // no need to free this
    if( boost::algorithm::iequals( m_command, "submit" ) || boost::algorithm::iequals( m_command, "reschedule" ) ) {
      // Look for "JobAd" attribute inside "arguments"
      if ( !argumentsAD->EvaluateAttrClassAd( "jobad", adAD ) && ! argumentsAD->EvaluateAttrClassAd( "adlad", adAD )) {
	throw JobRequest_ex("Attribute \"JobAd\" nor \"AdlAd\" not found inside 'arguments', or is not a classad" );
      }
    }
	    
    // initializes the m_jdl attribute



    classad::ClassAdUnParser unparser;
    unparser.Unparse( jdl, argumentsAD->Lookup( "jobad" ) );
    unparser.Unparse( adl, argumentsAD->Lookup( "adlad" ) );
  } // end classad-mutex protected regions

//   cout << "\n\njobad=["<<jdl<<"]\n\nadlad=["<<adl<<"]" << endl<<endl;
//   exit(1);
	  
  try {
    m_job.set_jdl( jdl, m_command ); // this puts another mutex
    m_job.set_status( glite::ce::cream_client_api::job_statuses::UNKNOWN );
  } catch( ClassadSyntax_ex& ex ) {
    throw( ClassadSyntax_ex( ex.what() ) );
  }
} // unparse_request 
