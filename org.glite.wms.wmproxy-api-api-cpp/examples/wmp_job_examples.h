/*
*	Copyright (c) Members of the EGEE Collaboration. 2004.
*	See http://www.eu-egee.org/partners/ for details on the
*	copyright holders.
*
*	Licensed under the Apache License, Version 2.0 (the "License");
*	you may not use this file except in compliance with the License.
*	You may obtain a copy of the License at
*	 
*	     http://www.apache.org/licenses/LICENSE-2.0
*	 
*	Unless required by applicable law or agreed to in writing, software
*	distributed under the License is distributed on an "AS IS" BASIS,
*	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
*	either express or implied.
*	See the License for the specific language governing permissions and
*	limitations under the License.
*/

#ifndef GLITE_WMS_WMPROXYAPICPP_WMP_JOB_EXAMPLES_H
#define GLITE_WMS_WMPROXYAPICPP_WMP_JOB_EXAMPLES_H

#include "glite/wms/wmproxyapi/wmproxy_api.h"


// Exceptions
#include "glite/wmsutils/exception/Exception.h"

#include <iostream>
#include <vector>

extern "C" {
#include <getopt.h>
}
#include <fstream>
#include <sstream>

namespace glite {
namespace wms {
namespace wmproxyapi {
namespace examples {
namespace utilities {

// environment variable for the wmproxy service URL
#define GLITE_WMPROXY_ENDPOINT 	"GLITE_WMPROXY_ENDPOINT"

/*
*	removes the white spaces at the beginning and
*	at the end of the input string
*	@param str input string
*/
const char* clean(char *str);
/*
*	reads the jobid from a file
*	@param path filepath
*	@return a pointer to the jobid string
*/
std::string* jobidFromFile (const std::string &path);
/*
*	gets a string with messages of the input exception
*	@param b_ex input BaseException
*	@return the error message of the exception
*/
const std::string handle_exception (const glite::wms::wmproxyapi::BaseException &b_ex );
/*
*	save the text in the input string buffer into a file
*	@param path filepath
*	@param bfr input string buffer
*	@return 0 in case of success; -1 in case of any error
*/
int saveToFile (const std::string &path, const std::string &bfr) ;

/*
*	contacts the endpoint configurated in the context
*	in order to retrieve the http(s) destionationURI of the job
*	identified by jobid
*	@param jobid the identifier of the job
*	@param cfs context
*	@return a string with the http(s) destinationURI('s) (or NULL in case of error)
*
*/
std::string* getInputSBDestURI(const std::string &jobid, const ConfigContext *cfs);
/*
*	performs user proxy delegation with del_id delegation identifier string
*	@param cfs context (proxy, endpoint, trusted certificates)
*	@param del_id identifier of the delegation operation
*	@param verbose verbosity
*	@return a string with the user proxy
*/
const std::string makeDelegation (ConfigContext *cfs, const std::string &del_id, const bool &verbose = false);
/*
*	peforms proxy delegation automatically generating the delegation identifier string
*	@param cfs context (proxy, endpoint, trusted certificates)
*	@param verbose verbosity
*/
std::string* makeAutomaticDelegation (ConfigContext *cfs, const bool &verbose = false);
/*
*	checks if the user free quota could support (in size) the transferring of a set of files
*	to the endpoint specified in the context
*	@param files the set of files to be transferred
*	@param cfs context (proxy, endpoint, trusted certificates)
*	@param verbose verbosity
*/
bool checkFreeQuota ( const std::vector<std::pair<std::string,std::string> > &files, ConfigContext *cfs, const bool &verbose = false );
/*
*	removes dir-separator characters at the end of the input path
*	if they are present
*	@param fpath the input pathname
*	@return the normalized pathname
*/
std::string normalize_path( const std::string &fpath );
/*
*	adds a wildcard at the end of the input pathname
*	@param path the input pathname
*	@param wc the wildcard
*	@return the pathame with the wildcard at the end
*
*/
std::string addWildCards2Path(std::string& path, const std::string &wc);



} //glite
}//wms
}//wmproxyapi
}//examples
}//utilities



#endif  // GLITE_WMS_WMPROXYAPICPP_WMP_JOB_EXAMPLES_H
