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


// common utilities
#include "wmp_job_examples.h"
// std library
#include <vector>
#include <sstream>
// wmproxy api utilities
#include "glite/wms/wmproxyapi/wmproxy_api_utilities.h"
// AD
#include "glite/jdl/Ad.h"
#include "glite/jdl/ExpDagAd.h"
// JDL
#include "glite/jdl/jdl_attributes.h"
#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/extractfiles.h"
#include "glite/jdl/adconverter.h"
#include "glite/jdl/collectionad.h"
// CURL
#include "curl/curl.h"
// file-stat
#include "sys/stat.h"
#include "fcntl.h"
// BOOST
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/exception.hpp"


using namespace std;
namespace wmpapi = glite::wms::wmproxyapi;
namespace exutils = glite::wms::wmproxyapi::examples::utilities;
namespace apiutils = glite::wms::wmproxyapiutils;
namespace exc = glite::wmsutils::exception;
namespace requestad = glite::jdl;
namespace fs = boost::filesystem;

// long option strings
static const char* LONGOPT_SERVICE = "endpoint" ;
static const char* LONGOPT_JDL= "jdl-file" ;
static const char* LONGOPT_COLLECTION = "collection" ;
static const char* LONGOPT_DELEGATION = "delegation";
static const char* LONGOPT_AUTODEL = "auto-delegation";
static const char* LONGOPT_OUTPUT = "output";
static const char* LONGOPT_VERBOSE = "verbose";
static const char* LONGOPT_HELP = "help";

// short options chars
static const char SHORTOPT_SERVICE = 'e';
static const char SHORTOPT_JDL = 'f';
static const char SHORTOPT_COLLECTION = 'c' ;
static const char SHORTOPT_DELEGATION = 'd';
static const char SHORTOPT_AUTODEL = 'a';
static const char SHORTOPT_OUTPUT = 'o';
static const char SHORTOPT_VERBOSE = 'v';
static const char SHORTOPT_HELP = '?';

/*
	long options
*/
static const struct option long_options[] = {
    { LONGOPT_SERVICE,		required_argument,	NULL, SHORTOPT_SERVICE},
    { LONGOPT_JDL,           	required_argument,	NULL, SHORTOPT_JDL},
    { LONGOPT_COLLECTION,    required_argument,	NULL, SHORTOPT_COLLECTION},
    { LONGOPT_DELEGATION,	required_argument,	NULL, SHORTOPT_DELEGATION },
    { LONGOPT_OUTPUT,		required_argument,	NULL, SHORTOPT_OUTPUT},
    { LONGOPT_AUTODEL,		no_argument,		NULL, SHORTOPT_AUTODEL },
    { LONGOPT_VERBOSE,		no_argument ,		NULL, SHORTOPT_VERBOSE},
    { LONGOPT_HELP,			no_argument ,		NULL, SHORTOPT_HELP},
    {0, 0, 0, 0}
};

/*
	short options
*/
static const char* short_options = "e:f:d:o:c:av?";

/*
	Type of jobs
*/
enum JobType{
	NORMAL,
	DAG,
	COLLECTION
};

/*
* 	short usage help
*/
static void short_usage(const char *exe)
{
	fprintf(stderr,"\nUsage: %s <delegation_options> <jdl_options> [other options] \n",exe);
	fprintf(stderr,"where  \n");
	fprintf(stderr,"delegation_options (one of these two options is mandatory):\n");
	// delegation string
	fprintf(stderr,"\t--%s, -%c	 <delegation string>\n",
		LONGOPT_DELEGATION , SHORTOPT_DELEGATION);

	fprintf(stderr,"\t\tor\n");
	// auto-delegation
	fprintf(stderr,"\t--%s, -%c\n\n",
		LONGOPT_AUTODEL , SHORTOPT_AUTODEL);
	// jdl
	fprintf(stderr,"jdl_options (one of these two options is mandatory):\n");
	fprintf(stderr,"\t--%s, -%c	 <JDL filepath>\t(for normal and DAG jobs)\n",
		LONGOPT_JDL , SHORTOPT_JDL);

	fprintf(stderr,"\t\tor\n");
	// JDL for collection
	fprintf(stderr,"\t--%s, -%c	 <collection of JDLs path>\t(for collection of jobs)\n",
		LONGOPT_COLLECTION , SHORTOPT_COLLECTION);

	fprintf(stderr,"\nother options (not mandatory):\n\n");
	// endpoint
	fprintf(stderr,"\t--%s, -%c		<endpoint URL> (mandatory if the %s is not set)\n",
		LONGOPT_SERVICE , SHORTOPT_SERVICE, GLITE_WMPROXY_ENDPOINT);
	// output file
	fprintf(stderr,"\t--%s -%c		<output file>\n",
		LONGOPT_OUTPUT , SHORTOPT_OUTPUT);
	// verbosity
	fprintf(stderr,"\t--%s -%c\n",
		LONGOPT_VERBOSE , SHORTOPT_VERBOSE);

	fprintf(stderr,"\nWMProxy Testing User Interface\n");
	fprintf(stderr,"Copyright (C) 2005 by DATAMAT SpA\n\n");
	fprintf(stderr, "Please report any bug at:\n\t\tfabrizio.pacini@datamat.it\n\n");

	exit (-1);
}

/*
	long usage help message
*/
static void long_usage(const char *exe)
{
	// main message
	fprintf(stderr,"\nUsage: %s <delegation_options> <jdl_options> [other options] \n",exe);
	fprintf(stderr,"where  \n");
	// DELEGATION options ==================
	fprintf(stderr,"delegation_options (one of these two options is mandatory):\n");
	// delegation string
	fprintf(stderr, "\t-%c delegation_string\n",  SHORTOPT_DELEGATION);
	fprintf(stderr, "\t--%s delegation_string\n", LONGOPT_DELEGATION);
	fprintf(stderr, "\tuser proxy delegation string previously used to delegate credential to the endpoint\n");
	fprintf(stderr, "\t(by glite-wmp-delegate-proxy command)\n");
	fprintf(stderr,"\t\tor\n");
	fprintf(stderr, "\t-%c\n",  SHORTOPT_AUTODEL);
	fprintf(stderr, "\t--%s\n", LONGOPT_AUTODEL);
	fprintf(stderr, "\tperforms automatic delegation of user credential before submitting the job\n");
	// jdl file or collection
	fprintf(stderr,"jdl_options (one of these two options is mandatory):\n");
	fprintf(stderr, "\t-%c collection-dir\n", SHORTOPT_COLLECTION);
	fprintf(stderr, "\t--%s collection-dir\n",LONGOPT_COLLECTION);
	fprintf(stderr, "\t\t.collection-dir is the pathname to the directory containing a set of JDL files (on the submitting machine)\n");
	fprintf(stderr, "\t\trepresenting a collection. The pathanme can be absolute or relative.\n\n");
	fprintf(stderr,"\t\tor\n");
	fprintf(stderr, "\t-%c file_path\n", SHORTOPT_JDL);
	fprintf(stderr, "\t--%s file_path\n",LONGOPT_JDL);
	fprintf(stderr, "\tthe file containing the JDL describing the job to be submitted.\n\tfile_path can be either a simple name or an absolute path (on the submitting machine).\n\n");

	fprintf(stderr,"options (not mandatory):\n");
	// endpoint
	fprintf(stderr, "\t-%c url\n", SHORTOPT_SERVICE);
	fprintf(stderr, "\t--%s url\n", LONGOPT_SERVICE);
	fprintf(stderr, "\twmproxy endpoint URL, overriding any setting of %s;\n",
		GLITE_WMPROXY_ENDPOINT);
	fprintf(stderr, "\tmandatory if the %s is not set.\n", GLITE_WMPROXY_ENDPOINT);
	fprintf(stderr, "\t(e.g.: https://<server>:<port>/<wmProxy-fcgi>)\n\n");
	// ouput file
	fprintf(stderr, "\t-%c out_file\n", SHORTOPT_OUTPUT);
	fprintf(stderr, "\t--%s out_file\n", LONGOPT_OUTPUT);
  	fprintf(stderr, "\twrites the generated edg_jobId assigned to the submitted job in the file specified by out_file.\n\tout_file can be either a simple name or an absolute path (on the submitting machine).\n\n");
	// verbosity
	fprintf(stderr, "\t-%c\n", SHORTOPT_VERBOSE);
	fprintf(stderr, "\t--%s\n", LONGOPT_VERBOSE);
	fprintf(stderr, "\tEnables verbose output\n\n");

	fprintf(stderr, "\t--help, -?\n");
	fprintf(stderr, "\tdisplays this message\n\n");
	fprintf(stderr,"\nWMProxy Testing User Interface\n");
	fprintf(stderr,"Copyright (C) 2005 by DATAMAT SpA\n\n");
	fprintf(stderr, "Please report any bug at:\n\t\tfabrizio.pacini@datamat.it\n");

	exit (-1);
}
  /*
  *	callback called during file uploading by curl
 *	in case if verbosity is on
*/
 size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
   size_t retcode;
   retcode = fread(ptr, size, nmemb, (FILE*)stream);
   fprintf(stderr, "%d byte read\n", retcode);

   return retcode;
 }

/*
*  Uploads InpustSB files to the Destination URIs
*	@param paths InputSB files
*	@param verbose verbosity
*/
void transferFiles (vector<pair<string,string> > paths, wmpapi::ConfigContext *cfs,bool verbose)
{
	// curl struct
	CURL *curl = NULL;
	// curl result code
	CURLcode res;
	// file struct
	struct stat file_info;
	FILE * hd_src  = NULL;
	// vector iterator
	vector <pair<string , string> >::iterator it ;
	// source and destination
	string source = "" ;
	string destination = "" ;
	// local filepath
	string file = "" ;
	// result message
	string result = "";
	// locat proxy filepath
	char *proxy = NULL;
        char *tcert = NULL;
	int hd = 0;
	long	httpcode = 0;
	// checks user free quota
	if ( ! exutils::checkFreeQuota (paths, cfs, verbose ) ){
		cerr << "\nError: unable to perform the submission" << endl;
		cerr << "(the size of your InputSanbox files exceeds your free quota on the endpoint server)"<< endl;
		exit(-1);
	}
	// user proxy
	proxy = (char*)apiutils::getProxyFile(cfs);
        if (!proxy){
		cerr << "\nError: unable to find a valid proxy to transfer the local InputSandox files to the DestinationURI(s)\n";
                exit(-1);
        }
        // trusted certificate pathname
	tcert = (char*)apiutils::getTrustedCert(cfs);
        if (!tcert){
		cerr << "\nError: no valid location of trusted certificates found\n";
                exit(-1);
        }
	// curl init
	curl_global_init(CURL_GLOBAL_ALL);
 	curl = curl_easy_init();
	 if(curl) {
	 	if ( verbose ) {
                	cout << "\n#### DEBUG #####\n";
			cout << "Input Sandbox file transferring ....\n" << endl;
			cout << "User proxy : " << proxy << endl;
			cout << "Transferring of number " << paths.size() << " files" << endl;
                        cout << "################\n";
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
			curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
		}
		// result message
		result = "\nInputSandbox file(s)  :\n";
		result += "--------------------------------------\n";
		// curl options: proxy
		curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
     		curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE,   "PEM");
		curl_easy_setopt(curl, CURLOPT_SSLCERT,	proxy);
       		curl_easy_setopt(curl, CURLOPT_SSLKEY, proxy);
		curl_easy_setopt(curl, CURLOPT_SSLKEYPASSWD, NULL);
      		 curl_easy_setopt(curl, CURLOPT_CAPATH, tcert);
		// curl options: no verify the ssl properties
		 curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
      		 curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		// enable uploading
		curl_easy_setopt(curl, CURLOPT_PUT, 1);
		// name of the file to be transferred
		for ( it = paths.begin() ; it != paths.end() ; it++ ){
			// curl options: source
			source = it->first ;
			hd_src = fopen(source.c_str(), "rb");
			if (hd_src == NULL) {
				cerr << "\nError : InputSandbox file not found (" << source << ")" << endl;
				exit(-1);
			}
			curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);
			// curl options: file size
			hd = open(source.c_str(), O_RDONLY) ;
			fstat(hd, &file_info);
			close(hd) ;
			curl_easy_setopt(curl,CURLOPT_INFILESIZE, file_info.st_size);
			// curl options: destination
			destination = it->second;
			curl_easy_setopt(curl,CURLOPT_URL, destination.c_str());
			if ( verbose ) {
                        	cout << "\n#### DEBUG #####\n";
				cout << "InputSandbox file : " << source << endl;
				cout << "Destination URI : " << destination << endl;
				cout << "Size : " << file_info.st_size << " byte"<< endl;
				cout << "\nTransferring ..... " ;
                                cout << "################\n";
			}
			// transferring ......
			res = curl_easy_perform(curl);
			// result
			if ( res != 0 ){
				cerr << "\nError: couldn't transfer the InputSandbox file : " + source << endl;
				cerr << "to " + destination << endl ;
				curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &httpcode);
				cerr << "http error code : " << httpcode << endl;
				exit(-1);
			} else {
				if ( verbose ) {
                                	cout << "\n#### DEBUG #####\n";
					cout << "File transferring: success" << endl ;
                                        cout << "################\n";
				}
			}
			// result string message
			result += "-> " + source + "\n";
			result += "to " + destination + "\n\n";
			fclose(hd_src);
		}
		// cleanup
		curl_easy_cleanup(curl);
	}
}
/*
*	performs the operations for the job submission
*	@param j_id JobId struct
*	@param endpoint endpoint URL
*	@param ad jobAd
*	@param cfs context
*	@param limit free-quota limit
*	@param verbose verbosity
*/
string submission(const wmpapi::JobIdApi &j_id, const string endpoint, requestad::Ad *ad, wmpapi::ConfigContext *cfs, bool verbose )
{
	// jobid and nodename
	string id = "";
	string jobid = "";
	string node = "";
	// final output message
	string outmsg = "";
	// Destination URI
	string *dest_uri = NULL;
	// InputSB files
	vector <string> paths ;
	vector <pair<string, string> > to_bcopied ;
        if (!ad){
		fprintf (stderr, "\nFatal error(submission): unable to read the Job Description\n");
		exit(-1);
	}
	// JOB-ID
	jobid = j_id.jobid ;
	id = jobid ;
        if (verbose){
        	cout << "\n#### DEBUG #####\n";
		cout << "The job has been successfully submitted with the jobid : " << jobid << "\n";
  		cout << "jdl=" << ad->toString( ) << "\n";
		cout << "Retrieving InputSanbox files ...." << endl;
                cout << "################\n";
	}
	// INPUTSANDBOX
	if (ad->hasAttribute(requestad::JDL::INPUTSB)){
		// gets the InputSandbox files
		paths = ad->getStringValue  (requestad::JDL::INPUTSB) ;
                if (!paths.empty()){
			dest_uri = exutils::getInputSBDestURI(jobid, cfs);
			if (!dest_uri){
				fprintf (stderr, "\nError: unable to retrieve the InputSandbox destination URI\n");
				fprintf (stderr, "jobid: %s", jobid.c_str() );
				exit(-1);
			}
			// gets the InputSandbox files to be transferred to destinationURIs
			requestad::toBcopied(requestad::JDL::INPUTSB, paths, to_bcopied, *dest_uri,"");
                        if (verbose){
                        	cout << "\n#### DEBUG #####\n";
                        	cout << "This  job has " << paths.size() << " InputSB files" << endl;
                        	cout << "Destination URI = " << *dest_uri << "\n";
                                cout << "################\n";
                	}
   		}
		// InputSandbox file transferring
		transferFiles (to_bcopied, cfs, verbose);
	}
 	return id;
}
/*
*	performs submission operation for a DAG job
*/

string  dag_submission(const wmpapi::JobIdApi &j_id, const string endpoint, JobType jobtype, requestad::ExpDagAd *dag, wmpapi::ConfigContext *cfs, bool verbose )
{
	// jobid and node-name
	string jobid = "";
	string id = "";
	string node = "";
	// Destination URI
	string *dest_uri = NULL;
	// InputSB files
	vector <string> paths ;
	vector <pair<string, string> > to_bcopied ;
	// children
	vector <wmpapi::JobIdApi*> children;
	wmpapi::JobIdApi* child = NULL;
	// iterators
	vector <wmpapi::JobIdApi*>::iterator it;
	std::ostringstream oss;
        if (!dag){
		fprintf (stderr, "\nFatal error: unable to read the DAG Job Description\n");
		exit(-1);
	}
        // MAIN JOB ====================
        // jobid
        jobid = j_id.jobid ;
        id = jobid ;
        if (verbose){
        	cout << "\n#### DEBUG #####\n";
                cout << "The job has been successfully submitted with the jobid : " << jobid << "\n";
                cout << "jdl=" << dag->toString( ) << "\n";
                cout << "Retrieving InputSanbox files ...." << endl;
                cout << "################\n";
        }
        // MAIN JOB: InputSandox files
        if (dag->hasAttribute(requestad::JDL::INPUTSB)) {
                // MAIN JOB: http destination URI
                dest_uri = exutils::getInputSBDestURI(jobid, cfs);
                if (!dest_uri){
                        fprintf (stderr, "\nError: unable to retrieve the InputSandbox destination URI\n");
                        fprintf (stderr, "(main job) jobid: %s", jobid.c_str() );
                        exit(-1);
                }
                // InputSB files
                paths = dag->getInputSandbox();
                // InputSB files to be transferred to the DestURI
                requestad::toBcopied(requestad::JDL::INPUTSB, paths, to_bcopied, *dest_uri, "");
                if (verbose){
                	cout << "\n#### DEBUG #####\n";
                        cout << "This  job has " << paths.size() << " InputSB files" << endl;
                        cout << "Destination URI = " << *dest_uri << "\n\n";
                        cout << "################\n";
                }
        } else {
                if (verbose){
                	cout << "\n#### DEBUG #####\n";
                        cout << "No InputSB files for this job" << endl;
                        cout << "################\n";
                }
        }
        // CHILDREN ====================
        children = j_id.children ;
        // Second part of the result message (children)
        if ( ! children.empty() ){
                for ( it = children.begin() ; it != children.end(); it++){
                        child = *it;
                        if (child){
                                // child: jobid
                                jobid = child->jobid ;
                                //child:  node name
                                if ( ! child->nodeName ){
                                        cerr << "\nFatal error: unable to retrieve the node name for child (" << jobid << ")\n";
                                        exit(-1);
                                }
                                node = *(child->nodeName);
                                if (verbose){
                                	cout << "\n#### DEBUG #####\n";
                                        cout << "JobId node : " << jobid << "\n";
                                        cout << "node name : " << node << "\n\n";
                                        cout << "Retrieving InputSanbox files ...." << endl;
                                        cout << "################\n";
                                }
                                // child: Destination URI
                                dest_uri = exutils::getInputSBDestURI(jobid, cfs);
                                if (!dest_uri){
                                        fprintf (stderr, "\nError: unable to retrieve the InputSandbox destination URI\n");
                                        fprintf (stderr, "jobid: %s", jobid.c_str() );
                                        fprintf (stderr, "node: %s", node.c_str() );
                                        exit(-1);
                                }
                                // child: InputSandbox files
                                paths = dag->getNodeStringValue(node, requestad::JDL::INPUTSB);
                                if (!paths.empty()){
                                        requestad::toBcopied(requestad::JDL::INPUTSB, paths, to_bcopied, *dest_uri,"");
                                        if (verbose){
                                        	cout << "\n#### DEBUG #####\n";
                                                cout << "This node has " << paths.size() << " InputSB files" << endl;
                                                cout << "Destination URI = " << *dest_uri << "\n\n";
                                                cout << "################\n";
                                        }
                                } else{
                                        if (verbose){
                                        	cout << "\n#### DEBUG #####\n";
                                                cout << "No InputSB files for this node" << endl;
                                                cout << "################\n";
                                        }
                                }
                        }
                }
        }
        // InputSandbox file transferring
        transferFiles (to_bcopied, cfs, verbose);
        return id;
}

/*
* Gets the list of InputSB in the "list" vector for "node"
*/
vector<string> getNodeFileList (const string &node, vector< pair<string ,vector<string > > > &list){
	const vector<string> empty;
        vector< pair<string ,vector<string > > >::iterator it ;
        for ( it = list.begin() ; it != list.end( ) ; it++ ){
                if (node == it->first){
			return it->second ;
                }
   	}
	return empty;
}
/*
* Handles a collection
*/
string collection_submission(const wmpapi::JobIdApi &j_id, const string endpoint, JobType jobtype, requestad::CollectionAd *collect, wmpapi::ConfigContext *cfs, bool verbose )
{
	// jobid and node-name
	string jobid = "";
	string id = "";
	string node = "";
	// destURI protocol
	string protocol = "";
	// Destination URI
	string *dest_uri = NULL;
	// InputSB files
	vector< pair<string ,vector<string > > > files;
        vector<string> paths;
	vector <pair<string, string> > to_bcopied ;
	// children
	vector <wmpapi::JobIdApi*> children;
	wmpapi::JobIdApi* child = NULL;
	// iterators
	vector <wmpapi::JobIdApi*>::iterator it;
	std::ostringstream oss;
        if (!collect){
		fprintf (stderr, "\nFatal error: unable to read the  collection Description\n");
		exit(-1);
	}
        // MAIN JOB ====================
        // jobid
        jobid = j_id.jobid ;
        id = jobid ;
        if (verbose){
        	cout << "\n#### DEBUG #####\n";
                cout << "Info: The job has been successfully submitted with the jobid : " << jobid << "\n";
                cout << "Info: jdl=" << collect->toString( ) << "\n";
                cout << "Info: Retrieving InputSanbox files ...." << endl;
                cout << "################\n";
        }
        // Main InputSandbox
        if (collect->hasAttribute(requestad::JDL::INPUTSB)){
                paths = collect->getStringValue(requestad::JDL::INPUTSB);
                dest_uri = exutils::getInputSBDestURI(jobid, cfs);
                if (!dest_uri){
                        fprintf (stderr, "\nError: unable to retrieve the InputSandbox destination URI\n");
                        fprintf (stderr, "(main job) jobid: %s", jobid.c_str() );
                        exit(-1);
                }
                if (verbose){
                	cout << "\n#### DEBUG #####\n";
                        cout << "Info: This  job has " << paths.size() << " InputSB files" << endl;
                        cout << "Info: Destination URI = " << *dest_uri << "\n\n";
			 cout << "################\n";
                }
                requestad::toBcopied(requestad::JDL::INPUTSB, paths, to_bcopied, *dest_uri, "");
        }
        // InputSB files of the nodes
        files = collect->getNodeStringValues (requestad::JDL::INPUTSB);
        // NODES  ====================
        children = j_id.children ;
        // Second part of the result message (children)
        if ( ! children.empty() ){
                for ( it = children.begin() ; it != children.end(); it++){
                        child = *it;
                        if (child){
                                // child: jobid
                                jobid = child->jobid ;

                                //child:  node name
                                if ( ! child->nodeName ){
                                        cerr << "\nError: unable to retrieve the node name for child (" << jobid << ")\n";
                                        exit(-1);
                                }
                                node = *(child->nodeName);
                                if (verbose){
                                	cout << "\n#### DEBUG #####\n";
                                        cout << "Info: children 	: " << jobid << "\n";
                                        cout << "Info: node name : " << node << "\n";
                                        cout << "Info: Retrieving InputSanbox files ...." << endl;
                                        cout << "################\n";
                                }
                                paths = getNodeFileList (node, files);
                                if ( !paths.empty()){
                                        dest_uri = exutils::getInputSBDestURI(jobid, cfs);
                                        if (!dest_uri){
                                                fprintf (stderr, "\nError: unable to retrieve the InputSandbox destination URI\n");
                                                fprintf (stderr, "jobid: %s", jobid.c_str() );
                                                fprintf (stderr, "node: %s", node.c_str() );
                                                exit(-1);
                                        }
                                        requestad::toBcopied(requestad::JDL::INPUTSB, paths, to_bcopied, *dest_uri,"");
                                        if (verbose){
                                        	cout << "\n#### DEBUG #####\n";
                                                cout << "This  job has " << paths.size() << " InputSB files" << endl;
                                                cout << "Destination URI = " << *dest_uri << "\n";
						cout << "################\n";
                                        }
                                }

                        }
                }
        }
        // InputSandbox file transferring
        transferFiles (to_bcopied, cfs, verbose);
	return id;
}
//std::vector< std::pair<std::string,std::vector< std::string > > > CollectionAd::getNodeStringValues (const std::string &attr_name)
/*
	main
*/
int main (int argc,char **argv)
{
	// short-option ASCII code
	int next_option = -1;
	// programme name
	char *prg_name = argv[0];
	// endpoint by option
	string *service =NULL;
	// endpoint by env variable
	char* serv_ev = NULL;
	// jdl file (path)
	string *jdl = NULL;
	// collection: path for jdl files
	string *collection_path = NULL;
	// delegation ID string
	string *del_id = NULL;
	// automatic delegation
	bool auto_del = false;
	// out result filepath
	string *fout =NULL;
	// verbose option
	bool verbose = false;
	// JDL strings
	string dag_jdl = "";
	string jdl_string = "";
	// jobid
	string id = "";
	// error message
	string errmsg = "";
        // final output message
	string outmsg = "";
	// job type
	JobType job_type = NORMAL;
	// context
	wmpapi::ConfigContext *cfs = NULL;
	// Ads
	requestad::Ad *ad  = NULL;
	requestad::ExpDagAd *dag = NULL;
        requestad::CollectionAd *collect = NULL;
	// Jobid struct (result of JobRegister)
	wmpapi::JobIdApi jobid ;
	try {
                // check options
                do {
                        // current option
                        next_option = getopt_long (argc,
                                                argv,
                                                short_options,
                                                long_options,
                                                NULL);
                        // options .....
                        switch(next_option) {
                                case SHORTOPT_SERVICE: {
                                        service = new string(optarg);
                                        break;
                                }
                                case SHORTOPT_JDL : {
                                        jdl = new string(optarg);
                                        break;
                                }
                                case SHORTOPT_COLLECTION : {
                                        collection_path = new string(optarg);
                                        break;
                                }
                                case SHORTOPT_DELEGATION: {
                                        del_id = new string(optarg);
                                        break;
                                }
                                case SHORTOPT_AUTODEL: {
                                        auto_del = true;
                                        break;
                                }
                                case SHORTOPT_OUTPUT: {
                                        fout = new string(optarg);
                                        break;
                                }
                                case SHORTOPT_VERBOSE: {
                                        verbose = true ;
                                        break;
                                }
                                case SHORTOPT_HELP : {
                                        long_usage (prg_name);
                                        break;
                                }
                                case -1: {
                                        break;
                                }
                                default:{
                                        short_usage (prg_name);
                                        break;
                                }
                        } // switch
                } while (next_option != -1);
                // checks if the mandatory attribute are missing ======
                // ENDPOINT
                if ( !service ){
                        serv_ev = getenv (GLITE_WMPROXY_ENDPOINT);
                        if ( ! serv_ev ){
                                fprintf (stderr, "error: unknown wmproxy endpoint URL\n");
                                fprintf (stderr, "set the %s environment variable\n",GLITE_WMPROXY_ENDPOINT );
                                fprintf(stderr, "or use the --%s <endpoint_URL> (-%c <endpoint_URL>) option\n",
                                                        LONGOPT_SERVICE, SHORTOPT_SERVICE);
                                short_usage(prg_name);
                        } else {
                                service = new string(serv_ev);
                        }
                }
                // verbose message
                if ( verbose ){
                	cout << "\n#### DEBUG #####\n";
                        cout << "\nSubmitting job to : " << *service << endl ;
                }
                // checks if jdl and collection options are not both set
                if (jdl != NULL && collection_path != NULL){
                        fprintf (stderr, "\nerror: --%s and --%s are incompatible (cannot be specified together)\n",
                                        LONGOPT_JDL, LONGOPT_COLLECTION);
                        fprintf (stderr, "use:\n- for normal and DAG jobs\n");
                        fprintf (stderr, "\t--%s  <JDL filepath> or -%c <JDL filepath>\n\n",
                                        LONGOPT_JDL, SHORTOPT_JDL);
                        fprintf (stderr, "- for collection of jobs\n");
                        fprintf (stderr, "\t--%s  <JDLs fpath> or -%c <JDLs path>\n\n",
                                        LONGOPT_COLLECTION, SHORTOPT_COLLECTION);
                        short_usage(prg_name);
                }
                // JDL
                if ( !jdl ){
                        // loads JDLs for a COLLECTION
                        if (collection_path){
                                job_type = COLLECTION;
                                // check the collection path specified by the arguments
                                try {
                                        // fs::path cp (exutils::normalize_path(*collection_path), fs::system_specific); boost 1.29.1
		                        fs::path cp (exutils::normalize_path(*collection_path), fs::native);
                                        if ( fs::is_directory( cp ) ) {
                                                *collection_path = exutils::addWildCards2Path(*collection_path, "*");
                                        }
                                        if (verbose){
                                        	cout << "\n#### DEBUG #####\n";
                                                cout << "\ncollection_path="<< *collection_path << "\n";
                                        }
                                }  catch ( const fs::filesystem_error & ex ){
                                                fprintf (stderr, "\nerror : invalid collection pathname");
                                                cerr  << flush << " (" << ex.what ( ) << ")\n" ;
                                                short_usage (prg_name);
                                }
                                // builds the JDL string
                                try {
                                        collect = requestad::AdConverter::createCollectionFromPath (*collection_path);
                                        // JDL string
                                        jdl_string = collect->toString( );
                                } catch ( exc::Exception &ex){
                                                fprintf (stderr, "\nError: invalid collection\n(unable to create the collection Ad from the file(s) into: \n%s)\n",collection_path->c_str() );
                                                cerr << "reason:" << flush << ex.what ( ) << endl ;
                                }
                        } else {
                                // error message (in case both jdl and collection options are missing)
                                fprintf (stderr, "\nError: JDL file pathname uknown; use:\n");
                                fprintf (stderr, "\t--%s  <JDL filepath> or -%c <JDL filepath>;\n",
                                        LONGOPT_JDL, SHORTOPT_JDL);
                                fprintf (stderr, "or for a set of JDL files representing a collection:\n");
                                fprintf (stderr, "\t--%s  <directory path> or -%c <directory>\n\n",
                                        LONGOPT_COLLECTION, SHORTOPT_COLLECTION);
                                short_usage(prg_name);
                        }
                } else {
                        // Loads the JDL from file
                        if ( ! apiutils::checkPathExistence ( jdl->c_str() )  ){
                                cerr << "\nerror: unable to find JDL file (no such file : " << *jdl << ")\n\n" << endl;
                                exit(-1);
                        } else{
                                if (verbose){
                                        cout << "JDL file is : " << *jdl << "\n";
                                }
                        }
                        // ClassAd
                        ad = new requestad::Ad( );
                        ad->fromFile (*jdl);
                        // DAG JOB
                        if ( ad->hasAttribute(requestad::JDL::TYPE , JDL_TYPE_DAG) ) {
                        		if ( verbose ) {
                                        	cout << "\n#### DEBUG #####\n";
                                                cout << "\nJobType : DAG " << endl ;
                                        }
                                        job_type = DAG ;
                                        dag = new requestad::ExpDagAd ( ad->toString() );

                                        // expands the DAG loading all JDL files
                                        dag->expand( );
                                        // JDL string for the DAG
                                        jdl_string = dag->toString() ;
                        } else  if ( ad->hasAttribute(requestad::JDL::TYPE , JDL_TYPE_COLLECTION) ) {
                                // in case the JDL file (-f option) is a collection
                                job_type = COLLECTION ;
                                try {
                                	 if ( verbose ) {
                                         	cout << "\n#### DEBUG #####\n";
                                                cout << "\nJobType : COLLECTION " << endl ;
                                        }
                                	classad::ClassAd * class_ad = ad->ad();
                                        if ( ! class_ad ){
						fprintf (stderr, "\nFatal Error: invalid JDL collection (couldn't convert Ad to ClassAd)\n");
                                                exit(-1);
                                        }

                                	collect = new requestad::CollectionAd( *class_ad );
                                        // JDL string
                                        jdl_string = collect->toString( );
                                } catch ( exc::Exception &ex){
                                                fprintf (stderr, "\nError: invalid JDL collection\n(unable to create the collection Ad from the file(s) into: \n%s)\n",collection_path->c_str() );
                                                cerr << "reason:" << flush << ex.what ( ) << endl ;
                                                exit(-1);
                                }
                        } else {
                                job_type = NORMAL ;
                                // JDL string (normal and DAG jobs)
                       		jdl_string = ad->toString( );
                        }
                 }


                //checks if el_id and auto-delegation options are not both set
                if (del_id && auto_del){
                        fprintf (stderr, "\nError: --%s and --%s are incompatible (cannot be specified together)\n",
                                LONGOPT_DELEGATION, LONGOPT_AUTODEL);
                        fprintf (stderr, "\t- to use a proxy previously delegated:\n");
                        fprintf (stderr, "\t\t--%s  <del-id> or -%c <del-id>\n\n",
                                        LONGOPT_DELEGATION, SHORTOPT_DELEGATION);
                        fprintf (stderr, "\t- to perform automatically new proxy delegation before the job submission:\n");
                        fprintf (stderr, "\t\t--%s  <JDLs fpath> or -%c <JDLs path>\n\n",
                                        LONGOPT_AUTODEL, SHORTOPT_AUTODEL);
                        short_usage(prg_name);
                }
                try {
                        // config context
                        cfs = new wmpapi::ConfigContext("", *service, "");
                        // if automatic delegation
                        if ( !del_id ){
                                if (auto_del) {
                                        del_id = exutils::makeAutomaticDelegation(cfs, verbose);
                                        if (!del_id) {
                                                fprintf (stderr, "\nError: unable to get the credential delegation-id\n");
                                                fprintf (stderr, "(an error occured during the proxy delegation operations)\n");
                                                exit(-1);
                                        }

                                } else {
                                        // error message (in case both jdl and collection options are missing)
                                        fprintf (stderr, "\n\Error: a mandatory option for delegation is missing\n");
                                        fprintf (stderr, "- to use a proxy previously delegated:\n");
                                        fprintf (stderr, "\t--%s  <delegation string> or -%c <delegation string>\n\n",
                                                        LONGOPT_DELEGATION, SHORTOPT_DELEGATION);
                                        fprintf (stderr, "- to perform automatically new proxy delegation before the job submission:\n");
                                        fprintf (stderr, "\t--%s  or -%c \n\n",
                                                        LONGOPT_AUTODEL, SHORTOPT_AUTODEL);
                                        short_usage(prg_name);
                                }
                        }
                        if (verbose){
                        	cout << "\n#### DEBUG #####\n";
                                cout << "\nthe delegation string is: " << *del_id << "\n";
                                cout << "################\n";
                        }


                        // verbose message
                        if ( verbose ) {
                        	cout << "\n#### DEBUG #####\n";
                                cout << "\nJDL=[" << jdl_string << "]\n";
                                cout << "\nRegisterring job ....  " << endl ;
                                cout << "################\n";
                        }
                        cout << "Connecting to the service at " << *service << "\n";
                        // register
                        jobid = wmpapi::jobRegister( jdl_string , *del_id, cfs);
                        // verbose message
                        if ( verbose ) {
                        	cout << "\n#### DEBUG #####\n";
                                cout << "The job has been successfully registered" << endl ;
                                cout << "################\n";
                        }
                        // JOBTYPE
                        switch (job_type) {
                                case (NORMAL) : {
                                        // displays the result message (saves it into a file if it is requested)
                                        id = submission(jobid, *service, ad, cfs, verbose );
                                        break;
                                }
                                case (DAG) : {
                                        // displays the result message(saves it into a file if it is requested)
                                       id = dag_submission(jobid, *service, job_type, dag, cfs, verbose );
                                        break;
                                }
                                case (COLLECTION) : {
                                	id = collection_submission(jobid, *service, job_type, collect, cfs, verbose );
                                	break;
                                }
                                default : {
                                        fprintf (stderr, "\n\nError: unknown job type (check the JDL)\n");
                                        exit(-1);
                                }
                        }

                        // START
                        if (verbose){
                                cout << "\n#### DEBUG #####\n";
                                cout << "Starting the job ...\n";
                                cout << "################\n";
                        }
                        wmpapi::jobStart(id, cfs);
                        if (verbose){
                                cout << "\n#### DEBUG #####\n";
                                cout << "jobStart : ok\n\n" ;
                                cout << "The job has been successfully submitted\n";
                                cout << "################\n";
                        }
                        outmsg = "****************************************************************************\n";
                        outmsg += "\t\t\tJOB SUBMIT OUTCOME\n\n";
                        outmsg += "The job has been successfully submitted to the EndPoint\n";
                        outmsg += "Your job identifier is:\n\n";
                        outmsg += id + "\n\n";
                        outmsg += "****************************************************************************\n";
                        // Displays the output message
                        cout << outmsg << "\n";
                        // saves the result
                        if (fout){
                                if ( exutils::saveToFile(*fout, outmsg) < 0 ){
                                        fprintf (stderr, "\nWarning: unable to write the result in %s\n", fout->c_str());
                                } else{
                                        fprintf (stdout, "\nInfo: This message has been saved in %s\n", fout->c_str() );
                                }
                        }


                } catch( wmpapi::BaseException &b_ex) {
                        fprintf ( stderr, "Unable to submit  the job(BaseException caught)\n" );
                        errmsg = exutils::handle_exception ( b_ex );
                        fprintf (stderr, "%s\n", errmsg.c_str() );
                        exit (-1);
                }
        } catch( exc::Exception &ex) {
                fprintf ( stderr, "\nerror : unable to submit the job (Exception caught)\n" );
		cerr << flush << ex.what ( ) << "\n";
                exit (-1);
        }
	return 0;
}
