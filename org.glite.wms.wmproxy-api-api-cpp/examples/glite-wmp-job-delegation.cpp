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

#include "wmp_job_examples.h"
#include "glite/wms/wmproxyapi/wmproxy_api_utilities.h"

using namespace std;
using namespace glite::wms::wmproxyapi;
using namespace glite::wms::wmproxyapi::examples::utilities;
using namespace glite::wms::wmproxyapiutils;

static const char* LONGOPT_SERVICE = "endpoint" ;
static const char* LONGOPT_DELEGATION = "delegation";
static const char* LONGOPT_OUTPUT = "output";
static const char* LONGOPT_HELP = "help";

static const char SHORTOPT_SERVICE = 'e';
static const char SHORTOPT_DELEGATION = 'd';
static const char SHORTOPT_OUTPUT = 'o';
static const char SHORTOPT_HELP = '?';
/*
	long options
*/

static const struct option long_options[] = {
    { LONGOPT_SERVICE,              1,NULL, SHORTOPT_SERVICE},
    { LONGOPT_DELEGATION,	1,NULL, SHORTOPT_DELEGATION },
    { LONGOPT_OUTPUT,		1,NULL, SHORTOPT_OUTPUT},
    { LONGOPT_HELP,			0,NULL, SHORTOPT_HELP},
    {0, 0, 0, 0}
};

/*
	short options
*/
//static const char* const short_options = "u:a:d:c:n:v:f:h";
static const char* const short_options = "e:d:o:?";

static void short_usage(const char *exe)
{
	fprintf(stderr,"\nUsage: %s arguments [options]\n",exe);
	fprintf(stderr,"where  \n");
	fprintf(stderr,"arguments (mandatory):\n");
	fprintf(stderr,"\t--%s, -%c \t\t<delegation id string>\n",
		LONGOPT_DELEGATION, SHORTOPT_DELEGATION);

	fprintf(stderr,"\noptions (not mandatory):\n\n");

	fprintf(stderr,"\t--%s, -%c\t<endpoint URL> (mandatory if %s is not set)\n",
		LONGOPT_SERVICE , SHORTOPT_SERVICE, GLITE_WMPROXY_ENDPOINT);

	fprintf(stderr,"\t--%s, -%c\t\t<delegation string> (mandatory if %s is not set)\n",
		LONGOPT_SERVICE , SHORTOPT_SERVICE, GLITE_WMPROXY_ENDPOINT);
	fprintf(stderr,"\t--%s, -%c\t\t<output file>\n",
		LONGOPT_OUTPUT , SHORTOPT_OUTPUT);

	fprintf(stderr,"\t--%s, -%c\n",
		LONGOPT_HELP , SHORTOPT_HELP);

	fprintf(stderr,"\nWMProxy Testing User Interface\n");
	fprintf(stderr,"Copyright (C) 2005 by DATAMAT SpA\n\n");
	fprintf(stderr, "Please report any bug at:\n\t\tfabrizio.pacini@datamat.it\n\n");

	exit (-1);
}
/*
	usage message
*/
static void long_usage(const char *exe)
{
	fprintf(stderr,"\nUsage: %s arguments\n",exe);
	fprintf(stderr,"where  \n");
	fprintf(stderr,"arguments (mandatory):\n");

	fprintf(stderr, "\t-%c del_string",SHORTOPT_DELEGATION);
	fprintf(stderr, "\t--%s del_string\n",LONGOPT_DELEGATION);
	fprintf(stderr, "\tuser delegation-id string\n");

	fprintf(stderr,"\noptions (not mandatory):\n\n");

	fprintf(stderr, "\t-%c url\n", SHORTOPT_SERVICE);
	fprintf(stderr, "\t--%s url\n", LONGOPT_SERVICE);
	fprintf(stderr, "\twmproxy endpoint URL, overriding any setting of %s;\n",
		GLITE_WMPROXY_ENDPOINT);
	fprintf(stderr, "\tmandatory if the %s is not set.\n", GLITE_WMPROXY_ENDPOINT);
	fprintf(stderr, "\t(e.g.: https://<server>:<port>/<wmProxy-fcgi>)\n\n");

	fprintf(stderr, "\t-%c out_file\n", SHORTOPT_OUTPUT);
	fprintf(stderr, "\t--%s out_file\n", LONGOPT_OUTPUT);
  	fprintf(stderr, "\twrites the result message in the file specified by out_file.\n\tout_file can be either a simple name or an absolute path (on the submitting machine).\n\n");

	fprintf(stderr, "\t--help, -?\n");
	fprintf(stderr, "\tdisplays this message\n\n");
	fprintf(stderr,"\nWMProxy Testing User Interface\n");
	fprintf(stderr,"Copyright (C) 2005 by DATAMAT SpA\n\n");
	fprintf(stderr, "Please report any bug at:\n\t\tfabrizio.pacini@datamat.it\n\n");

	exit (-1);
}

void getOperationResult(const string del_id, const string endpoint, const string *fout, const string proxy )
{
	string outmsg = "*************************************************************\n";
	outmsg += "DELEGATION OPERATION :\n\n\n";
	outmsg += "your proxy has been successfully stored in the end point:\n\n";
	outmsg += "\t" + endpoint + "\n\n";
	outmsg += "with the delegation identifier :\t" + del_id + "\n\n";
	outmsg += "*************************************************************\n";

	fprintf (stdout, "\n\n%s", outmsg.c_str());

	// saves the result
	if (fout){
		outmsg += "\tproxy:\n\n";
		outmsg += proxy;

		if ( saveToFile(*fout, outmsg) < 0 ){
			fprintf (stderr, "\nwarning: unable to write the result in %s\n", fout->c_str());
		} else{
			fprintf (stdout, "\nThis message has been saved in %s\n", fout->c_str() );
		}
	}
}
/*
	main
*/
int main (int argc,char **argv)
{

	int next_option = -1;
 	char *prg_name = argv[0];
	string *service = NULL;
	char* serv_ev = NULL;
	string *del_id = NULL;
	string *fout = NULL ;
	string proxy = "";
	string errmsg = "";

	ConfigContext *cfs = NULL;

	// check options
	do {
		next_option = getopt_long (argc,
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
			case SHORTOPT_DELEGATION: {
				del_id = new string(optarg);
				//cout << "debug - del_id=[" << *del_id << "]" << endl;
				break;
			}
			case SHORTOPT_OUTPUT: {
				fout = new string(optarg);
				//cout << "debug - out=[" << *fout << "]" << endl;
				break;
			}
			case SHORTOPT_HELP: {
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

	// checks if the mandatory attribute are missing
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

	// delegation id
	if ( !del_id ){
		errmsg = errmsg.assign("--").append(LONGOPT_DELEGATION).append(" <delegation identifier>");
		fprintf (stderr, "error: the following mandatory option is missing:\n%s\n", errmsg.c_str() );
		short_usage(prg_name);
	}

	try {
		// init context
		string trustedCerts= getTrustedCert();
		cfs = new ConfigContext("", *service, trustedCerts);

		// gets a proxy
		proxy = getProxyReq(*del_id, cfs) ;

		// sends the proxy to the endpoint service
		putProxy(*del_id, proxy, cfs);

	} catch( BaseException &b_ex) {
		fprintf ( stderr, "Unable to delegate credential\n\nException caught:\n" );
		errmsg = handle_exception (b_ex);
		fprintf (stderr, "%s\n", errmsg.c_str() );
		exit (-1);
        }

	getOperationResult(*del_id, *service, fout, proxy );

	return 0;
}
