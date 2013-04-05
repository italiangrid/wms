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

#include "IceCommandFactory.h"
#include "IceAbstractCommand.h"
#include "IceCommandSubmit.h"
#include "iceCommandCancel.h"
#include "classad_distribution.h"
#include "boost/algorithm/string.hpp"
#include "boost/scoped_ptr.hpp"
#include "ice/IceCore.h"
#include "iceUtils/Request.h"

using namespace std;

namespace glite {
  namespace wms {
    namespace ice {
      
      IceAbstractCommand* 
      IceCommandFactory::mkCommand( util::Request* request,
				    const util::CreamJob& aJob,
				    const std::string& cmdtype )
	throw(util::ClassadSyntax_ex&, util::JobRequest_ex&) 
      {

	IceAbstractCommand* result = 0;
	  
	  if ( boost::algorithm::iequals( cmdtype, "submit" ) ) {
	    result = new IceCommandSubmit( request, aJob );
	  } else if ( boost::algorithm::iequals( cmdtype, "cancel" ) ) {
	    result = new iceCommandCancel( request );
	  } else {
	    throw util::JobRequest_ex( "Unknown command [" + cmdtype + "] in request classad" );
	  }
	  
	return result;
      }
      
    } // namespace ice
  } // namespace wms
} // namespace glite
