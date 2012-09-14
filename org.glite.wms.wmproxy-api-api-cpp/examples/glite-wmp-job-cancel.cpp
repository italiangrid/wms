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

using namespace std;
using namespace glite::wms::wmproxyapi;
using namespace glite::wms::wmproxyapi::examples::utilities;
namespace exc = glite::wmsutils::exception;


// long option strings
static const char* LONGOPT_SERVICE = "endpoint" ;
static const char* LONGOPT_JOBID = "jobid";
static const char* LONGOPT_OUTPUT = "output";
static const char* LONGOPT_HELP = "help";
// short options chars
static const char SHORTOPT_SERVICE = 'e';
static const char SHORTOPT_JOBID = 'j';
static const char SHORTOPT_OUTPUT= 'o';
static const char SHORTOPT_HELP = '?';

// long options
static const struct option long_options[] = {
    { LONGOPT_SERVICE,              1,NULL, SHORTOPT_SERVICE},
    { LONGOPT_JOBID,			1,NULL, SHORTOPT_JOBID },
    { LONGOPT_OUTPUT,		1,NULL, SHORTOPT_OUTPUT},
    { LONGOPT_HELP,			0,NULL, SHORTOPT_HELP},
    {0, 0, 0, 0}
};
// short options
static const char* const short_options = "e:j:o:?";

/*
* 	short usage help
*/
static void short_usage(const char *exe)
{
	fprintf(stderr,"\nUsage: %s arguments [options]\n",exe);
	fprintf(stderr,"where \n");
	fprintf(stderr,"arguments (mandatory):\n");

	fprintf(stderr,"\t--%s, -%c	\t<jobid string> | <jobid file>\n",
		LONGOPT_JOBID , SHORTOPT_JOBID);

	fprintf(stderr,"\noptions (not mandatory):\n\n");

	fprintf(stderr,"\t--%s, -%c\t<endpoint URL> (mandatory if %s is not set)\n",
		LONGOPT_SERVICE , SHORTOPT_SERVICE, GLITE_WMPROXY_ENDPOINT);

	fprintf(stderr,"\t--%s, -%c	\n",
		LONGOPT_HELP , SHORTOPT_HELP);

	fprintf(stderr,"\nWMProxy Testing User Interface\n");
	fprintf(stderr,"Copyright (C) 2005 by DATAMAT SpA\n\n");
	fprintf(stderr, "Please report any bug at:\n\t\tfabrizio.pacini@datamat.it\n\n");

	exit (-1);
}
/*
* 	long usage help
*/
static void long_usage(const char *exe)
{
	fprintf(stderr,"\nUsage: %s arguments\n",exe);
	fprintf(stderr,"where  \n");
	fprintf(stderr,"arguments (mandatory):\n");
	// jobid
	fprintf(stderr, "\t-%c jobid_string | jobid_file", SHORTOPT_JOBID);
	fprintf(stderr, "\t--%s jobid_string | jobid_file\n", LONGOPT_JOBID);
	fprintf(stderr, "\tjob identifier string returned by job-submit.\n");
	fprintf(stderr, "\tIt is possible to specify directly the jobid string or the jobid_file containing the jobid_string\n");
	fprintf(stderr, "\t(a simple name or an absolute path on the local machine)\n");

	fprintf(stderr,"\noptions (not mandatory):\n\n");
	// endpoint
	fprintf(stderr, "\t-%c url\n", SHORTOPT_SERVICE);
	fprintf(stderr, "\t--%s url\n", LONGOPT_SERVICE);
	fprintf(stderr, "\twmproxy endpoint URL, overriding any setting of %s;\n",
		GLITE_WMPROXY_ENDPOINT);
	fprintf(stderr, "\tmandatory if the %s is not set.\n", GLITE_WMPROXY_ENDPOINT);
	fprintf(stderr, "\t(e.g.: https://<server>:<port>/<wmProxy-fcgi>)\n\n");
	// result file
	fprintf(stderr, "\t-%c out_file\n", SHORTOPT_OUTPUT);
	fprintf(stderr, "\t--%s out_file\n", LONGOPT_OUTPUT);
  	fprintf(stderr, "\twrites the result message in the file specified by out_file.\n\tout_file can be either a simple name or an absolute path (on the submitting machine).\n\n");
	// help
	fprintf(stderr, "\t--help, -?\n");
	fprintf(stderr, "\tdisplays this message\n\n");
	fprintf(stderr,"\nWMProxy Testing User Interface\n");
	fprintf(stderr,"Copyright (C) 2005 by DATAMAT SpA\n\n");
	fprintf(stderr, "Please report any bug at:\n\t\tfabrizio.pacini@datamat.it\n\n");

	exit (-1);
}

/*
*	handles the result messages
*
*/
void getOperationResult(const string j_id, const string endpoint, const string *fout )
{
	// output message
	string outmsg = "*************************************************************\n";
	outmsg += "CANCEL OPERATION :\n\n\n";
	outmsg += "your job has been successfully cancelled by the end point:\n\n";
	outmsg += "\t" + endpoint + "\n\n";
	outmsg += "\tjobid:\n\t" + j_id + "\n\n";
	outmsg += "*************************************************************\n";
	// prints the message
	fprintf (stdout, "\n%s", outmsg.c_str());
	// save the message into a file (if it requested)
	if (fout){
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
	// read options
	int next_option = -1;
	// endpoint by options
	string *service = NULL;
	// endpoint by env var
	char* serv_ev = NULL;
	// job identifier
	string *jobid = NULL;
	// output filepath
	string *fout = NULL;
	// error messages
	string errmsg = "";
	// context
	ConfigContext *cfs = NULL;
	// excutable name
 	char *prg_name = argv[0];
        try {
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
                                case SHORTOPT_JOBID: {
                                        jobid = new string(optarg);
                                        //cout << "debug - jobid=[" << *jobid << "]" << endl;
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
                // endpoint
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
                        string * j_id =  jobidFromFile ( *jobid );
                        if ( j_id ) {
                                jobid = new string ( *j_id ) ;
                        }
                }
                //cout << "debug - jobid=[" << *jobid << "]" << endl;
                try {
                        // config context
                        cfs = new ConfigContext("", service->c_str(),"");
                        // performs cancelling of the job
                        jobCancel(*jobid, cfs);
                        // displays result message (in case it is saved into the user file)
                        getOperationResult( *jobid, *service, fout );
                } catch( BaseException &b_ex) {
                        fprintf ( stderr, "Unable to cancel the job:\n%s\n\nException caught:\n", jobid->c_str());
                        errmsg = handle_exception ( b_ex );
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
