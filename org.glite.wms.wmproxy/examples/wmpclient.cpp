/*
*	Copyright (c) Members of the EGEE Collaboration. 2004.
*	See http://www.eu-egee.org/partners/ for details on the copyright holders.
*
*	Licensed under the Apache License, Version 2.0 (the "License");
*	you may not use this file except in compliance with the License.
*	You may obtain a copy of the License at
*
*	    http://www.apache.org/licenses/LICENSE-2.0
*
*	Unless required by applicable law or agreed to in writing, software
*	distributed under the License is distributed on an "AS IS" BASIS,
*	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*	See the License for the specific language governing permissions and
*	limitations under the License.
*/

//
// File: wmpclient.cpp
// Author: Giuseppe Avellino <giuseppe.avellino@datamat.it>
//

// This is an example of a Workload Manager Proxy client

#include "WMProxy.nsmap"
#include "soapWMProxyProxy.h"
#include "soapDelegationSoapBindingProxy.h"

#include "wmpresponsestruct.h"

#include <fstream>
#include <sstream>

// Gridsite C library
extern "C" {
	#include "gridsite.h"
}

#define LOCALPROXY      "/tmp/x509up_u604" // User Proxy location
#define CAPATH          "/etc/grid-security/certificates"
#define EXPIREMINUTES   24000

using namespace std;

const string DELEGATION_ID = "delegationID"; // User delegation ID

/**
 * Converts int to string
 */
string
itos(int i)
{
	stringstream s;
	s << i;
	return s.str();
}

void
usage()
{
	cout<<endl;
	cout<<"Please provide a command as argument"<<endl;
	cout<<"Available commands:"<<endl;
	cout<<"- getVersion -> version"<<endl;
	cout<<"- jobRegister-> register"<<endl;
	cout<<"- jobStart -> start"<<endl;
	cout<<"- jobSubmit -> submit"<<endl;
	cout<<"- jobCancel -> cancel"<<endl;
	cout<<"- getMaxInputSandboxSize -> sbsize"<<endl;
	cout<<"- getSandboxDestURI -> sburi"<<endl;
	cout<<"- getSandboxBulkDestURI -> bsburi"<<endl;
	cout<<"- getTotalQuota -> totalquota"<<endl;
	cout<<"- getFreeQuota -> freequota"<<endl;
	cout<<"- jobPurge -> purge"<<endl;
	cout<<"- getOutputFileList -> filelist"<<endl;
	cout<<"- jobListMatch -> listmatch"<<endl;
	cout<<"- getJobTemplate -> jobtemplate"<<endl;
	cout<<"- getDAGTemplate -> dagtemplate"<<endl;
	cout<<"- getIntParametricJobTemplate -> paraminttemplate"<<endl;
	cout<<"- getStringParametricJobTemplate -> paramstringtemplate"<<endl;
	cout<<"- getCollectionTemplate -> colltemplate"<<endl;
	cout<<"- get/putProxy -> allproxy"<<endl;
	cout<<"- getACLItems -> getitems"<<endl;
	cout<<"- addACLItems -> additems"<<endl;
	cout<<"- removeACLItem -> remitem"<<endl;
	cout<<"- getDelegatedProxyInfo -> proxyinfo"<<endl;
	cout<<"- enableFilePerusal -> enafp"<<endl;
	cout<<"- getPerusalFiles -> getfp"<<endl;
}


void
printFault(const WMProxy &ns)
{
	soap_print_fault(ns.soap, stderr);
	ns1__BaseFaultType *ex = NULL;
	if (ns.soap->version == 2) {
		ex = (ns1__BaseFaultType*)ns.soap->fault->SOAP_ENV__Detail->fault;
	} else {
		ex = (ns1__BaseFaultType*)ns.soap->fault->detail->fault;
	}
	if (ex) {
		cout<<"--- Method Name: "<<ex->methodName<<endl;
		cout<<"--- Timestamp: "<<ex->Timestamp<<endl;
		cout<<"--- ErrorCode: "<<*(ex->ErrorCode)<<endl;
		cout<<"--- Description: "<<*(ex->Description)<<endl;
		cout<<"--- Fault Cause:"<<endl;
		for (unsigned int i = 0; i < ex->FaultCause.size(); i++) {
			cout<<(ex->FaultCause)[i]<<endl;
		}
	}
}

void
printFault(const DelegationSoapBinding &deleg)
{
	soap_print_fault(deleg.soap, stderr);
	if (deleg.soap->fault->detail->__type == SOAP_TYPE_ns2__DelegationExceptionType) {
		ns2__DelegationExceptionType *ex = NULL;
		if (deleg.soap->version == 2) {
			ex = (ns2__DelegationExceptionType*)deleg.soap->fault->SOAP_ENV__Detail->fault;
		} else {
			ex = (ns2__DelegationExceptionType*)deleg.soap->fault->detail->fault;
		}
		if (ex) {
			cout<<"---  Message: "<<*(ex->message)<<endl;
		}
	} else {
		cout<<"Error in fault type"<<endl;
	}
}

time_t ASN1_UTCTIME_get(const ASN1_UTCTIME *s)
{
    struct tm tm;
    int offset;
	time_t now = time(NULL);

    memset(&tm,'\0',sizeof tm);
#define g2(p) (((p)[0]-'0')*10+(p)[1]-'0')
    tm.tm_year=g2(s->data);
    if(tm.tm_year < 50)
            tm.tm_year+=100;
    tm.tm_mon=g2(s->data+2)-1;
    tm.tm_mday=g2(s->data+4);
    tm.tm_hour=g2(s->data+6);
    tm.tm_min=g2(s->data+8);
    tm.tm_sec=g2(s->data+10);
    if (s->data[12] == 'Z') {
    	offset=0;
    } else {
	    offset=g2(s->data+13)*60+g2(s->data+15);
	    if (s->data[12] == '-') {
            offset= -offset;
        }
    }
#undef g2

    return timegm(&tm)-offset*60;
}

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		usage();
		exit(1);
	}
	
	// WMProxy service
	WMProxy ns;
	// ghemon.cnaf.infn.it
	ns.endpoint = "https://131.154.100.122:7443/glite_wms_wmproxy_server";
	
	//ns.endpoint = "https://10.100.4.52:7443/glite_wms_wmproxy_server";
	
	// Delegation service
    DelegationSoapBinding deleg;
    // ghemon.cnaf.infn.it
	deleg.endpoint = "https://131.154.100.122:7443/glite_wms_wmproxy_server";
	
	//deleg.endpoint = "https://10.100.4.52:7443/glite_wms_wmproxy_server";

	ERR_load_crypto_strings ();
  	OpenSSL_add_all_algorithms();

    if (soap_ssl_client_context(ns.soap,
        SOAP_SSL_NO_AUTHENTICATION,
        LOCALPROXY, /* keyfile: required only when client must authenticate to server */
        "", /* password to read the key file */
        NULL, /* optional cacert file to store trusted certificates (needed to verify server) */
        "/etc/grid-security/certificates", /* optional capath to direcoty with trusted certificates */
        NULL /* if randfile!=NULL: use a file with random data to seed randomness */
        ))
    {
      soap_print_fault(ns.soap, stderr);
      exit(1);
    }
    
    if (soap_ssl_client_context(deleg.soap,
        SOAP_SSL_NO_AUTHENTICATION,
        LOCALPROXY, /* keyfile: required only when client must authenticate to server */
        "", /* password to read the key file */
        NULL, /* optional cacert file to store trusted certificates (needed to verify server) */
        "/etc/grid-security/certificates", /* optional capath to direcoty with trusted certificates */
        NULL /* if randfile!=NULL: use a file with random data to seed randomness */
        ))
    {
      soap_print_fault(deleg.soap, stderr);
      exit(1);
    }
    
    
	string *op = new string(argv[1]);
	cout<<"Contacting server..."<<endl;
	cout<<"Requested Operation: "<<argv[1]<<endl;
	
	string operation = *op;
	
	// OPERATION getVersion
	if (operation == "version") {
		ns1__getVersionResponse response;
		if (ns.ns1__getVersion(response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			cout<<"Version: "<<response.version<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION jobRegister
	} else if (operation == "register") {
		if (argc < 3) {
			cout<<"provide a file name representing the job to register"<<endl;
			exit(1);
		}
		ifstream in(argv[2], ios::in);
		if (!in.good()) {
			cout<<"Error in source file"<<endl;
			exit(1);
		}
		string s;
		string jdl = "";
		while (getline(in, s, '\n')) {
			cout<<"line: "<<s<<endl;
			jdl += s + '\n';
		}
		in.close();
		cout<<"jdl: "<<jdl<<endl;

		ns1__jobRegisterResponse response;
		if (ns.ns1__jobRegister(jdl, DELEGATION_ID, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			cout<<"JobId: "<<(response._jobIdStruct->id)<<endl;
			vector<ns1__JobIdStructType*> children = response._jobIdStruct->childrenJob;
			cout<<"Children number: "<<children.size()<<endl;
			for (unsigned int i = 0; i < children.size(); i++) {
				cout<<"Child "<<i + 1<<" - name: "<<*((children)[i]->name)
					<<" - ID: "<<(children)[i]->id<<endl;
			}
		} else {
			printFault(ns);
		}
	// OPERATION jobStart
	} else if (operation == "start") {
		if (argc < 3) {
			cout<<"provide a job id representing the job to start"<<endl;
			exit(1);
		}
		string *jobid = new string(argv[2]);
		ns1__jobStartResponse response;
		if (ns.ns1__jobStart(*jobid, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			cout<<"Job Started"<<endl;
		} else {
			printFault(ns);
		}
		delete jobid;
	// OPERATION jobSubmit
	} else if (operation == "submit") {
		if (argc < 3) {
			cout<<"provide a file name representing the job to submit"<<endl;
			exit(1);
		}
		ifstream in(argv[2], ios::in);
		if (!in.good()) {
			cout<<"Error in source file"<<endl;
			exit(1);	
		}
		string s;
		string jdl = "";
		while (getline(in, s, '\n')) {
			cout<<"line: "<<s<<endl;
			jdl += s;
		}
		in.close();
		cout<<"--- jdl: "<<jdl<<endl;

		ns1__jobSubmitResponse response;
		if (ns.ns1__jobSubmit(jdl, DELEGATION_ID, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			cout<<"JobId: "<<(response._jobIdStruct->id)<<endl;
			vector<ns1__JobIdStructType*> children = response._jobIdStruct->childrenJob;
			cout<<"Children number: "<<children.size()<<endl;
			for (unsigned int i = 0; i < children.size(); i++) {
				cout<<"Child "<<i + 1<<" - name: "<<*((children)[i]->name)
					<<" - ID: "<<(children)[i]->id<<endl;
			}
		} else {
			printFault(ns);
		}
	// OPERATION jobCancel
	} else if (operation == "cancel") {
		if (argc < 3) {
			cout<<"Provide a job id as argument"<<endl;
			exit(1);
		}
		cout<<"JobId: "<<argv[2]<<endl;
		ns1__jobCancelResponse response;
		if (ns.ns1__jobCancel(argv[2], response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION getMaxInputSandboxSize
	} else if (operation == "sbsize") {
		ns1__getMaxInputSandboxSizeResponse response;
		if (ns.ns1__getMaxInputSandboxSize(response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			cout<<"Size: "<<response.size<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION getSandboxDestURI
	} else if (operation == "sburi") {
		if (argc < 3) {
			cout<<"Provide a job id as argument"<<endl;
			exit(1);
		}
		ns1__getSandboxDestURIResponse response;
		if (ns.ns1__getSandboxDestURI(argv[2], response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			for (int i = 0; i < response._path->Item.size(); i++) {
				cout<<"Destination URI: "<<(((response._path->Item))[i])<<endl;
			}
		} else {
			printFault(ns);
		}
	// OPERATION getSandboxBulkDestURI
	} else if (operation == "bsburi") {
		if (argc < 3) {
			cout<<"Provide a job id as argument"<<endl;
			exit(1);
		}
		ns1__getSandboxBulkDestURIResponse response;
		if (ns.ns1__getSandboxBulkDestURI(argv[2], response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			vector<ns1__DestURIStructType*> uris = response._DestURIsStructType->Item;
			for (int i = 0; i < uris.size(); i++) {
				cout<<"Destination URIs:"<<endl;
				for (int j = 0; j < (uris)[i]->Item.size(); j++) {
					cout<<j<<"  "<<(((uris)[i]->Item))[j]<<endl;
				}
			}

		} else {
			printFault(ns);
		}
	// OPERATION getTotalQuota
	} else if (operation == "totalquota") {
		ns1__getTotalQuotaResponse response;
		if (ns.ns1__getTotalQuota(response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			cout<<"Total Quota soft limit: "<<response.softLimit<<endl;
			cout<<"Total Quota hard limit: "<<response.hardLimit<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION getFreeQuota
	} else if (operation == "freequota") {
		ns1__getFreeQuotaResponse response;
		if (ns.ns1__getFreeQuota(response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			cout<<"Free Quota soft limit: "<<response.softLimit<<endl;
			cout<<"Free Quota hard limit: "<<response.hardLimit<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION jobPurge
	} else if (operation == "purge") {
		if (argc < 3) {
			cout<<"Provide a job id as argument"<<endl;
			exit(1);
		}
		cout<<"JobId: "<<argv[2]<<endl;
		ns1__jobPurgeResponse response;
		if (ns.ns1__jobPurge(argv[2], response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION getOutputFileList
	} else if (operation == "filelist") {
		if (argc < 3) {
			cout<<"Provide a job id as argument"<<endl;
			exit(1);
		}
		cout<<"JobId: "<<argv[2]<<endl;
		ns1__getOutputFileListResponse response;
		if (ns.ns1__getOutputFileList(argv[2], response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			if (response._OutputFileAndSizeList->file.size()) {
				for (int i = 0; i < response._OutputFileAndSizeList->file.size(); i++) {
					cout<<"Name: "<<(response._OutputFileAndSizeList->file)[i]->name<<endl;
					cout<<"Size: "<<(response._OutputFileAndSizeList->file)[i]->size<<endl;
					cout<<endl;
				}
			} else {
				cout<<"No result found"<<endl;
			}
		} else {
			printFault(ns);
		}
	// OPERATION jobListMatch
	} else if (operation == "listmatch") {
		if (argc < 3) {
			cout<<"provide a file name representing the job"<<endl;
			exit(1);
		}
		ifstream in(argv[2], ios::in);
		if (!in.good()) {
			cout<<"Error in source file"<<endl;
			exit(1);
		}
		string s;
		string jdl = "";
		while (getline(in, s, '\n')) {
			cout<<"line: "<<s<<endl;
			jdl += s;
		}
		in.close();
		cout<<"jdl: "<<jdl<<endl;

		ns1__jobListMatchResponse response;
		if (ns.ns1__jobListMatch(jdl, DELEGATION_ID, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			if (response._CEIdAndRankList->file.size()) {
				cout<<"Size: "<<response._CEIdAndRankList->file.size()<<endl;
				for (int i = 0; i < response._CEIdAndRankList->file.size();i++) {
					cout<<"CE: "<<(response._CEIdAndRankList->file)[i]->name
						<<" - Rank: "<<(response._CEIdAndRankList->file)[i]->size<<endl;
					cout<<endl;
				}
			} else {
				cout<<"No result found"<<endl;
			}
		} else {
			printFault(ns);
		}
	// OPERATION getJobTemplate
	} else if (operation == "jobtemplate") {
		ns1__getJobTemplateResponse response;
		string executable ="PIPPO.exe";
		string arguments = "-a";
		string requirements = "true";
		string rank = "pluto";
		ns1__JobTypeList *job_type_list = new ns1__JobTypeList();
		vector<ns1__JobType> job_type ;
		ns1__JobType type = ns1__JobType__NORMAL;
		job_type.push_back(type);
		job_type_list->jobType = job_type;
		if (ns.ns1__getJobTemplate(job_type_list, executable, arguments,
			requirements, rank, response) == SOAP_OK) {
				cout<<"Operation Successfully"<<endl;
				cout<<"Template: "<<response._jdl<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION getDAGTemplate
	} else if (operation == "dagtemplate") {
		ns1__getDAGTemplateResponse response;
		string requirements = "true";
		string rank = "pluto";
		vector<ns1__GraphStructType*> vect;
		ns1__GraphStructType *node = NULL;
		for (unsigned int i = 0; i < 4; i++) {
			node = new ns1__GraphStructType();
			node->name = new string("NODE" + itos(i));
			node->childrenJob = *(new vector<ns1__GraphStructType*>);
			vect.push_back(node);
		}
		node = new ns1__GraphStructType();
		node->name = NULL;
		node->childrenJob = vect;
		if (ns.ns1__getDAGTemplate(node,
			requirements, rank, response) == SOAP_OK) {
				cout<<"Operation Successfully"<<endl;
				cout<<"Template: "<<response._jdl<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION getCollectionTemplate
	} else if (operation == "colltemplate") {
		ns1__getCollectionTemplateResponse response;
		int job_num = 5;
		string requirements = "true";
		string rank = "pluto";
		if (ns.ns1__getCollectionTemplate(job_num, requirements, rank, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			cout<<"Template: "<<response._jdl<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION getIntParametricJobTemplate
	} else if (operation == "paraminttemplate") {
		ns1__getIntParametricJobTemplateResponse response;
		ns1__StringList *attributes = new ns1__StringList();
		attributes->Item;
		int param = 10;
		int parameter_start = 0;
		int parameter_step = 2;
		attributes->Item.push_back("InputSandbox");
		string requirements = "true";
		string rank = "pluto";
		if (ns.ns1__getIntParametricJobTemplate(attributes, param, parameter_start,
				parameter_step, requirements, rank, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			cout<<"Template: "<<response._jdl<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION getStringParametricJobTemplate
	} else if (operation == "paramstringtemplate") {
		ns1__getStringParametricJobTemplateResponse response;

		ns1__StringList *attributes = new ns1__StringList();
		attributes->Item;
		attributes->Item.push_back("InputSandbox");
		ns1__StringList *params = new ns1__StringList();
		params->Item;
		params->Item.push_back("try_1");
		params->Item.push_back("try_2");
		params->Item.push_back("try_3");
		string requirements = "true";
		string rank = "pluto";
		if (ns.ns1__getStringParametricJobTemplate(attributes, params,
				requirements, rank, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			cout<<"Template: "<<response._jdl<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION getMaxInputSandboxSize
	} else if (operation == "sbsize") {
		ns1__getMaxInputSandboxSizeResponse response;
		if (ns.ns1__getMaxInputSandboxSize(response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			cout<<"Size: "<<response.size<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION getProxyReq & putProxy
	} else if (operation == "allproxy") {
		ns2__getProxyReqResponse response;
		string delegation_id = DELEGATION_ID;
		if (deleg.ns2__getProxyReq(delegation_id, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			cout<<"Proxy: "<<endl<<"|"<<response._getProxyReqReturn<<"|"<<endl;
		} else {
			printFault(deleg);
		}
		
		// Signing...
       	char *certtxt;
       	char *resp;
    	resp = strdup(response._getProxyReqReturn.c_str());
       	cout<<"RESP = "<<resp<<endl;
       	cout<<"LOCALPROXY = "<<LOCALPROXY<<endl;

       	time_t timeleft;
		BIO *in = NULL;
		X509 *x = NULL;
  
		in = BIO_new(BIO_s_file());
		if (BIO_read_filename( in, LOCALPROXY ) > 0) {
			x = PEM_read_bio_X509(in, NULL, 0, NULL);
			timeleft = (ASN1_UTCTIME_get(X509_get_notAfter(x))
				- time(NULL)) / 60;
		}

       	if (GRSTx509MakeProxyCert(&certtxt, stderr, resp,
        		LOCALPROXY, LOCALPROXY, timeleft) != GRST_RET_OK) {
        	return 1;
       	}

		cout<<"Calling ns1__putProxyResponse..."<<endl;
		ns2__putProxyResponse response2;
		string value = certtxt;
		cout<<"Proxy:"<<endl<<"|"<<value<<"|"<<endl;
		if (deleg.ns2__putProxy(delegation_id, value, response2) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
		} else {
			printFault(deleg);
		}
	// OPERATION getProxyReq
	} else if (operation == "getproxy") {
		ns2__getProxyReqResponse response;
		if (deleg.ns2__getProxyReq(DELEGATION_ID, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			cout<<"Proxy: "<<endl<<"|"<<response._getProxyReqReturn<<"|"<<endl;
		} else {
			printFault(deleg);
		}
	// OPERATION getACLItems
	} else if (operation == "getitems") {
		if (argc < 3) {
			cout<<"provide a job id"<<endl;
			exit(1);
		}
		string *jobid = new string(argv[2]);
		ns1__getACLItemsResponse response;
		if (ns.ns1__getACLItems(*jobid, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			if (response._items->Item.size()) {
				for (int i = 0; i < response._items->Item.size(); i++) {
					cout<<"Item: "<<((response._items->Item))[i]<<endl;
				}
			} else {
				cout<<"No result found"<<endl;
			}
		} else {
			printFault(ns);
		}
	// OPERATION addACLItems
	} else if (operation == "additems") {
		if (argc < 4) {
			cout<<"provide an item and a job id"<<endl;
			exit(1);
		}
		string *dn = new string(argv[2]);
		string *jobid = new string(argv[3]);
		ns1__StringList *dnlist = new ns1__StringList();
		dnlist->Item;
		dnlist->Item.push_back(*dn);
		ns1__addACLItemsResponse response;
		if (ns.ns1__addACLItems(*jobid, dnlist, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION removeACLItem
	} else if (operation == "remitem") {
		if (argc < 4) {
			cout<<"provide an item and a job id"<<endl;
			exit(1);
		}
		string *dn = new string(argv[2]);
		string *jobid = new string(argv[3]);
		ns1__removeACLItemResponse response;
		if (ns.ns1__removeACLItem(*jobid, *dn, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION getDelegatedProxyInfo
	} else if (operation == "proxyinfo") {
		if (argc < 3) {
			cout<<"provide an job id"<<endl;
			exit(1);
		}
		string *jobid = new string(argv[2]);
		ns1__getDelegatedProxyInfoResponse response;
		if (ns.ns1__getDelegatedProxyInfo(*jobid, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			if (response._items->Item.size()) {
				for (int i = 0; i < response._items->Item.size(); i++) {
					cout<<"Info: "<<((response._items->Item))[i]<<endl;
				}
			} else {
				cout<<"No info found"<<endl;
			}
		} else {
			printFault(ns);
		}
	// OPERATION enableFilePerusal
	} else if (operation == "enafp") {
		if (argc < 4) {
			cout<<"provide at least a file name and a job id as last argument"<<endl;
			exit(1);
		}
		string *jobid = new string(argv[argc-1]);
		ns1__StringList *files = new ns1__StringList();
		files->Item;
		for (int i = 2; i < argc - 1; i++) {
			files->Item.push_back(string(argv[i]));
		} 
		ns1__enableFilePerusalResponse response;
		if (ns.ns1__enableFilePerusal(*jobid, files, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
		} else {
			printFault(ns);
		}
	// OPERATION getPerusalFiles
	} else if (operation == "getfp") {
		if (argc < 5) {
			cout<<"provide an item, a flag (0 for false, != 0 for true) and a job id"<<endl;
			exit(1);
		}
		string *filename = new string(argv[2]);
		int allChunks = atoi(argv[3]);
		string *jobid = new string(argv[4]);
		ns1__getPerusalFilesResponse response;
		if (ns.ns1__getPerusalFiles(*jobid, *filename, allChunks, response) == SOAP_OK) {
			cout<<"Operation Successfully"<<endl;
			for (int i = 0; i < response._fileList->Item.size(); i++) {
				cout<<"URI: "<<((response._fileList->Item))[i]<<endl;
			}
		} else {
			printFault(ns);
		}
	} else {
		usage();
		exit(1);
	}

    soap_done(ns.soap);
	return 0;
}


