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

#include "classad_distribution.h"

#include <string>
#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include <unistd.h>
#include <exception>
#include <fstream>

/* workaround for gsoap 2.7.13 */
#include "glite/ce/cream-client-api-c/cream_client_soapH.h"
SOAP_NMAC struct Namespace namespaces[] = {};

using namespace std;

int main(int argc, char* argv[]) 
{
  /**
   * Usage: putFL_cancel <filelist> <grid_job_id>
   *
   */
  if ( argc != 3 ) {
    cerr << "Usage: " << argv[0] << " <filelist> <gridjobid>" << endl;
    return -1;
  }
  
  string request = "[ protocol = \"1.0.0\"; command = \"cancel\"; arguments = [ jobid = \"" + string(argv[2]) + "\" ] ]";
  
 /* 
  cout << "Adding JDL <" 
       << request << "> to filelist..." << endl;
  {
    try{
      fl.open(argv[1]);
    }
    catch(std::exception& ex) {
      cerr << ex.what()<<endl;
      _exit(1);
    }
    FileListMutex mx(fl);
    FileListLock  lock(mx);
    try {
      fl.push_back(request);
    } catch(std::exception& ex) {
      cerr << ex.what()<<endl;
      _exit(1);
    }
  }*/
}
