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
// wmproxy api utilities
#include "glite/wms/wmproxyapi/wmproxy_api_utilities.h"
// CURL
#include <curl/curl.h>
// file-stat
#include <sys/stat.h>
#include <fcntl.h>


using namespace std;
using namespace glite::wms::wmproxyapi;
namespace exutils = glite::wms::wmproxyapi::examples::utilities;
namespace apiutils = glite::wms::wmproxyapiutils;
namespace exc = glite::wmsutils::exception;

// long option strings
static const char* LONGOPT_SERVICE = "endpoint" ;
static const char* LONGOPT_JOBID = "jobid";
static const char* LONGOPT_DIR = "dir";
static const char* LONGOPT_LIST = "listonly";
static const char* LONGOPT_VERBOSE = "verbose";
static const char* LONGOPT_HELP = "help";
// short options chars
static const char SHORTOPT_SERVICE = 'e';
static const char SHORTOPT_JOBID = 'j';
static const char SHORTOPT_DIR = 'd';
static const char SHORTOPT_LIST = 'l';
static const char SHORTOPT_VERBOSE = 'v';
static const char SHORTOPT_HELP = '?';

// long options
static const struct option long_options[] = {
    { LONGOPT_SERVICE,              required_argument,	NULL, SHORTOPT_SERVICE},
    { LONGOPT_JOBID,			required_argument,	NULL, SHORTOPT_JOBID },
    { LONGOPT_DIR,			required_argument,	NULL, SHORTOPT_DIR},
    { LONGOPT_LIST,			no_argument , 		NULL, SHORTOPT_LIST},
    { LONGOPT_VERBOSE,		no_argument ,		NULL, SHORTOPT_VERBOSE},
    { LONGOPT_HELP,			no_argument ,		NULL, SHORTOPT_HELP},
    {0, 0, 0, 0}
};

// short options
static const char* const short_options = "e:j:o:lv?";

/*
* 	short usage help
*/
static void short_usage(const char *exe)
{
	fprintf(stderr,"\nUsage: %s arguments [options]\n",exe);
	fprintf(stderr,"where \n");
	fprintf(stderr,"arguments (mandatory):\n");
	// jobid (mandatory)
	fprintf(stderr,"--%s, -%c	\t\t<jobid string> | <jobid file>\n",
		LONGOPT_JOBID , SHORTOPT_JOBID);
	fprintf(stderr,"\noptions (not mandatory):\n\n");
	// enpoint
	fprintf(stderr,"\t--%s, -%c\t\t<endpoint URL> (mandatory if %s is not set)\n",
		LONGOPT_SERVICE , SHORTOPT_SERVICE, GLITE_WMPROXY_ENDPOINT);
	// output directory
	fprintf(stderr,"--%s, -%c	\t\t<output directory>\n",
		LONGOPT_DIR , SHORTOPT_DIR);
	// only-listing
	fprintf(stderr,"--%s, -%c	\n",
		LONGOPT_LIST , SHORTOPT_LIST);
	// verbosity
	fprintf(stderr,"\t--%s -%c\n",
		LONGOPT_VERBOSE , SHORTOPT_VERBOSE);
	// help
	fprintf(stderr,"--%s, -%c	\n",
		LONGOPT_HELP , SHORTOPT_HELP);

	fprintf(stderr,"\nWMProxy Testing User Interface\n");
	fprintf(stderr,"Copyright (C) 2005 by DATAMAT SpA\n\n");
	fprintf(stderr, "Please report any bug at:\n\t\tfabrizio.pacini@datamat.it\n");

	exit (-1);
}
/*
	long usage help
*/
static void long_usage(const char *exe)
{
	fprintf(stderr,"\nUsage: %s arguments\n",exe);
	fprintf(stderr,"where  \n");
	fprintf(stderr,"arguments (mandatory):\n");
	// jobid (mandatory)
	fprintf(stderr, "\t-%c jobid_string | jobid_file", SHORTOPT_JOBID);
	fprintf(stderr, "\t--%s jobid_string | jobid_file\n", LONGOPT_JOBID);

	fprintf(stderr,"\noptions (not mandatory):\n\n");
	// endpoint
	fprintf(stderr, "\t-%c url\n", SHORTOPT_SERVICE);
	fprintf(stderr, "\t--%s url\n", LONGOPT_SERVICE);
	fprintf(stderr, "\twmproxy endpoint URL, overriding any setting of %s;\n",
		GLITE_WMPROXY_ENDPOINT);
	fprintf(stderr, "\tmandatory if the %s is not set.\n", GLITE_WMPROXY_ENDPOINT);
	fprintf(stderr, "\t(e.g.: https://<server>:<port>/<wmProxy-fcgi>)\n\n");
	// only-listing
	fprintf(stderr, "\t-%c\n", SHORTOPT_LIST);
	fprintf(stderr, "\t--%s\n", LONGOPT_LIST);
	fprintf(stderr, "\the job output files will be only listed.\n");
	// ouput directory
	fprintf(stderr, "\t-%c out_dir\n", SHORTOPT_DIR);
	fprintf(stderr, "\t--%s out_dir\n", LONGOPT_DIR);
  	fprintf(stderr, "\tthe directory of the local machine where the output file will be downloaded\n\n");
	// only-listing
	fprintf(stderr, "\t-%c\n", SHORTOPT_LIST);
	fprintf(stderr, "\t--%s\n", LONGOPT_LIST);
  	fprintf(stderr, "\tonly the listening (the files are not downloaded on the local machine)\n\n");
	// verbosity
	fprintf(stderr, "\t-%c\n", SHORTOPT_VERBOSE);
	fprintf(stderr, "\t--%s\n", LONGOPT_VERBOSE);
	fprintf(stderr, "\tEnables verbose output\n\n");
	// help
	fprintf(stderr, "\t--help, -?\n");
	fprintf(stderr, "\tdisplays this message\n\n");

	fprintf(stderr,"\nWMProxy Testing User Interface\n");
	fprintf(stderr,"Copyright (C) 2005 by DATAMAT SpA\n\n");
	fprintf(stderr, "Please report any bug at:\n\t\tfabrizio.pacini@datamat.it\n");

	exit (-1);
}
/*
* displays the file listing
*/
void listResult(vector <pair<std::string , long> > &vect, const string jobid )
{
	vector <pair<string , long> >::iterator it ;
	string name = "";
	long size = 0;
	char s_size[1024] = "";
	// output message
	string outmsg = "*************************************************************\n";
	outmsg += "OUTPUT FILE LIST for the job :\n\n";
	outmsg += "jobid:\n\t" + jobid + "\n\n";
	if ( vect.empty() ){
		outmsg += "no files for the job\n";
	} else {
		for ( it = vect.begin() ; it != vect.end() ; it++ ){
			name = it->first ;
			sprintf (s_size, "%lx", size);
			outmsg += " - " + name + "\t\tsize:\t [ " + s_size + " ]\n" ;
		}
	}

	outmsg += "\n*************************************************************\n";
	// prints the output message
	fprintf (stdout, "\n%s", outmsg.c_str());
}
/*
	struct for files
*/
struct httpfile { char *filename; FILE* stream; } ;
/*
* writing callback for curl operations
*/
int storegprbody(void *buffer, size_t size, size_t nmemb, void *stream)
 {
	struct httpfile *out=(struct httpfile*)stream;
	if(out && !out->stream) {
		// open stream
		out->stream=fopen(out->filename, "wb");
		if(!out->stream) {
			return -1;
		}
   	}
  	return fwrite(buffer, size, nmemb, out->stream);
 }
/*
*	file downloading
*/
void getFiles (vector <pair<std::string , long> > &vect, string* dirout, ConfigContext *cfs, bool verbose  )
{
	CURL *curl = NULL;
	vector <pair<std::string , long> >::iterator it ;
	string source = "" ;
	string destination = "" ;
	string file = "" ;
	CURLcode res;
	char *proxy = NULL;
        char *tcert = NULL;
	unsigned int p = 0;
	// user proxy
	proxy = (char*)apiutils::getProxyFile(cfs);
        if (!proxy){
		cerr << "\n error: unable to find a valid proxy to transfer the local InputSandox files to the DestinationURI(s)\n";
                exit(-1);
        }
        // trusted certificate pathname
	tcert = (char*)apiutils::getTrustedCert(cfs);
        if (!tcert){
		cerr << "\n error: no valid location of trusted certificates found\n";
                exit(-1);
        }
	// curl init
	curl_global_init(CURL_GLOBAL_ALL);
 	curl = curl_easy_init();
	if ( curl ) {
		// writing function
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, storegprbody);
		// user proxy
		curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE,  "PEM");
		curl_easy_setopt(curl, CURLOPT_SSLCERT,	proxy);
		curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE,   "PEM");
		curl_easy_setopt(curl, CURLOPT_SSLKEY, proxy);
		curl_easy_setopt(curl, CURLOPT_SSLKEYPASSWD, NULL);
		//trusted certificate directory
		curl_easy_setopt(curl, CURLOPT_CAPATH, tcert);
		//ssl options (no verify)
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		// verbose message
		if ( verbose ){
			cout << "file transferring ....." << endl;
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
			cout << "user proxy : " << proxy << endl;
		}
		// files to be downloaded
		for ( it = vect.begin( ) ; it != vect.end( ) ; it++ ){
			source = it->first ;
			curl_easy_setopt(curl, CURLOPT_URL, source.c_str());
			p = source.find_last_of("/");
			if ( p != string::npos ){
				destination = source.substr( p+1 , source.size( ) ) ;
			}
			if ( dirout ){
				destination = *dirout + "/" + destination ;
			}
			if ( verbose ){
				cout << "source : " << source << "\n";
				cout << "destination : " << destination<< "\n";
			}
			struct httpfile params={
				(char*)destination.c_str() ,
 				NULL
  			};
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &params);
			// downloading
			res = curl_easy_perform(curl);
			if (res<0){
				cerr << "error: failed to perform the downloading of the file" << endl;
				cerr << "source : " << source << "\n";
				cerr << "destination : " << destination<< "\n";
				exit(-1);
			}
		}
	}
}
/*
	main
*/
int main (int argc,char **argv)
{
	// options
	int next_option = -1;
	// programme name
	char *prg_name = argv[0];
	// endpoint from env variable
	char *serv_ev = NULL;
	// endpoint
	string *service = NULL;
	// jobid
	string *jobid = NULL;
	// output directory
	string *dir_out = NULL;
	//error message
	string errmsg = "";
	// flags
	bool list_only = false;
	bool verbose = false;
	// context
	ConfigContext *cfs = NULL;
	// file to be downloaded
	vector <pair<std::string , long> > files ;
	// check options
        try {
                do {

                        next_option = getopt_long(argc,
                                                argv,
                                                short_options,
                                                long_options,
                                                NULL);
                        switch(next_option) {
                                case SHORTOPT_SERVICE: {
                                        service = new string(optarg);
                                        //cout << "debug - service=[" << *service << "]" << endl;
                                        break;
                                }
                                case SHORTOPT_JOBID: {
                                        jobid = new string(optarg);
                                        //cout << "debug - jobid=[" << *jobid << "]" << endl;
                                        break;
                                }
                                case SHORTOPT_DIR: {
                                        dir_out= new string(optarg);
                                        //cout << "debug - out=[" << *fout << "]" << endl;
                                        break;
                                }
                                case SHORTOPT_LIST: {
                                        list_only = true;
                                        //cout << "debug - out=[" << *fout << "]" << endl;
                                        break;
                                }
                                case SHORTOPT_VERBOSE: {
                                        verbose = true ;
                                        break;
                                }
                                case SHORTOPT_HELP: {
                                        long_usage (prg_name);
                                        break;
                                }
                                case -1: {cout << "-1\n";
                                        break;
                                }
                                default:{cout << "default\n";
                                        short_usage (prg_name);
                                        break;
                                }
                        } // switch
                } while (next_option != -1);
                // service
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
                // jobid
                if ( !jobid ){
                        errmsg = errmsg.assign("--").append(LONGOPT_JOBID).append(" <job identifier>");
                        fprintf (stderr, "error: the following mandatory option is missing:\n%s\n", errmsg.c_str() );
                        short_usage(prg_name);
                } else {
                        // checks if the jobid string is a path to file
                        string * j_id =  exutils::jobidFromFile ( *jobid );
                        if ( j_id ) {
                                jobid = new string ( *j_id ) ;
                        }
                }
                if ( verbose ){
                        cout << "\nGetting job output files list ...." << endl ;
                        cout << "jobid : " << *jobid << endl;
                        cout << "endpoint :  " << *service << endl ;
                }
                try {
                        // config context
                        cfs = new ConfigContext("", service->c_str(), "");
                        // start
                        files = getOutputFileList (*jobid, cfs);
                        if ( verbose ) {
                                cout << "\nInput Sandbox file list : successfully received" << endl;
                        }
                        // displays result message (in case it is saved into the user file)
                        listResult( files, *jobid );
                        // downloading
                        if ( ! list_only ) {
                                getFiles (files, dir_out, cfs, verbose) ;
                        }

                } catch( BaseException &b_ex) {
                        fprintf ( stderr, "Unable to retrieve the output job file list for the job:\n%s\n\nException caught:\n", jobid->c_str());
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
