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
//AD
#include "glite/jdl/ExpDagAd.h"
#include "glite/jdl/JobAd.h"
#include "glite/jdl/RequestAdExceptions.h"
// JDL
#include "glite/jdl/adconverter.h"
#include "glite/jdl/jdl_attributes.h"
#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/extractfiles.h"
#include "glite/jdl/adconverter.h"
// wmproxy api utilities
#include "glite/wms/wmproxyapi/wmproxy_api_utilities.h"

using namespace std;
namespace requestad = glite::jdl;
namespace exc = glite::wmsutils::exception;
namespace wmpapi = glite::wms::wmproxyapi;
namespace apiutils = glite::wms::wmproxyapiutils;
namespace exutils = glite::wms::wmproxyapi::examples::utilities;

// long option strings
static const char* LONGOPT_SERVICE = "endpoint" ;
static const char* LONGOPT_JDL= "jdl-file" ;
static const char* LONGOPT_COLLECTION = "collection" ;
static const char* LONGOPT_DELEGATION = "delegation";
static const char* LONGOPT_OUTPUT = "output";
static const char* LONGOPT_HELP = "help";
// short options chars
static const char SHORTOPT_SERVICE = 'e';
static const char SHORTOPT_JDL = 'f';
static const char SHORTOPT_COLLECTION = 'c' ;
static const char SHORTOPT_DELEGATION = 'd';
static const char SHORTOPT_OUTPUT = 'o';
static const char SHORTOPT_HELP = '?';

/*
	long options
*/
static const struct option long_options[] = {
    { LONGOPT_SERVICE,              1,NULL, SHORTOPT_SERVICE},
    { LONGOPT_JDL,           		1,NULL, SHORTOPT_JDL},
    { LONGOPT_COLLECTION,       1,NULL, SHORTOPT_COLLECTION},
    { LONGOPT_DELEGATION,	1,NULL, SHORTOPT_DELEGATION },
    { LONGOPT_OUTPUT,		1,NULL, SHORTOPT_OUTPUT},
    { LONGOPT_HELP,			0,NULL, SHORTOPT_HELP},
    {0, 0, 0, 0}
};

/*
	short options
*/
static const char* const short_options = "e:f:d:o:?";

/*
* 	short usage help
*/
static void short_usage(const char *exe)
{
	fprintf(stderr,"\nUsage: %s arguments [options] \n",exe);
	fprintf(stderr,"where  \n");
	fprintf(stderr,"arguments (mandatory):\n");

	fprintf(stderr,"\t--%s, -%c	 <delegation string>\n",
		LONGOPT_DELEGATION , SHORTOPT_DELEGATION);

	// jdl
	fprintf(stderr,"jdl_options (one of these two options is mandatory):\n");
	fprintf(stderr,"\t--%s, -%c	 <JDL filepath>\t(for normal and DAG jobs)\n",
		LONGOPT_JDL , SHORTOPT_JDL);

	fprintf(stderr,"\t\tor\n");
	// JDL for collection
	fprintf(stderr,"\t--%s, -%c	 <collection of JDLs path>\t(for collection of jobs)\n",
		LONGOPT_COLLECTION , SHORTOPT_COLLECTION);

	fprintf(stderr,"\noptions (not mandatory):\n\n");

	fprintf(stderr,"\t--%s, -%c		<endpoint URL> (mandatory if the %s is not set)\n",
		LONGOPT_SERVICE , SHORTOPT_SERVICE, GLITE_WMPROXY_ENDPOINT);

	fprintf(stderr,"\t--%s -%c		<output file>\n",
		LONGOPT_OUTPUT , SHORTOPT_OUTPUT);
	fprintf(stderr,"\noptions (not mandatory):\n\n");

	fprintf(stderr,"\nWMProxy Testing User Interface\n");
	fprintf(stderr,"Copyright (C) 2005 by DATAMAT SpA\n\n");
	fprintf(stderr, "Please report any bug at:\n\t\tfabrizio.pacini@datamat.it\n");

	exit (-1);
}
/*
	long usage help message
*/
static void long_usage(const char *exe)
{
	fprintf(stderr,"\nUsage: %s arguments [options] \n",exe);
	fprintf(stderr,"where  \n");
	fprintf(stderr,"arguments (mandatory):\n");
	/// jdl file or collection
	fprintf(stderr,"jdl_options (one of these two options is mandatory):\n");
	fprintf(stderr, "\t-%c <collection of JDLs path>\n", SHORTOPT_COLLECTION);
	fprintf(stderr, "\t--%s <collection of JDLs path>\n",LONGOPT_COLLECTION);
	fprintf(stderr, "\tthe file containing the JDL describing the jobs to be submitted.\n");
	fprintf(stderr, "\tfile_path can be either a simple name or an absolute path (on the submitting machine).\n\n");
	fprintf(stderr,"\t\tor\n");
	fprintf(stderr, "\t-%c file_path\n", SHORTOPT_JDL);
	fprintf(stderr, "\t--%s file_path\n",LONGOPT_JDL);
	fprintf(stderr, "\tthe file containing the JDL describing the job to be submitted.\n\tfile_path can be either a simple name or an absolute path (on the submitting machine).\n\n");
	// delegation string
	fprintf(stderr, "\t-%c del_string",SHORTOPT_DELEGATION);
	fprintf(stderr, "\t--%s del_string\n",LONGOPT_DELEGATION);
	fprintf(stderr, "\tuser delegation-id string\n\n");

	fprintf(stderr,"options (not mandatory):\n");
	// endpoint
	fprintf(stderr, "\t-%c url\n", SHORTOPT_SERVICE);
	fprintf(stderr, "\t--%s url\n", LONGOPT_SERVICE);
	fprintf(stderr, "\twmproxy endpoint URL, overriding any setting of %s;\n",
		GLITE_WMPROXY_ENDPOINT);
	fprintf(stderr, "\tmandatory if the %s is not set.\n", GLITE_WMPROXY_ENDPOINT);
	fprintf(stderr, "\t(e.g.: https://<server>:<port>/<wmProxy-fcgi>)\n\n");
	// output result
	fprintf(stderr, "\t-%c out_file\n", SHORTOPT_OUTPUT);
	fprintf(stderr, "\t--%s out_file\n", LONGOPT_OUTPUT);
  	fprintf(stderr, "\twrites the generated edg_jobId assigned to the submitted job in the file specified by out_file.\n\tout_file can be either a simple name or an absolute path (on the submitting machine).\n\n");
	// help
	fprintf(stderr, "\t--help, -?\n");
	fprintf(stderr, "\tdisplays this message\n\n");
	fprintf(stderr,"\nWMProxy Testing User Interface\n");
	fprintf(stderr,"Copyright (C) 2005 by DATAMAT SpA\n\n");
	fprintf(stderr, "Please report any bug at:\n\t\tfabrizio.pacini@datamat.it\n");

	exit (-1);
}
string getInputFilesList (const string &jobid,requestad::Ad *ad, wmpapi::ConfigContext *cfs  )
{
	vector<string> paths;
	vector<pair<string, string> > to_bcopied;
        vector<pair<string, string> >::iterator it;
        string *dest_uri = NULL;
        string msg = "";
        if (!ad){
		fprintf (stderr, "fatal error(getInputFileList): unable to read the Job Description\n");
		exit(-1);
	}
	 if (ad->hasAttribute(requestad::JDL::INPUTSB)){
        	// gets the InputSandbox files
		paths = ad->getStringValue  (requestad::JDL::INPUTSB) ;

		// http destination URI
		dest_uri = exutils::getInputSBDestURI(jobid, cfs);
		if (!dest_uri){
			fprintf (stderr, "\nerror: unable to retrieve the InputSandbox destination URI\n");
			fprintf (stderr, "jobid: %s", jobid.c_str() );
			exit(-1);
		}
		// gets the InputSandbox files
		paths = ad->getStringValue  (requestad::JDL::INPUTSB) ;
		// gets the InputSandbox files to be transferred to the destinationURI's
		requestad::toBcopied(requestad::JDL::INPUTSB, paths, to_bcopied, *dest_uri,"");
		if ( ! paths.empty( ) ){
		 	msg = "List of the InputSandbox files to be transferred :\n";
                        msg += "------------------------------------------------------\n";
                        for ( it = to_bcopied.begin( ) ; it != to_bcopied.end( ) ; it++ ){
				msg += "File :		" + it->first + "\n";
                                msg += "DestinationURI : " + it->second + "\n";
                                msg += "------------------------------------------------------\n";
                        }
                }
	 }
         return msg ;
  }
string getDAGInputFilesList (const string &jobid, const string &node, requestad::ExpDagAd *dag, wmpapi::ConfigContext *cfs  )
{
       	vector<string> paths;
	vector<pair<string, string> > to_bcopied;
        vector<pair<string, string> >::iterator it;
        string *dest_uri = NULL;
        string msg = "";

         if (!dag){
		fprintf (stderr, "fatal error(getDAGInputFileList): unable to read the Job Description\n");
		exit(-1);
	}
        try{
        	// gets the InputSandbox files
		paths = dag->getNodeStringValue(node, requestad::JDL::INPUTSB);
		// http destination URI
		dest_uri = exutils::getInputSBDestURI(jobid, cfs);
		if (!dest_uri){
			fprintf (stderr, "\nerror: unable to retrieve the InputSandbox destination URI\n");
			fprintf (stderr, "jobid: %s", jobid.c_str() );
			exit(-1);
		}
		// gets the InputSandbox files
		paths = dag->getNodeStringValue(node, requestad::JDL::INPUTSB);
		// gets the InputSandbox files to be transferred to the destinationURI's
		requestad::toBcopied(requestad::JDL::INPUTSB, paths, to_bcopied, *dest_uri,"");
                // message
		if ( ! paths.empty( ) ){
		 	msg = "List of the InputSandbox files to be transferred :\n";
                        msg += "------------------------------------------------------\n";
                        for ( it = to_bcopied.begin( ) ; it != to_bcopied.end( ) ; it++ ){
				msg += "File :		" + it->first + "\n";
                                msg += "DestinationURI : " + it->second + "\n";
                                msg += "------------------------------------------------------\n";
                        }
                }

         } catch (requestad::AdEmptyException &exc){
		// exception caught if the node doesn't contain the InputSB attribute
                msg = "";
         }
         return msg;
}

/*
*	result message
*/
void getOperationResult(const wmpapi::JobIdApi &j_id, requestad::Ad *ad, requestad::ExpDagAd *dag,wmpapi::ConfigContext *cfs, const string &endpoint, const string *fout )
{
	string *node = NULL;
	vector <wmpapi::JobIdApi*> children;
	vector <wmpapi::JobIdApi*>::iterator it;
	wmpapi::JobIdApi* child = NULL;
	// output message
	string outmsg = "***********************************************************************************************\n";
	outmsg += "JOB REGISTER OUTCOME :\n\n\n";
	outmsg += "your job has been successfully registered with the jobid:\n\n";
	// father job
        outmsg += "\t" + 	j_id.jobid + "\n\n";
	node = j_id.nodeName ;
	if (node && node->size() > 0 ){
		outmsg += "node: " + *node + "\n\n";
	}
       outmsg += getInputFilesList( j_id.jobid, ad, cfs );
	// children
	children = j_id.children ;
	if ( ! children.empty() ){
		outmsg += "children:\n";
		outmsg += "=================================================================\n\n";
		for ( it = children.begin() ; it != children.end();++it){
			child = *it ;
			if (child){
				outmsg += "jobid: " + child->jobid + "\n";
				node = child->nodeName ;
				if (node && node->size() > 0 ){
					outmsg += "node: " + *node + "\n\n";
                                	outmsg += getDAGInputFilesList(child->jobid, *node, dag, cfs );
                                }
				outmsg += "=================================================================\n\n";
			}
		}
	}
	outmsg += "***********************************************************************************************\n";
	// prints the message
	fprintf (stdout, "\n%s", outmsg.c_str());
	// saves the result
	if (fout){
		if ( exutils::saveToFile(*fout, outmsg) < 0 ){
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
	// options
	int next_option = -1;
	// endpoint by options
	string *service =NULL;
	// endpoint by env var
	char* serv_ev = NULL;
	// JDL filepath
	string *jdl = NULL;
	// JDL collection
	string *collection_path = NULL;
	// delegation string
	string *del_id = NULL;
	// ouput filepath
	string *fout =NULL;
	// JDL
	string jdl_string = "";
	// error messages
	string errmsg = "";
	// context
	wmpapi::ConfigContext *cfs = NULL;
	// Ads
	requestad::Ad *ad ;
	requestad::ExpDagAd *dag = NULL;
	// executable name
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
                                case SHORTOPT_JDL : {
                                        jdl = new string(optarg);
                                        //cout << "debug - jdl=[" << *jdl << "]" << endl;
                                        break;
                                }
                                case SHORTOPT_COLLECTION : {
                                        collection_path = new string(optarg);
                                        //cout << "debug - jdl=[" << *jdl << "]" << endl;
                                        break;
                                }
                                case SHORTOPT_DELEGATION: {
                                        del_id = new string(optarg);
                                        //cout << debug - "del_id=[" << *del_id << "]" << endl;
                                        break;
                                }
                                case SHORTOPT_OUTPUT: {
                                        fout = new string(optarg);
                                        //cout << "debug - out=[" << *fout << "]" << endl;
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
                // JDL
                if ( !jdl ){
                        // loads JDLs for a COLLECTION
                        if (collection_path){
                                try {
                                        ad = requestad::AdConverter::createCollectionFromPath (*collection_path);
                                        // JDL string
                                        jdl_string = ad->toString( );
                                } catch ( exc::Exception &ex){
                                        fprintf (stderr, "\nerror: unable to create the collection Ad from the path:\n%s\n",collection_path->c_str() );
                                        cerr << "reason:" << flush << ex.what ( ) << endl ;
                                }
                        } else {
                                // error message (in case both jdl and collection options are missing)
                                fprintf (stderr, "\n\nerror: a mandatory option for JDL is missing\n");
                                fprintf (stderr, "use:\n- for normal and DAG jobs\n");
                                fprintf (stderr, "\t--%s  <JDL filepath> or -%c <JDL filepath>\n\n",
                                        LONGOPT_JDL, SHORTOPT_JDL);
                                fprintf (stderr, "- for collection of jobs\n");
                                fprintf (stderr, "\t--%s  <JDLs fpath> or -%c <JDLs path>\n\n",
                                        LONGOPT_COLLECTION, SHORTOPT_COLLECTION);
                                short_usage(prg_name);
                        }
                } else {
                        // Loads the JDL from file (normal and DAG jobs)
                        if ( ! apiutils::checkPathExistence ( jdl->c_str() ) ){
                                cerr << "\nerror: unable to find JDL file\n(no such file : " << *jdl << ")\n\n" << endl;
                                exit(-1);
                        }
                }
                if ( !jdl ){
                        errmsg = errmsg.assign("--").append(LONGOPT_JDL).append(" <JDL file>");
                        fprintf (stderr, "error: the following mandatory option is missing:\n%s\n", errmsg.c_str() );
                        short_usage(prg_name);
                }
                //delegation string
                if ( !del_id ){
                        errmsg = errmsg.assign("--").append(LONGOPT_DELEGATION).append(" <delegation identifier>");
                        fprintf (stderr, "error: the following mandatory option is missing:\n%s\n", errmsg.c_str() );
                        short_usage(prg_name);
                }

                try {
                        // config context
                        cfs = new wmpapi::ConfigContext("", *service, "");
                        // reads JDL from file on local machine
                        ad = new requestad::Ad( );
                        ad->fromFile (*jdl);
                        // JDL string (normal and DAG jobs)
                        jdl_string = ad->toString( );
                        // reads JDL's for DAG jobs
                        if ( ad->hasAttribute(requestad::JDL::TYPE , JDL_TYPE_DAG) ) {
                                dag = new requestad::ExpDagAd ( jdl_string );
                                //cout << "debug - dag=[" << dag->toString() << "]\n";
                                dag->expand( );
                                jdl_string = jdl_string.assign( dag->toString()) ;
                                ad = new requestad::Ad (jdl_string);
                        }
                        //cout << "debug - ad=[" << jdl_string << "]\n";
                        cout << "\nconnecting to the service at : " << *service << "\n";
                        // register
                        wmpapi::JobIdApi jobid = wmpapi::jobRegister( jdl_string, *del_id, cfs);
                        // displays result message (in case it is saved into the user file)
 			getOperationResult( jobid, ad, dag, cfs, *service, fout );

                } catch ( wmpapi::BaseException &b_ex) {
                        fprintf ( stderr, "Unable to register the job\n\nException caught:\n" );
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
