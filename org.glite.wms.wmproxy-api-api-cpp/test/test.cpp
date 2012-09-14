// Copyright (c) Members of the EGEE Collaboration. 2004. 
// See http://www.eu-egee.org/partners/ for details on the copyright
// holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 

//     http://www.apache.org/licenses/LICENSE-2.0 

// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

#include "glite/wms/wmproxyapi/wmproxy_api.h"

using namespace std;
using namespace glite::wms::wmproxyapi;

const std::string errMsg (const glite::wms::wmproxyapi::BaseException &b_ex ){
        string meth = b_ex.methodName.c_str();
        string *errcode =b_ex.ErrorCode ;
        string *description = b_ex.Description ;
        string excmsg = "";
        //description
        if (description){ if (description->size() > 0 ){
                excmsg += *description+"\n" ; }
        }
        if (b_ex.FaultCause) {
                int size = b_ex.FaultCause->size( );
                for (int i = 0; i < size; i++) {
                        excmsg += (*b_ex.FaultCause)[i]+"\n";
                }
        }
        //method
        if (meth.size() > 0){excmsg +="Method: "+meth+"\n";}
        //errorcode
        if (errcode){if (errcode->size()>0){excmsg += "Error code: " + *errcode+"\n";}}
        return excmsg;
}




int main (int argc,char **argv){
	string endpoint = "";
	string version = "";
	string delegID =	"";
	string proxy = "";
	if (argc < 2  ){
		cout << "usage " << argv[0] << " <endpoint-url>\n";
		exit(-1);
	}
	endpoint = argv[1];
	ConfigContext *cxt = new ConfigContext("", endpoint, "");
	cout << "Testing endpoint: " << endpoint << "\n";
	cout << "==========================================\n\n";
	try{
		cout << "Connecting to " << endpoint << "\n\n";
		version = getVersion(cxt) ;
		cout << "WMProxy Version = " << version << "\n\n" ;
	} catch (BaseException &exc){
		cout << "Failed :(((\n";
		cout << "Exception: " << errMsg(exc)<< "\n";
	}
	/*
	cout << "Test: getProxyReq\n";
	cout << "----------------------------------\n\n";
	cout << "Calling getProxyReq service con delegationId: "<< delegID << "\n";
	try{
		cout << "Connecting to " << endpoint << "\n\n";
		string proxy = grstGetProxyReq(delegID, cxt);
		cout << "Success !!!\n";
		cout << "Proxy:\n" << proxy << "\n\n";
	} catch (BaseException &exc){
		cout << "Failed :(((\n";
		cout << "Exception: " << errMsg(exc)<< "\n";
	}
	*/

	try{
		cout << "Test: getDelegationVersion\n";
		cout << "----------------------------------\n";
		cout << "Calling  getDelegationVersion service .....\n";
		cout << "Connecting to " << endpoint << "\n";
		version = getDelegationVersion(cxt );
		cout << "DelegationVersion = " << version << "\n";
		cout << "Test: getDelegationInterfaceVersion\n";
		cout << "----------------------------------\n";
		cout << "Calling getDelegationInterfaceVersion service .....\n";
		cout << "Connecting to " << endpoint << "\n";
		version = getDelegationInterfaceVersion(cxt );
		cout << "DelegationInterfaceVersion = " << version << "\n\n";

		cout << "Test: getNewProxyReq\n";
		cout << "----------------------------------\n";
		cout << "Calling getNewProxyReq service .....\n";
		cout << "Connecting to " << endpoint << "\n";
		ProxyReqStruct newProxy = getNewProxyReq(cxt);
		cout << "Success !!!\n";
		delegID = newProxy.delegationId ;
		proxy = newProxy.proxy ;
		cout << "DelegationId = " << delegID << "\n";
		cout << "Proxy:\n" << proxy << "\n\n";
		cout << "Test: putProxy\n";
		cout << "----------------------------------\n";
		cout << "Calling putProxy service .....\n";
		cout << "Connecting to " << endpoint << "\n";
		putProxy(delegID, proxy, cxt);
		cout << "Success !!!\n\n";
		cout << "Test: getProxyTerminationTime\n";
		cout << "----------------------------------\n";
		cout << "Calling getProxyTerminationTime service .....\n";
		cout << "Connecting to " << endpoint << "\n";
		int tt = getProxyTerminationTime("", cxt);
		cout << "Success !!!\n";
		cout << "Termination time = " << tt << "\n\n";

/*
		cout << "Test: proxyDestroy\n";
		cout << "----------------------------------\n\n";
		cout << "Calling proxyDestroy service .....\n";
		cout << "Connecting to " << endpoint << "\n\n";
		proxyDestroy(delegID, cxt);
		cout << "Success !!!\n\n";
*/
	} catch (BaseException &exc){
	 	cout << "Failed :(((\n";
		cout << "Exception: " << errMsg(exc)<< "\n";
	 }
	return 0;
}
