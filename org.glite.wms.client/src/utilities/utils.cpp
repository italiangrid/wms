 // PRIMITIVE
#include "netdb.h" // gethostbyname (resolveHost)
#include "iostream" // cin/cout     ()
#include "fstream" // filestream (ifstream)
#include "time.h" // time (getTime)
#include "sstream" //to convert number in to string
#include <signal.h>
#include <errno.h>
#include <stdlib.h>	// srandom
#include <sys/stat.h> //mkdir
#include <sys/wait.h> //wait
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// BOOST
#include "boost/tokenizer.hpp"
#include "boost/filesystem/operations.hpp"  // prefix & files procedures
#include "boost/filesystem/path.hpp" // prefix & files procedures
#include "boost/filesystem/exception.hpp" //managing boost errors
#include <boost/lexical_cast.hpp> // string->int conversion
#include "boost/regex.hpp"
// GLITE
#include "glite/wmsutils/jobid/JobId.h" // JobId
// JobId
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"
// WMProxy API's
#include "glite/wms/wmproxyapi/wmproxy_api.h"
#include "glite/wms/wmproxyapi/wmproxy_api_utilities.h"
// COMPONENT
#include "utils.h"
// encoding
#include "openssl/md5.h"
//gettimeofday
#include "sys/time.h"
#include "unistd.h"
// fstat
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"
#include "fcntl.h"
//gzip
#include "zlib.h"
#define local
#define CHUNK 16384
#include <sys/mman.h>
// Service Discovery
#include "ServiceDiscovery.h"

// Voms implementation
#include "openssl/ssl.h" // SSLeay_add_ssl_algorithms & ASN1_UTCTIME_get
#include "glite/security/voms/voms_api.h"  // voms parsing


namespace glite {
namespace wms{
namespace client {
namespace utilities {

using namespace std ;
using namespace glite::jdl;
using namespace glite::wmsutils::jobid ;

//using namespace boost ;
using namespace glite::wms::wmproxyapi;
using namespace glite::wms::wmproxyapiutils;
using namespace glite::wmsutils::exception;
namespace fs = boost::filesystem ;


// Define File Separator
#ifdef WIN
        // Windows File Separator
        const string FILE_SEP = "\\";
#else
        // Linux File Separator
        const string FILE_SEP ="/";
#endif

const char * X509_VOMS_DIR = "X509_VOMS_DIR";
const char * X509_CERT_DIR = "X509_CERT_DIR";
const char * VOMS_DIR = "/etc/grid-security/vomsdir";
const char * CERT_DIR = "/etc/grid-security/certificates";
const string monthStr[]  = {"Jan", "Feb", "March", "Apr", "May", "June" ,"July", "Aug", "Sept", "Oct", "Nov", "Dec"};

// gLite environment variables
const char* GLITE_LOCATION = "GLITE_LOCATION";
const char* GLITE_WMS_LOCATION = "GLITE_WMS_LOCATION";

const char*  WMS_CLIENT_CONFIG			=	"GLITE_WMS_CLIENT_CONFIG";
const char*  GLITE_WMS_WMPROXY_ENDPOINT	= 	"GLITE_WMS_WMPROXY_ENDPOINT";
const char*  GLITE_CLIENTCONF_FILENAME 		= "glite_wmsclient.conf";
const char*  GLITE_CONF_FILENAME 			= "glite_wms.conf";
const string DEFAULT_LB_PROTOCOL			=	"https";
const string PROTOCOL					=	"://";
const string TIME_SEPARATOR				=	":";
const int SUCCESS = 0;
const int FAILURE = 1;
const int FORK_FAILURE = -1;
const int COREDUMP_FAILURE = -2;
const int TIMEOUT_FAILURE = -3 ;



const unsigned int DEFAULT_LB_PORT	=	9000;
const unsigned int DEFAULT_WMP_PORT	=	7772;


const string Utils::JOBID_FILE_HEADER = "###Submitted Job Ids###";
const string Utils::JOBID_FAILED_HEADER = "#*Following Job Registered but Submission failed (try use --start option)";

const string PROTOCOL_SEPARATOR= "://";

const char* COMPRESS_MODE ="wb6 " ;
const int OFFSET = 16;
// File protocol string
const string FILE_PROTOCOL = "file://" ;
// Archives & Compressed files
const char* GZ_SUFFIX = ".gz";
const char* TAR_SUFFIX = ".tar";
const string GZ_MODE = "wb6f";

bool handled_sign = false ;
int status = 0;

class Dup {
public:
        /**
        * Default constructor
        */
        Dup( std::string script);
        /**
        * Default destructor
        */
        ~Dup( );
        /**
        * Retrieve infos in the files if they are not empty
	* @stream, the stream to be caught
        */
        std::string getInfo(std::string stream);
private:
	string outfile ;
	string errorfile ;
};
//                 VOMS METHODS (BEGIN)


/******************************************************************
		VOMS IMPLEMENTATION
*******************************************************************/
/******************************************************************
 method :load_chain
*******************************************************************/
STACK_OF(X509) *load_chain(const char *certfile){
	STACK_OF(X509_INFO) *sk=NULL;
	STACK_OF(X509) *stack=NULL, *ret=NULL;
	BIO *in=NULL;
	X509_INFO *xi;
	int first = 1;
	if(!(stack = sk_X509_new_null())) {
		BIO_free(in);
		sk_X509_INFO_free(sk);
		throw  WmsClientException(__FILE__,__LINE__, "load_chain",DEFAULT_ERR_CODE,
			"memory allocation","Unable to allocate STACK_OF(X509) instance") ;
	}
	if(!(in=BIO_new_file(certfile, "r"))) {
		BIO_free(in);
		sk_X509_INFO_free(sk);
		throw  WmsClientException(__FILE__,__LINE__, "load_chain",DEFAULT_ERR_CODE,
			"I/O Error","error opening proxy file") ;
	}
	// This loads from a file, a stack of x509/crl/pkey sets
	if(!(sk=PEM_X509_INFO_read_bio(in,NULL,NULL,NULL))) {
		BIO_free(in);
		sk_X509_INFO_free(sk);
		throw  WmsClientException(__FILE__,__LINE__, "load_chain",DEFAULT_ERR_CODE,
			"I/O Error","error reading proxy file") ;
	}
	/* scan over it and pull out the certs */
	while (sk_X509_INFO_num(sk)) {
		/* skip first cert */
		if (first) {
			first = 0;
			continue;
		}
		xi=sk_X509_INFO_shift(sk);
		if (xi->x509 != NULL) {
			sk_X509_push(stack,xi->x509);
			xi->x509=NULL;
		}
		X509_INFO_free(xi);
	}
	if(!sk_X509_num(stack)) {
		sk_X509_free(stack);
		BIO_free(in);
		sk_X509_INFO_free(sk);
		// goto end;
		throw  WmsClientException(__FILE__,__LINE__, "load_chain",DEFAULT_ERR_CODE,
			"Parsing Error", "no certificates in proxy file") ;
	}
	ret=stack;
	return ret;
}
/******************************************************************
private method: load_voms
*******************************************************************/
void load_voms (vomsdata *vo_data, const char *proxy_file){
	//  Pointer MUST arrive already set
	assert(proxy_file);
	assert(vo_data);
	vo_data->data.clear() ;
	BIO *in = NULL;
	X509 *x = NULL;
	in = BIO_new(BIO_s_file());
	SSLeay_add_ssl_algorithms();
	if (in) {
		if (BIO_read_filename(in,proxy_file)>0){
			x = PEM_read_bio_X509(in, NULL, 0, NULL);
			if(!x){
				throw  WmsClientException(__FILE__,__LINE__, "load_voms",DEFAULT_ERR_CODE,
					"I/O Error","Couldn't find a valid proxy");
			}
			STACK_OF(X509) *chain = load_chain(proxy_file);
			vo_data->SetVerificationType((verify_type)(VERIFY_SIGN|VERIFY_KEY));
			if (!vo_data->Retrieve( x , chain, RECURSE_CHAIN)  ){
				vo_data->SetVerificationType((verify_type)(VERIFY_NONE));
				if (vo_data->Retrieve(x, chain, RECURSE_CHAIN)){
					// it is ONLY a WARNING: Unable to verify signature!
					// (Please check if the host certificate of the VOMS server that has
					// issued your proxy is installed on this machine)
				}
			}
			sk_X509_free(chain);
		} else{
			throw  WmsClientException(__FILE__,__LINE__,"load_voms",DEFAULT_ERR_CODE,
				"I/O Error","Couldn't find a valid proxy certificate");
		}
	} else {
		throw  WmsClientException(__FILE__,__LINE__,"load_voms",DEFAULT_ERR_CODE,
			"I/O Error","Invalid Proxy");
	}
	if (vo_data->error!=VERR_NONE){
		// throw CredProxyException(__FILE__,__LINE__ , METHOD,WMS_VO_LOAD,"parse");
		// Do something?
	}
	// Release memory
	BIO_free(in);
}


/**
*     Returns the Default VirtualOrganisation for the specified certificate
*     @param pxfile the proxy file pathname
*     @return the string representation of the default VO find inside the specified certificate
*     @throws BaseException If any error occurred during the reading of the proxy information
*     @see glite::wms::wmproxyapi::BaseException
**/


const std::string getDefaultVoVoms(const char *pxfile){
	// int error = 0;
	vomsdata *vo_data = new vomsdata() ;
	load_voms(vo_data, pxfile);
	voms v;
	// get Default voms
	if (!vo_data->DefaultData(v)){
		// Free the memory allocated	
		delete(vo_data);

		// Unable to load default VO value: return empty string
		return "";
	}

	// Free the memory allocated	
	delete(vo_data);
	
	return string (v.voname);
}

//                 VOMS METHODS (END)

/*************************************
*** Constructor **********************
**************************************/


Utils::Utils(Options *wmcOpts){
	string ids = "";
	this->wmcOpts=wmcOpts;
	// verbosity level
        vbLevel = (LogLevel)wmcOpts->getVerbosityLevel();
	logInfo = new Log(vbLevel);
	string file_header = Utils::getStripe(80, "*")+ "\n";
	file_header += string(Options::HELP_UI) +  " - " + string(Options::HELP_VERSION) +  " - Log File\n";
	file_header += Utils::getStripe(80, "*");
	logInfo->print(WMS_UNDEF, file_header, "", false, true);
	logInfo->print(WMS_INFO, "Function:", wmcOpts->getApplicationName( ), false, true);
        logInfo->print(WMS_INFO, "Options:", wmcOpts->getOptionsInfo( ), false,true);
	ids = Utils::getList(wmcOpts->getJobIds( ));
	if (ids.size() > 0){ logInfo->print(WMS_INFO, "JobId(s):", ids, false,true);}
	// Ad utilities
	wmcAd = new AdUtils (wmcOpts);
	// checks and reads the configuration file
	this->virtualOrganisation = this->checkConf();
	// debug information
	debugInfo = wmcOpts->getBoolAttribute(Options::DBG);
	// log-file
	string log_file = this->generateLogFile();
	if (!log_file.empty()){ logInfo->createLogFile (log_file);}
}
/*************************************
*** Destructor **********************
**************************************/
Utils::~Utils( ){
	if (wmcAd){ delete(wmcAd); }
        if (logInfo){ delete(logInfo); }
        if (wmcConf){ delete(wmcConf); }
}

/*************************************
*** General Utilities methods *
**************************************/
bool Utils::answerYes (const std::string& question, bool defaultAnswer, bool defaultValue){
	if ( this->wmcOpts->getBoolAttribute(Options::NOINT)){
		// No interaction required
		return defaultValue ;
	}
	string possible=" [y/n]";
	possible +=(defaultAnswer?"y":"n");
	possible +=" :";
	char x[1024];
	char *c = (char*)malloc(128);
	while (1){
		cout << question << possible << " " ;
		cin.getline(x,128);
		c=&x[0]; //cut off the \n char
		if((*c=='y')||(*c=='Y')){return true;}
		else if((*c=='n')||(*c=='N')){return false;}
		else if (*c=='\0'){return defaultAnswer;}
	}
	if(c){free(c);}
}

void Utils::ending(unsigned int exitCode){
	exit(exitCode);
}

bool Utils::askForFileOverwriting(const std::string &path){
	bool ow = true;
	// checks if the output file already exists
	if ( isFile(path) && ! this->wmcOpts->getBoolAttribute(Options::NOINT) ){
		// if the file exists ......
		string info = Utils::getAbsolutePath(path) + " file already exists";
		// writes a warning msg in the log file
		if (logInfo){ logInfo->print(WMS_WARNING, "Ouput file:", info, false);}
		ostringstream q;
		q << "\n\n" + info + "\n";
		q  << "Do you want to overwrite it ?";
		if (  ! this->answerYes( q.str(), false, true) ){ ow = false; }
	}
	return ow;
}

std::vector<std::string> Utils::askMenu(std::vector<std::string> &items, const enum WmcMenu &type){
	std::vector<std::string> chosen;
	ostringstream out ;
        ostringstream question ;
	string line = "";
	string comment = "";
        char x[1024];
	int len = 0;
        int size = items.size();
	bool ask = true;
	bool multiple = false;
	// if the vector is empty or contains only one items, no menu is shown
	if (size < 2 ){
		return items;
	}
	// type of question
	switch (type){
		case (Utils::MENU_JOBID) :{
			question << "Choose one or more jobId(s) in the list - [1-" << size
			<< "]all (use , as separator or - for a range):";
			multiple = true;
			break;
		};
		case (Utils::MENU_SINGLEJOBID) :{
			question << "Choose one jobId in the list :";
			break;
		};
		case (Utils::MENU_CE) :{
			question << "Choose one or more resource(s) in the list - [1-" << size
			<< "]all (use , as separator- for a range):";
			multiple = true ;
			break;
		};
		case (Utils::MENU_SINGLECE) :{
			question << "Choose one resource in the list :";
			break;
		};
		case (Utils::MENU_FILE) :{
			question << "Choose one or more file(s) in the list - [1-" << size
			<< "]all (use , as separator- for a range):";
			multiple = true ;
			break;
		};
		case (Utils::MENU_SINGLEFILE) :{
			question << "Choose one file in the list :";
			break;
		};
		default :{
			break;
		};
	}
	// MENU ============
	out <<"------------------------------------------------------------------\n";
	//counters
	int i, it = 0;
	//checks how many comments will be erased from the vector items
	int erased = 0;
	while ( i < ( size - erased ) ) {
		if ( items[i].find("#*") == 0 ) {
			// Saving and formatting the comment to be printed on next jobId line
			comment = items[i] ;
			int len = comment.size() ;
			int iter = 0;
			vector<string>::iterator temp = items.begin()+i ;
			items.erase(temp) ;
			erased++ ;
			// Erases # and * chars from the special comment
			while ( iter < len-2) {
				if ( (comment.compare(iter,1,"#") == 0) || (comment.compare(iter,1,"*") == 0)) {
					comment.erase(iter, 2);
					iter++;
				}
			iter++ ;
			}
			//decrease loop counter since it has been erased an element from the vector
			i-- ;
		}
		if ( !comment.empty() ) {
			// Appends the comment to the jobId
			out << "\t" << comment << "\n";
			comment = "";
		} else {
			// No comment to be appended
			out << (it+1) << " : " << items[i] << "\n";
			it++ ;
		}
		i++ ;
	}
	if (multiple) { out << "a : all\n" ; }
	out << "q : quit\n";
	out <<"------------------------------------------------------------------\n\n";
	cout << out.str();
	// ===============
	while (ask){
		// Question --------
		ask = false;
		cout << question.str() <<  " " ;
		cin.getline(x,128);
		// Processing the reply -----------
		line = Utils::cleanString(string(x));
		len = line.size( );
		if ( len == 0 ){
			// Return typed - All items selected
			// cancellation/output of all jobs
			return items;
		} else {
			// a= all jobs
			if(line=="a" && multiple) {
				return items;
			} else if (line=="q") {
				// q = quit ; exits from the programme execution
				cout << "bye\n";
				Utils::ending(1);
			}

			// checks for a single-job choice
			try {
				int n = boost::lexical_cast<unsigned int>(Utils::cleanString(line) );
				if ( n>=1 && n <= size ){
					chosen.push_back(items[(int)(n-1)]);
				} else {ask = true; continue;}
				break;
			} catch(boost::bad_lexical_cast &exc) { /* do nothing */ }
			if (multiple) {
				// range: <n1>-<n2>
				string::size_type pos =line.find("-",string::npos) ;
				if ( pos != string::npos) {
					// reads the range limits
					string s1 = line.substr(0, pos);
					string s2 = line.substr(pos+1, line.size()-pos);

					try {
						// string-to-int conversion
						int n1 = boost::lexical_cast<unsigned int>(Utils::cleanString(s1) );
						int n2 = boost::lexical_cast<unsigned int>(Utils::cleanString(s2) );
						// range extraction
						if ((n1 >= 1 && n1 <= size)  && (n2 >= n1 && n2 <= size) ) {
							for (int i=n1; i < (n2+1) ; i++) {
								chosen.push_back(items[(int)(i-1)]);
							}
							break; // exits from the while-loop
						} else {ask = true ; continue;}
					} catch(boost::bad_lexical_cast &exc) { ask = true; }
				} else {
					// checks for a multiple-job choice
					if (chosen.empty()){
						// some of the jobs ......
						line +=  string( " " );
						boost::char_separator<char> separator(",");
						boost::tokenizer<boost::char_separator<char> > tok(line, separator);
						boost::tokenizer<boost::char_separator<char> >::iterator token = tok.begin();
						boost::tokenizer<boost::char_separator<char> >::iterator const end = tok.end();
						for ( ; token != end; ++token) {
							string ch = *token;
							int cc = atoi(ch.c_str());
							if (cc >= 1 && cc <= size) {
								chosen.push_back(items[(int)(cc-1)]);
							} else {
								ask = true ;
								break;
							} // if (cc)
						} //for
					} //if (chosen)
				} // if (pos) - else
			} else{
				ask = true;
				continue;
			} // if (multiple).else
		} //while
	}

	return chosen;
}


/**********************************
*** WMP, LB, Host Static methods ***
***********************************/
void Utils::resolveHost(const std::string& hostname, std::string& resolved_name){
	struct hostent *result = NULL;
	if( (result = gethostbyname(hostname.c_str())) == NULL ){
		throw WmsClientException(__FILE__,__LINE__,"resolveHost",DEFAULT_ERR_CODE,
					"Wrong Value","Unable to resolve host: "+hostname);
	}
	resolved_name=result->h_name;
}
std::vector<std::string> Utils::getLbs(const std::vector<std::vector<std::string> >& lbGroup, int nsNum){
	std::vector<std::string> lbs ;
	unsigned int lbGroupSize=lbGroup.size();
	switch (lbGroupSize){
		case 0:
			// No LB provided
			throw WmsClientException(__FILE__,__LINE__,"getLbs",DEFAULT_ERR_CODE,
				"Empty Value","No Lb found in Configuration File");
		case 1:
			// One lb group provided: it's the one
			return lbGroup[0] ;
		default:
			break;
	}
	if (nsNum>(int)lbGroupSize){
		// requested LB number is out of available LBs
		throw WmsClientException(__FILE__,__LINE__,"getLbs",DEFAULT_ERR_CODE,
				"Mismatch Value","LB request number out of limit");
	}else if (nsNum>=0){
		// Retrieving the requested LB by provided nsNum
		return lbGroup[nsNum];
	}else for (unsigned int i=0; i < lbGroupSize; i++){
		// No WMP number provided, gathering all LB
		unsigned int size = lbGroup[i].size() ;
		for (unsigned int j=0;j < size ;j++){
			lbs.push_back(lbGroup[i][j]);
		}
	}
	return lbs;

}

/*
* methods to get WMProxy URL's
*/
const int Utils::getRandom (const unsigned int &max ){
	unsigned int r = 0;
        if (max>0){
		srand( ((int)time(NULL) * max) );
                r = rand() % max ;
 	 }
        return r;
}
std::vector<std::string> Utils::getWmps(){
	std::vector<std::string> wmps ;
	char *ep = NULL;
	string eps = "";
	// URL by the command line options
	eps = wmcOpts->getStringAttribute(Options::ENDPOINT);
	if (!eps.empty()){ wmps.push_back(eps);}
	else {
		// URL by the environment
		ep = getenv(  GLITE_WMS_WMPROXY_ENDPOINT);
		if (ep){ wmps.push_back(ep);}
		else if (wmcConf){
			// Check if exists the attribute WmProxyEndPoints
			if(wmcConf->hasAttribute(JDL_WMPROXY_ENDPOINT)) {
				// Retrieve and set the attribute WmProxyEndPoints
				wmps = wmcConf->getStringValue(JDL_WMPROXY_ENDPOINT);
			}
		}
	}
	return wmps;
}
/*
* Query the service discovery for published WMProxy enpoints
*/
std::vector<std::string> Utils::lookForServiceType(SdServiceType st, const string &vo){
	std::vector<std::string> foundServices ;
	SDServiceList *serviceList=NULL;
	SDException ex;
	SDVOList *vos = NULL;
	char **names  = NULL;

	string serviceType="";
	
	switch (st){
		case WMP_SD_TYPE:
			if (wmcConf){
				// Check if exists the attribute WMProxyServiceDiscoveryType
				if(wmcConf->hasAttribute(JDL_WMPROXY_SERVICE_DISCOVERY_TYPE)) {
					// Retrieve and set the attribute WMProxyServiceDiscoveryType
					serviceType=wmcConf->getString(JDL_WMPROXY_SERVICE_DISCOVERY_TYPE);
				}
			}
			if (serviceType.empty()){serviceType=DEFAULT_WMPROXY_SERVICE_TYPE;}
			// TBD default value is already present inside configuration
			break;
		case LB_SD_TYPE:
			if (wmcConf){
				// Check if exists the attribute WMProxyServiceDiscoveryType
				if(wmcConf->hasAttribute(JDL_WMPROXY_SERVICE_DISCOVERY_TYPE)) {
					// Retrieve and set the attribute WMProxyServiceDiscoveryType
					serviceType=wmcConf->getString(JDL_WMPROXY_SERVICE_DISCOVERY_TYPE);
				}
			}
			if (serviceType.empty()){serviceType=DEFAULT_LBCLIENT_SERVICE_TYPE;}
			// TBD default value is already present inside configuration
			break;
		default:
			throw WmsClientException(__FILE__,__LINE__,"lookForServiceType",DEFAULT_ERR_CODE,
				"Service Discovery not performed", "Unkwnown Service Type requested");
	}

	// Setup VirtualORganisation
	if (vo.length()){
		// Vo specified, check the ENV variable:
		const char* GLITE_SD_VO ="GLITE_SD_VO";
		char* sdVo=getenv(GLITE_SD_VO);
		if (sdVo && vo.compare(string(sdVo))){
			// Two different VOs: warning message
			logInfo->print (WMS_WARNING,"$GLITE_SD_VO value ("+string(sdVo) +
			") will be overriden by default user VO:", vo, true, true);
			if (!this->answerYes("Do you whish to continue?", true, true) ){
				return foundServices;
			}
		}
		char **names = new char*[1];
		names[0] = new char[vo.length() +1];
		strcpy(names[0], vo.c_str());
		SDVOList vosTmp = {1, names};
		vos=&vosTmp;
		logInfo->print (WMS_DEBUG, "Querying service discovery","\nService Type: "+
			serviceType +"\nVirtualOrganisation: "+ vo,true,true);
	}else{
		logInfo->print (WMS_DEBUG, "Querying service discovery","\n Service Type:"+
			serviceType,true,true);
	}
	// Service Query:
	//  SD_listServices parameters: type, site, volist, exception
	serviceList = SD_listServices( serviceType.c_str(), NULL, vos, &ex);
	if (serviceList != NULL){
		if ( serviceList->numServices > 0 )  {
			for(int k=0; k < serviceList->numServices; k++) {
				foundServices.push_back(strdup(serviceList->services[k]->endpoint));
			}
		}else{
			logInfo->print (WMS_WARNING, "Service Discovery produced no result for VO:", vo,true,true);
		}
		SD_freeServiceList(serviceList);
		if (names){
			delete []names[0];
			delete []names;
		}
	} else {
		if (ex.status == SDStatus_SUCCESS){
			throw WmsClientException(__FILE__,__LINE__,"lookForServiceType",DEFAULT_ERR_CODE,
			"Service not known",
			string(DEFAULT_WMPROXY_SERVICE_TYPE));
		}else {
			throw WmsClientException(__FILE__,__LINE__,"lookForServiceType",DEFAULT_ERR_CODE,
			"Service Discovery failed",
			ex.reason);
		}
	}
	return foundServices;
}

/*
* Gets the ErrorStorage pathname
*/
const std::string Utils::getErrorStorage( ){
	// Initialise the ErrorStorage
	string storage = DEFAULT_ERROR_STORAGE;
	
	// Check if exists the configuration
        if (wmcConf){
		// Check if exists the attribute ErrorStorage
		if(wmcConf->hasAttribute(JDL_ERROR_STORAGE)) {
			// Retrieve and set the attribute ErrorStorage
			storage = wmcConf->getString(JDL_ERROR_STORAGE);
		}
	}
	
	// Return the ErrorStorage
	return storage;
 }
/*
* Get&check the OuputStorage pathname
*/
std::string Utils::getOutputStorage( ){
	// Initialise the OutputStorage
	string storage = DEFAULT_OUTPUT_STORAGE;

	// Check if exists the configuration
        if (wmcConf){
		// Check if exists the attribute OutputStorage
		if(wmcConf->hasAttribute(JDL_OUTPUT_STORAGE)) {
			// Retrieve and set the attribute OutputStorage
			storage = wmcConf->getString(JDL_OUTPUT_STORAGE);
		}
	}
	
	// Check if the storage is a directory
	if(!this->isDirectory(storage)){
		throw WmsClientException(__FILE__,__LINE__,"getOutputStorage",DEFAULT_ERR_CODE,
				"Configuration Error",
				"Output Storage pathname doesn't exist (check the configuration file): "+storage);
	}
	
	// Return the OutputStorage
	return storage ;
 }

 /*
 * Gets the default Log File pathname
 * /<OutStorage-path>/<commandname>_<UID>_<PID>_<timestamp>.log
 */
const std::string Utils::getDefaultLog ( ){
        //
        ostringstream filepath ;
        string appl_name = "";
        // error storage pathname
	string storage = this->getErrorStorage( );
        if (Utils::isDirectory(storage)){
                filepath << Utils::normalizePath(storage) << "/";
        } else {
		filepath << DEFAULT_ERROR_STORAGE << "/";
        }
        //application name
	appl_name = wmcOpts->getApplicationName();
	if (appl_name.size( ) == 0){filepath << "wms-client_" ;}
	else{filepath << appl_name << "_";}
	filepath << getuid( ) << "_"  << getpid( ) << "_";
	// timestamp calculation:
	time_t now = time(NULL);
	struct tm *ns = localtime(&now);
	filepath << (ns->tm_year+1900)  <<  twoDigits(ns->tm_mon+1)  << twoDigits(ns->tm_mday)<<"_";
	filepath << twoDigits(ns->tm_hour) << "-" << twoDigits(ns->tm_min) << "-" << twoDigits(ns->tm_sec);
	filepath << ".log";
	return  filepath.str();
}

/*
* Gets the log filename
*/
std::string Utils::getLogFileName (void){
	string path = "";
	string log = "";

	if (logInfo){
        	// if logInfo object has been already set
        	log = logInfo->getPathName();
        } 
	else if (!wmcOpts->getStringAttribute(Options::LOGFILE).empty()){
	       	// by --log option
        	log = wmcOpts->getStringAttribute(Options::LOGFILE);	
	} 
	else  if (wmcOpts->getBoolAttribute(Options::DBG)){
		log = string(this->getDefaultLog( ));
	}

	return log;
};

/*
* Generate the log filename
*/
std::string Utils::generateLogFile ( ){

	string path = "";
	string log = "";

        if (!wmcOpts->getStringAttribute(Options::LOGFILE).empty()){
        	// by --log option
        	log = wmcOpts->getStringAttribute(Options::LOGFILE);

	} 
	else  if (wmcOpts->getBoolAttribute(Options::DBG)){
		log = string(this->getDefaultLog( ));
	}

	return log;

};

/*
* Error messages according to HTTP /1.1 status codes
*/
std::string Utils::httpErrorMessage(const int &code){
	string msg = "";
	switch (code){
		case (400):{
			msg = "Bad Request (the request could not be understood by the server due to malformed syntax)";
			break;
		};
		case (401):{
			msg = "Not Authorised (user authentication error)";
			break;
		};
		case (403):{
			msg = "Forbidden (request refused)";
			break;
		};
		case (404):{
			msg = "File Not Found (no matching found for the requested URI)";
			break;
		};
		case (407):{
			msg = "Proxy Authentication Required (the request first requires authentication with the proxy)";
			break;
		};
		case (408):{
			msg = "Request Timeout";
			break;
		};
		case (414):{
			msg = "Requested URI Too Long";
			break;
		};
		case (500):{
			msg = "";
			break;
		};
		case (501):{
			msg = "Service Not Implemented";
			break;
		};
		case (505):{
			msg = "HTTP Version Not Supported";
			break;
		};
		default :{
			msg = "";
		};
	};
	return msg;
};
/** Static private method **/
std::pair <std::string, unsigned int> checkAd(	const std::string& adFullAddress,
						const std::string& DEFAULT_PROTOCOL,
						unsigned int DEFAULT_PORT){
	pair<string, unsigned int> ad;
	// Look for protocol
	unsigned int protInd=adFullAddress.find(PROTOCOL);
	if (protInd==string::npos){
		// protocol not found
		ad.first=DEFAULT_PROTOCOL;
		protInd=0;
	}else if (protInd==0){
		throw WmsClientException(__FILE__,__LINE__,"checkAd",DEFAULT_ERR_CODE,
				"Wrong Value","Wrong Protocol Specified for: "+adFullAddress);
	}
	// Look for port
	unsigned int portInd=adFullAddress.find(":",protInd+1);
	if (portInd==string::npos){
		// port not found
		ad.second=DEFAULT_PORT;
	}else {
		// port found
		try{
			ad.second=boost::lexical_cast<unsigned int>(adFullAddress.substr(portInd+1));
		}catch(boost::bad_lexical_cast &){
			throw WmsClientException(__FILE__,__LINE__,"checkAd",DEFAULT_ERR_CODE,
				"Wrong Value","Failed to parse integer port for: "+adFullAddress);
		}
	}
	// Add host
	ad.first+=adFullAddress.substr(0,portInd);
	return ad;
}


/**********************************
Virtual Organisation methods
VO check priority:
	- cedrtificate extension
	- config option
	- env variable
	- JDL (submit||listmatch)
***********************************/
std::vector<std::string> Utils::parseFQAN(const std::string &fqan){
	if (fqan==""){
		throw WmsClientException(__FILE__,__LINE__,"parseFQAN",
			DEFAULT_ERR_CODE,
			"Wrong Value", "Unable to parse empty FQAN value");
	}
	vector<string> returnvector;
	boost::char_separator<char> separator("/");
	boost::tokenizer<boost::char_separator<char> >tok(fqan, separator);
	boost::tokenizer<boost::char_separator<char> >::iterator token = tok.begin();
	boost::tokenizer<boost::char_separator<char> >::iterator const end = tok.end();
	for( ; token != end; token++) {
		returnvector.push_back(*token);
	}
	return returnvector;
}

std::string Utils::FQANtoVO(const std::string fqan){
	return  Utils::parseFQAN(fqan)[0];
}



std::string Utils::getDefaultVo(){
	const char *proxy = glite::wms::wmproxyapiutils::getProxyFile(NULL) ;
	if (proxy){
		return getDefaultVoVoms(string(proxy).c_str());
	} else {
		throw WmsClientException(__FILE__,__LINE__,"getDefaultVo",
			DEFAULT_ERR_CODE,
			"Proxy File Not Found", "Unable to find a valid proxy file");
	}
}
std::string Utils::checkConf(){
	string voPath, voName;
	// config-file pathname
	string cfg = wmcOpts->getStringAttribute( Options::CONFIG ) ;
	// vo-name
	string vo = wmcOpts->getStringAttribute( Options::VO ) ;
	// they can't set together !
	if (!vo.empty() && !cfg.empty()){
		ostringstream err ;
		err << "the following options cannot be specified together:\n" ;
		err << wmcOpts->getAttributeUsage(Options::VO) << "\n";
		err << wmcOpts->getAttributeUsage(Options::CONFIG) << "\n\n";
		throw WmsClientException(__FILE__,__LINE__,
				"checkConf",DEFAULT_ERR_CODE,
				"Input Option Error", err.str());
	}
	voSrc src =NONE;
	if(getDefaultVo()!=""){
		// certificate extension - point to vo plain name
		logInfo->print (WMS_DEBUG, "Vo read from", "proxy certificate extension",true,true);
		voName=getDefaultVo();
		src=CERT_EXTENSION;
	}else{
		// Unable to read VOMS extension: debug
		logInfo->print (WMS_DEBUG, "Unable to read VOMS extension from proxy certificate", "",true,true);
	}
	// OTHER OPTIONS PARSING.........
	if (!vo.empty() && src!=NONE){
		// VO name forcing ignored
		logInfo->print (WMS_WARNING, "--vo option ignored","" , true,true);
	}else if (!vo.empty()){
		// vo option point to the file
		// SCR is definitely NONE
		voName=vo;
		src=VO_OPT;
		logInfo->print (WMS_DEBUG, "Vo read from", "--vo option", true,true);
	}else if (!cfg.empty()){
		cfg = Utils::getAbsolutePath (cfg);
		// config option point to the file
		if (src==NONE){
			logInfo->print (WMS_DEBUG, "Vo read from", "--config option",true,true);
		}
		// Store config path value
		voPath= cfg;
		src=CONFIG_OPT;
	}else if(getenv(WMS_CLIENT_CONFIG)){
		// env variable point to the file
		if (src==NONE){
			logInfo->print (WMS_DEBUG, "Vo read from", "ENV option",true,true);
		}
		// Store config path value
		voPath=string(getenv(WMS_CLIENT_CONFIG));
		src=CONFIG_VAR;
	}else if (!wmcOpts->getPath2Jdl().empty()){
		// JDL specified(submit||listmatch) read the vo plain name
		if (src==NONE){
			src=JDL_FILE;
			voPath=wmcOpts->getPath2Jdl();
			// Read jdl (voName is still an empty string)
			wmcAd->parseVo(src,voPath,voName);
			logInfo->print (WMS_DEBUG, "Vo read from", "JDL",true,true);
		}
	}else if (src==NONE){
		// If this point is reached no possible VO source found
		throw WmsClientException(__FILE__,__LINE__,
				"checkConf", DEFAULT_ERR_CODE,
				"Empty value",
				(voName=="")?("Unable to find any VirtualOrganisation"):"Unable to find any configuration file");
	}
	// Eventually Parse fields
	wmcAd->parseVo(src,voPath,voName);
	// Print Info
	//if (vbLevel==WMSLOG_DEBUG){errMsg (WMS_DEBUG, "VirtualOrganisation value :", voName,true);}
	logInfo->print(WMS_DEBUG, "VirtualOrganisation value :", voName,true,true);
	// check the Installation path
	checkPrefix( );
	string cfDefault = this->getPrefix( ) +  "/etc/" + glite_wms_client_toLower(voName) + "/" + GLITE_CLIENTCONF_FILENAME ;
	string cfGeneral = this->getPrefix( ) +  "/etc/" + GLITE_CLIENTCONF_FILENAME ;
	if ( !(isFile(cfDefault)))
	{
	//checking old configuration file glite_wms.conf for compatibility purpose if glite_wmsclient.conf was not found
		cfDefault = this->getPrefix( ) +  "/etc/" + glite_wms_client_toLower(voName) + "/" + GLITE_CONF_FILENAME ;
	}
	if (!(isFile(cfGeneral)))
	{
	//checking old configuration file glite_wms.conf for compatibility purpose if glite_wmsclient.conf was not found
		cfGeneral = this->getPrefix( ) +  "/etc/" + GLITE_CONF_FILENAME ;
	}
	wmcConf = wmcAd->loadConfiguration(voPath, cfDefault,cfGeneral, voName);
	
	return voName;
}

void Utils::checkPrefix( ){
	// Look for GLITE installation path
	vector<string> paths ;
	if (getenv("GLITE_WMS_LOCATION")){ paths.push_back (string(getenv("GLITE_WMS_LOCATION")) );}
	if (getenv("GLITE_LOCATION")){ paths.push_back (string(getenv("GLITE_LOCATION")) );}
	paths.push_back("/opt/glite");
	paths.push_back("/usr/local");
	// Look for conf-file:
	string defpath = "";
	unsigned int size = paths.size();
	for (unsigned int i=0;i < size;i++){
		defpath =paths[i];
		if ( isFile((defpath + "/bin/" + wmcOpts->getApplicationName()))){
			break;
		}else{
                	// Unable to find path
			defpath = "";
                }
	}
	if (defpath.size()==0) {
		//errMsg(WMS_WARNING, "Unable to find glite installation",
		//"no installation in /opt/glite or in /usr/local ; neither GLITE_WMS_LOCATION nor GLITE_LOCATION are set", true);
		logInfo->print(WMS_WARNING, "Unable to find glite installation",
		"no installation in /opt/glite or in /usr/local ; neither GLITE_WMS_LOCATION nor GLITE_LOCATION are set", true,true);
	}
	prefix=defpath;
}
std::string Utils::getPrefix(){return prefix;}

/**********************************
JobId checks Methods
***********************************/

string Utils::checkJobId(std::string jobid){
	if ( jobid.find("#*") == 0 )	{
		return jobid;
	}
	JobId jid (jobid);
        return jobid;
}

 std::vector<std::string> Utils::checkJobIds(std::vector<std::string> &jobids){
        vector<std::string> wrongs;
	vector<std::string> rights;
        if (wmcOpts) {
		std::vector<std::string>::iterator it = jobids.begin() ;
		std::vector<std::string>::iterator const end = jobids.end();
                for ( ; it != end ; it++){
                        try{
                                Utils::checkJobId(*it);
				rights.push_back(*it);
                        } catch (WrongIdException &exc){
                        	wrongs.push_back(*it);
                        }
                }
		// the found wrongs jobids
                if ( ! wmcOpts->getBoolAttribute(Options::NOINT) && ! wrongs.empty() ){
			if (rights.size()){
				ostringstream err ;
				err << "bad format for the following jobid(s) :\n";
				for ( it = wrongs.begin( ) ; it !=  wrongs.end( ) ; it++ ){
					err << " - " << *it << "\n";
				}
				logInfo->print(WMS_WARNING, "Wrong JobId(s)", err.str() );
				if (! answerYes ("Do you wish to continue ?", true, true) ){
					cout << "bye\n";
					ending(0);
				}
			} else {
				// Not even a right jobid
				throw WmsClientException(__FILE__,__LINE__,
					"checkJobIds", DEFAULT_ERR_CODE,
					"Wrong Input Value",
					"all parsed jobids in bad format" );
			}
		}
        }
	return rights;
}

string Utils::getUnique(std::string jobid){
	JobId jid (jobid);
        return jid.getUnique();
}

/**********************************
Resource checks Methods
***********************************/

void Utils::checkResource(const std::string& resource){
	// resource must have at least a slash separator
	if ( resource.find("/")==string::npos ){
		throw WmsClientException(__FILE__,__LINE__,
			"checkResource", DEFAULT_ERR_CODE,
			"Wrong Resource Value",
			"invalid resource value ("+resource+")" );
	}
}

 std::vector<std::string> Utils::checkResources(std::vector<std::string> &resources){
	vector<std::string> wrongs;
	vector<std::string> rights;
	if (wmcOpts) {
		std::vector<std::string>::iterator it = resources.begin();
		std::vector<std::string>::iterator const end = resources.end();
		for ( ; it != end; it++){
			try{
				Utils::checkResource(*it);
				rights.push_back(*it);
			} catch (WmsClientException &exc){
				wrongs.push_back(*it);
			}
		}
		// the found wrongs resources
		if ( ! wmcOpts->getBoolAttribute(Options::NOINT) && ! wrongs.empty() ){
			if (rights.empty()){
				// Not even a right resource
				throw WmsClientException(__FILE__,__LINE__,
					"checkResources", DEFAULT_ERR_CODE,
					"Wrong Input Value",
					"all parsed resources in bad format" );
			} else{
				ostringstream err ;
				err << "bad format for the following resource(s) :\n";
				for ( it = wrongs.begin( ) ; it !=  wrongs.end( ) ; it++ ){
					err << " - " << *it << "\n";
				}
				logInfo->print(WMS_WARNING, "Wrong Resource(s)", err.str() );
				if (! answerYes ("Do you wish to continue ?", true, true) ){
					cout << "bye\n";
					ending(0);
				}
			}
		}
	}
	return rights;
}


/**********************************
Time Checks methods
***********************************/
const std::vector<std::string> Utils::extractFields(const std::string &instr, const std::string &sep){
	vector<string> vt ;
	// extracts the fields from the input string
	boost::char_separator<char> separator(sep.c_str());
	boost::tokenizer<boost::char_separator<char> > tok(instr, separator);
	boost::tokenizer<boost::char_separator<char> >::iterator token = tok.begin();
	boost::tokenizer<boost::char_separator<char> >::iterator const end = tok.end();
	for ( ; token != end; token++) {
    		vt.push_back(*token);
	}
	return vt;
}
const long Utils::getTime(const std::string &st,
			const std::string &sep,
			const time_t &now,
			const unsigned int &nf){
	// extract the time fields from the input string
	vector<string> vt = extractFields(st, TIME_SEPARATOR);
	// checks the number of fields (only if nf>0 !)
	if (nf >0 && vt.size() != nf ){
		ostringstream err ;
                char c = 'X' ;
		err <<  "expected format is " ;
                for (unsigned int i = 0; i < nf ; i++){
                	if (i > 0) err << ":";
			err << c << c ;
                        c++;
                }
		throw WmsClientException(__FILE__,__LINE__,
			"getTime", DEFAULT_ERR_CODE,
                        "Wrong Time Value",
			"invalid time string (" + err.str() +")" );
	}
	// reads the fields of the vector and get the number of seconds from 1970
	struct tm ts  = {0,0,0,0,0,0,0} ;
	// time struct for the local-time
	struct tm *ns = localtime(&now);
	//seconds
	ts.tm_sec = 0;
	// seconds east of UTC
	ts.tm_gmtoff =ns->tm_gmtoff ;
	//daylight saving time
	ts.tm_isdst = ns->tm_isdst ;
	switch (vt.size()){
		case (2): {
			// hh:mm
			ts.tm_hour = atoi(vt[0].c_str());
			ts.tm_min = atoi(vt[1].c_str());
			// current day of the year
			ts.tm_year = ns->tm_year ;
			ts.tm_mon = ns->tm_mon ;
			ts.tm_mday = ns->tm_mday;
			break;
		}
		case (4) :{
			// MM:DD:hh:mm
			ts.tm_mon = atoi(vt[0].c_str()) -1;
			ts.tm_mday = atoi(vt[1].c_str());
			ts.tm_hour =atoi( vt[2].c_str());
			ts.tm_min = atoi(vt[3].c_str());
			// current year
			ts.tm_year = ns->tm_year ;
			break;
		}
		case (5) :{
			// MM:DD:hh:mm:YYYY
			ts.tm_mon = atoi(vt[0].c_str()) -1;
			ts.tm_mday = atoi(vt[1].c_str());
			ts.tm_hour = atoi(vt[2].c_str());
			ts.tm_min = atoi(vt[3].c_str());
			ts.tm_year = atoi(vt[4].c_str()) - 1900;
			break;
		}
		default:{
			throw WmsClientException(__FILE__,__LINE__,
				"getTime", DEFAULT_ERR_CODE,
				"Wrong Time Value",
				string("invalid time string (" + st + ")"));
		}
	}
	return  mktime(&ts) ;
}

const long Utils::checkTime ( const std::string &st, int &days,  int &hours, int &minutes, const Options::TimeOpts &opt){
	long sec = 0;
        // current time
	time_t now = time(NULL);
	// converts the input  time string to the vector to seconds from 1970
        switch (opt){
		case (Options::TIME_VALID):{
			sec = getTime(st, TIME_SEPARATOR, now, 2);
                        break;
                }
                default :{
                	sec = getTime(st, TIME_SEPARATOR, now);
                        break;
                }
        }
	if (sec > 0){
		 switch (opt){
                 	case (Options::TIME_TO):
                        case (Options::TIME_VALID):{
				if (sec <= now){
					throw WmsClientException(__FILE__,__LINE__,
					"checkTime", DEFAULT_ERR_CODE,
					"Invalid Time Value",
					"the time value is out of limit ("+ st + ")");
                                }
                                break;
                	}
                        case (Options::TIME_FROM):{
				if (sec > now){
					throw WmsClientException(__FILE__,__LINE__,
					"checkTime", DEFAULT_ERR_CODE,
					"Invalid Time Value",
					"time value should be earlier than the current time");
                                }
                                break;
                	}
                        default :{
				break;
                        }
    		}
	} else {
		throw WmsClientException(__FILE__,__LINE__,
			"checkTime", DEFAULT_ERR_CODE,
			"Wrong Time Value",
			string("the string is not a valid time expression (" + st + ")") );
	}
	hours = (sec-now) / 3600 ;
	if (hours > 23) {
		days = hours / 24 ;
		hours = hours % 24 ;
	} else {
		days = 0;
	}
	minutes = ((sec-now) % 3600) / 60 ;
 	return sec;
}

/****************************************************
* Utility methods for strings
****************************************************/
/*
*	base64 encoder
*/
const int Utils::base64Encoder(void *enc, int enc_size, char *out,  int out_max_size)
{
    static const char* b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    unsigned char* enc_buf = (unsigned char*)enc;
    int out_size = 0;
    unsigned int  bits = 0;
    unsigned int  shift = 0;
    while ( out_size < out_max_size ) {
        if ( enc_size>0 ) {
            // Shift in byte
            bits <<= 8;
            bits |= *enc_buf;
            shift += 8;
            // Next byte
            enc_buf++;
            enc_size--;
        } else if ( shift>0 ) {
            // Pad last bits to 6 bits - will end next loop
            bits <<= 6 - shift;
            shift = 6;
        } else {
            // Terminate with Mime style '='
            *out = '=';
            out_size++;
            return out_size;
        }
        // Encode 6 bit segments
        while ( shift>=6 ) {
            shift -= 6;
            *out = b64[ (bits >> shift) & 0x3F ];
            out++;
            out_size++;
        }
    }
    // Output overflow
    return -1;
}

/*
*	converts the input string to a md5base64-format string
*/
const char * Utils::str2md5Base64(const char *s)
{
    MD5_CTX md5;
    unsigned char d[16];
    char buf[50];
    int l;
    MD5_Init(&md5);
    MD5_Update(&md5, s, strlen(s));
    MD5_Final(d, &md5);
    l = base64Encoder(d, 16, buf, sizeof(buf) - 1);
    if (l < 1) {
        return NULL;
   }
    buf[l - 1] = 0;
    return strdup(buf);
}


/*
* generate a unique string
*/
std::string Utils::getUniqueString (){

        struct hostent* he;
        struct timeval tv;
        int skip;
        // to hold string for encrypt
        char hostname[1024];
        char* unique = NULL ;
        // generation of the string
        gethostname(hostname, 100);
        he = gethostbyname(hostname);
        assert(he->h_length > 0);
        gettimeofday(&tv, NULL);
        srandom(tv.tv_usec);
        skip = strlen(hostname);
        skip += sprintf(hostname + skip, "-IP:0x%x-pid:%d-rnd:%d-time:%d:%d",
                    *((int*)he->h_addr_list[0]), getpid(), (int)random(),
                    (int)tv.tv_sec, (int)tv.tv_usec);
        //gets back the generated del-id
       unique = ((char*)str2md5Base64(hostname) );
       if (unique){
       		return string(unique);
       } else{
       		return "";
       }
}
/*
* gets a char stripe
*/
const std::string Utils::getStripe (const int &len, const std::string &ch, const std::string &msg){
	int max = len;
        int size = 0;
        string stripe = "";
        string fs = "";
	if (len > 0){
        	size = msg.size( );
                if (size > 0){ max = (len / 2) - ((size  / 2) +1) ; }
                // build the stripe
                for (int i = 0; i < max ; i++){
			stripe += ch ;
                }
                if (size > 0){
			fs = stripe + " " + msg + " " + stripe;
                } else {
			fs = stripe ;
                }
 	}
	return fs ;
}
/*
* removes white spaces form the begininng and from the end of the input string
*/

const std::string Utils::cleanString(std::string str) {
	int len = 0;
	string ws = " "; //white space char
	len = str.size( );
	if (len > 0) {
		// erases white space at the beginning of the string
		while (len>1) {
			if ( str.compare(0,1,ws) == 0) {
				str = str.substr(1, len);
			} else {
				break;
			}
			len = str.size();
		}
		// erases white space at the end of the string
		while (len>1) {
			if( str.compare(len-1,1,ws) == 0 ) {
				str = str.substr(0, len-1);
			} else {
				break;
			}
			len = str.size();
		}
		// 1 white space
		if (len == 1 & str.compare(ws)==0) {
			str = "";
		}
	}
	return str;
}

/*
* Converts the input integer to a string, adding a "zero"-digit ahead if it has one digit ( 0< d < )
*/
std::string Utils::twoDigits(unsigned int d ){
	ostringstream dd ;
        if (d<10){ dd << "0" << d ;}
        else { dd <<  d ;}
	return dd.str();
};
/*
*	adds the star (*) wildcard to the input pathname
*/
const std::string Utils::addStarWildCard2Path(std::string& path){
        if ( path.compare( path.length()-1, 1, "/" ) != 0 ){
                path += "/";
        }
        path += "*";
        return path;
 }

 /**
*	Gets a string with the list of the items
*/
const std::string Utils::getList (const std::vector<std::string> &items){
	string info = "" ;
	unsigned int size = items.size();
	for(unsigned int i = 0; i < size; i++ ){
		if (info.size()>0){ info += " ; "; }
		info += items[i];
	}
	return info;
 }
/*********************************************
* Utility method for files
**********************************************/

/*
* Checks whether the pathname represents a file
*/
const bool Utils::isFile (const std::string &pathname){
        bool is_valid = false;
        if ( checkPathExistence(pathname.c_str() ) ){
                try{
                      //  fs::path cp (normalizePath(pathname), fs::system_specific); // Boost 1.29.1
			fs::path cp (normalizePath(pathname), fs::native);
                        is_valid = !( fs::is_directory(cp)) ;
                }catch (fs::filesystem_error &ex){ }
        }
        return is_valid;
}
/*
* Checks whether the pathname represents a directory
*/
const bool Utils::isDirectory (const std::string &pathname){
	bool is_valid = false;
	if ( checkPathExistence(pathname.c_str()) ){
		try{
			 //fs::path cp (Utils::normalizePath(pathname), fs::system_specific); // Boost 1.29.1
			fs::path cp (Utils::normalizePath(pathname), fs::native);
        		is_valid =  fs::is_directory(cp) ;
		}catch (fs::filesystem_error &ex){ }
	}
        return is_valid;
}
/*
* Gets the absolute path of the file
*/
const std::string Utils::getAbsolutePath(const std::string &file ){
	string path = file ;
	char* pwd = getenv ("PWD");
	if (path.find("./")==0 || path.compare(".")==0){
		// PWD path  (./)
		if (pwd) {
			string leaf = path.substr(1,string::npos);
			if (leaf.size()>0) {
				if ( leaf.find("/",0) !=0 ) {
					path = normalizePath(pwd) + "/"  + leaf;
				} else {
					path = normalizePath(pwd) + leaf;
				}
			} else{
				path = normalizePath(pwd) + leaf;
			}
		}
	} else if (path.find("/") ==0 ){
		// ABsolute Path
		path = normalizePath(path);
	} else {
		// Relative path: append PWD
		if (pwd){
			path = normalizePath(pwd) + "/" + path;
		}
	}
	return path;
}
/*
* Gets the size of a file
*/
int Utils::getFileSize (const std::string &path) {
	struct stat file_info;
	if (isFile(path)) {
		int hd = open(path.c_str(), O_RDONLY) ;
 		 if (hd < 0) {
			throw WmsClientException(__FILE__,__LINE__,
				"open",  DEFAULT_ERR_CODE,
				"File i/o Error", "unable to open the file : " + path );
		}
                fstat(hd, &file_info);
                close(hd) ;
        } else {
		throw WmsClientException(__FILE__,__LINE__,
			"getFileSize",  DEFAULT_ERR_CODE,
			"File i/o Error", "no such file : " + path );
        }
        return file_info.st_size ;
}

/*
* Deletes a file
*/
void Utils::removeFile(const std::string &file) {
	try{
		//fs::path cp (file, fs::system_specific); // Boost 1.29.1
		fs::path cp (file, fs::native);
		if ( fs::is_directory(cp))  {
			throw WmsClientException(__FILE__,__LINE__,
				"removeFile",  DEFAULT_ERR_CODE,
				"File i/o Error", "this path is not a valid file : " + file );
		}
		 fs::remove (cp);
	} catch (fs::filesystem_error &ex) {
		throw WmsClientException(__FILE__,__LINE__,
			"removeFile",  DEFAULT_ERR_CODE,
			"File i/o Error", "no such file : " + file );
	}
}

/*
* Reads the content of a  file
*/
std::string Utils::fromFile (const std::string &path) {
	ostringstream bfr;
        string s = "";
	string txt = "";
	if (isFile(path)) {
		ifstream inputstream(path.c_str()) ;
             	if (inputstream.is_open() ) {
			while(getline(inputstream, s)){ bfr << s << "\n";}
   			inputstream.close();
			txt = string(bfr.str());
		}
 	}
        return txt;
}

/*
* Saves message into a  file
*/
int Utils::toFile (const std::string &path, const std::string &msg, const bool &append) {
	int result = -1;
	ios::openmode mode ;
	if (append ) {
		mode = ios::app ;
	} else {
		mode = ios::trunc ;
	}
	ofstream outputstream(path.c_str(), mode);
        if (outputstream.is_open()) {
                outputstream << msg  << "\n";
                outputstream.close();
                result = 0;
        }
        return result;
}
/**
* Saves a message into a file after checking whether it already exists.
* If it does and user interaction has not been disabled (by --noint),
* the user is asked which action has to be performed
* (either overwriting or appending ; otherwise nothing)
*/
int Utils::saveToFile(const std::string &path, const std::string &msg) {
	bool ask = true;
	string answer = "";
	char  x[1024] = "";
	int len = 0;
	int result = 0;
	// checks if the output file already exists
	if ( isFile(path ) && ! this->wmcOpts->getBoolAttribute(Options::NOINT) ){
		// if the file exists ......
		string info = Utils::getAbsolutePath(path) + " file already exists";
		// writes a warning msg in the log file
		if (logInfo){ logInfo->print(WMS_WARNING, "Ouput file:", info, false);}
		ostringstream q;
		q << "\n\n" + info + "\n";
		q  << "Do you want to append (a) or to overwrite (o) ?\n";
		q << "Press the 'q' key for not saving.\n";
		while (ask){
			// Question --------
			ask = false;
			cout << q.str() <<  " " ;
			cin.getline(x,128);
			// Processing the reply -----------
			answer = answer.assign(Utils::cleanString(x));
			len = answer.size( );
			if (len > 0) {
				if (answer.compare("a")==0) {
					result = this->toFile(path, msg, true);
				} else if (answer.compare("o")==0) {
					result = this->toFile(path, msg, false);
				} else if (answer.compare("q")==0) {
					/* do nothing*/
					result = -1;
				} else {
					ask = true;
				}
			} else {
				ask = true;
			}
		}
	} else {
		result = this->toFile(path, msg);
	}
	return result;
}
/*
* Stores a list in a file
*/
const int Utils::saveListToFile (const std::string &path, const std::vector<std::string> &list, const std::string header){
	string msg = "";
	// Number of items in the input list
	int size = list.size();
	if (header.size( )>0){ msg = header + "\n";}
	// Prepares the message with the list items
	for(int i = 0 ; i < size ; i++){
		msg += list [i] + "\n";
	}
	// Saves to file
	return saveToFile(path,msg);
}
/*
* Stores a jobid in a file
*/
const int Utils::saveJobIdToFile (const std::string &path, const std::string jobid, std::string failed ){
	string outmsg = "";
        string fromfile = "";
	try {
        	// reads the file if it already exists
		fromfile = Utils::fromFile(path);
       		if (!fromfile.empty()){
                        if ( fromfile.find(Utils::JOBID_FILE_HEADER)==string::npos){
                        	string outfile = wmcOpts->getStringAttribute (Options::OUTPUT);
                                if (!outfile.empty() &&
                                	Utils::answerYes("\nThe following pathname is not a valid submission output file:\n"+
                                        	string(Utils::getAbsolutePath(outfile) ) +
                                        "\nDo you want to overwrite it ?"   , false, true )  ){
						// no list of jobid's
 						outmsg = Utils::JOBID_FILE_HEADER + "\n";
       					} else {
						return (-1);
                                        }
                        } else {
                        	// list of jobid's = yes
				outmsg = cleanString(fromfile.c_str());
                                if ( outmsg.find("\n", outmsg.size()-1)==string::npos){outmsg +="\n";}
                        }
    		} else {
                	// the file doesn't exist (new file)
			outmsg = Utils::JOBID_FILE_HEADER + "\n";
   		}
	} catch (WmsClientException &exc){
		outmsg = Utils::JOBID_FILE_HEADER + "\n";
        }
	if (!failed.empty()){
		outmsg += Utils::JOBID_FAILED_HEADER + "\n";
        }
        outmsg += jobid ;
	return (toFile(path, outmsg));
}
/**
 * Removes '/' characters at the end of the of the input pathname
 */
 const std::string Utils::normalizePath( const std::string &fpath ) {
  string                   modified;
  string::const_iterator   last, next;
  string::reverse_iterator check;

  last = fpath.begin();
  do {
    next = find( last, fpath.end(), '/' );

    if( next != fpath.end() ) {
      modified.append( last, next + 1 );

      for( last = next; *last == '/'; ++last );
    }
    else modified.append( last, fpath.end() );
  } while( next != fpath.end() );

  check = modified.rbegin();
  if( *check == '/' ) modified.assign( modified.begin(), modified.end() - 1 );

  return modified;
}
/**
 * Removes the file protocol string at the beginning of the path if it is present
 */
 const std::string Utils::normalizeFile( const std::string &fpath ) {
	string file = "";
	unsigned int pos = fpath.find(FILE_PROTOCOL) ;
	if ( pos != string::npos){
		// removes the file protocol at the beginning of the path
		file = fpath.substr(pos+FILE_PROTOCOL.size( ), fpath.size());
	} else {
		file = fpath;
	}
	return file;
}

/*
* Returns the tail of the input URL, removing the protocol, the server
* and the port infomation
* e.g.   input 	https://server:port/tok1/tok2/file.txt
*	output:	tok1/tok2/file.txt
*/
std::string  Utils::getAbsolutePathFromURI (const std::string& uri) {
	string tmp = "";
	// looks for the end of the protocol string
	unsigned int p =uri.find(PROTOCOL_SEPARATOR);
 	if (p!=string::npos) { tmp = uri.substr(p+PROTOCOL_SEPARATOR.size(), uri.size());}
	// looks for the end of the host:port string
	 p = tmp.find("/");
	if (p!=string::npos){ tmp = tmp.substr(p+1, uri.size());}
	return tmp;

}

/*
* Extracts the protocol from the input URI
*/
std::string  Utils::getProtocol (const std::string& uri) {
	string proto = "";
	string list = "";
	unsigned int p =uri.find("//");
	if (p!=string::npos) {
		proto = uri.substr(0, p-1);
	} else {
		throw WmsClientException(__FILE__,__LINE__,
			"getProtocol",  DEFAULT_ERR_CODE,
			"Protocol Error",
			"This URI doesn't have protocol :" + uri );
	}
	return proto;
}
/*
* Extracts the filename from the input path
*/
std::string  Utils::getFileName (const std::string& path) {
	string tmp = "";
	int size = path.size( );
	unsigned int p =path.rfind("/", size);
 	if (p!=string::npos) { tmp = path.substr(p+1, size);}
	return tmp;
}

/*
* Reads a file
*/
std::vector<std::string> Utils::getItemsFromFile (const std::string &path){
	vector<string> items;
        string bfr = fromFile(path);
	if (!bfr.empty()){
		boost::char_separator<char> separator("\n");
		boost::tokenizer<boost::char_separator<char> > tok(bfr, separator);
		boost::tokenizer<boost::char_separator<char> >::iterator token = tok.begin();
		boost::tokenizer<boost::char_separator<char> >::iterator const end = tok.end();
		for ( ; token != end; ++token) {
			string comment = "";
			string it = *token;
			it = Utils::cleanString( (char*) it.c_str()) ;
			if ( it.find("#*")==0 &&!this->wmcOpts->getBoolAttribute(Options::NOINT) ) {
				// It's a special comment, insert line
				comment = it ;
				items.push_back(comment);
			} else if (   (it.find("#")==0)||it.find("//")==0) {
				// It's a comment, skip line
			} else if (Utils::hasElement(items, it) == false) {
				// Append line
				items.push_back(it);
			}
                }
  	} else {
		throw WmsClientException(__FILE__,__LINE__,
			"getItemsFromFile",  DEFAULT_ERR_CODE,
			"File i/o Error", "invalid input file : " + path );
        }
        return items;
}

/*********************************
* Get-FileExtensions methods
**********************************/
std::string Utils::getArchiveExtension( ) { return TAR_SUFFIX; }

std::string Utils::getZipExtension( ) { return GZ_SUFFIX; }

std::string Utils::getArchiveFilename (const std::string file){
	string tar = "";
	string ext =getArchiveExtension( ) ;
	string::size_type pos = file.find(getArchiveExtension( ));
	if (pos != string::npos){
		pos += ext.size( );
		tar = file.substr(0,pos);
	}
	return tar ;
}
/**
* Reports a zlib  error
*/
std::string Utils::gzError(int code) {
        string err ="";
        switch (code) {
        case Z_ERRNO:
                err = "i/o error";
                break;
        case Z_STREAM_ERROR:
                err = "invalid compression level";
                break;
        case Z_DATA_ERROR:
                err = "invalid or incomplete deflate data";
                break;
        case Z_MEM_ERROR:
                err = "out of memory\n";
                break;
        case Z_VERSION_ERROR:
                err = "zlib version mismatch";
                break;
        }
        return err;
}

/**
* File compression functionality
*/
 std::string  Utils::fileCompression(const std::string &file) {
	string compr = "";
	string errmsg = "";
	string zmsg = "";
	int len = 0;
	int err = 0;
	int ifd =0;
	struct stat sb;
	caddr_t buf;    /// mmap'ed buffer for the entire input file
	off_t buf_len;  // length of the input file
	FILE *in = NULL;
	gzFile out;
	in = fopen(file.c_str(), "rb");
	if (in == NULL) {
		errmsg = "unable to open file to be compressed: " + file;
		throw WmsClientException(__FILE__,__LINE__,
			"Utils::compressFile",  DEFAULT_ERR_CODE,
			"File i/o Error",
			errmsg);
	}
	compr = file + GZ_SUFFIX;
	out = gzopen(compr.c_str(), GZ_MODE.c_str());
	if (out == NULL) {
		errmsg = "unable to create the gz file: " + compr ;
		throw WmsClientException(__FILE__,__LINE__,
			"Utils::compressFile",  DEFAULT_ERR_CODE,
			"File i/o Error",
			errmsg);
	}
	ifd = fileno(in);
	/* Determine the size of the file, needed for mmap: */
	fstat(ifd, &sb);
	buf_len = sb.st_size;
	if (buf_len <= 0)  {
		errmsg = "unable to compress the file: " + file + "\n";
		errmsg += "(invalid file size)\n";
		throw WmsClientException(__FILE__,__LINE__,
			"Utils::compressFile",  DEFAULT_ERR_CODE,
			"File i/o Error",
			errmsg);
	}
	// Now do the actual mmap
	buf = (char*)mmap((caddr_t) 0, buf_len, PROT_READ, MAP_SHARED, ifd, (off_t)0);
	if (buf == (caddr_t)(-1))  {
		errmsg = "error while compressing the file: " + file + "\n";
		throw WmsClientException(__FILE__,__LINE__,
			"Utils::compressFile",  DEFAULT_ERR_CODE,
			"File i/o Error",
			errmsg);
	}
	/* Compress the whole file at once: */
	len = gzwrite(out, (char *)buf, (unsigned)buf_len);
	if (len != (int)buf_len)  {
		errmsg = "unable to compress the file: " + file + "\n";
		zmsg = gzerror(out, &err);
		if (zmsg.size()>0)  { errmsg += zmsg ;}
		throw WmsClientException(__FILE__,__LINE__,
			"Utils::compressFile",  DEFAULT_ERR_CODE,
			"File i/o Error",
			errmsg);
	}
	munmap(buf, buf_len);
	fclose(in);
	if (gzclose(out) != Z_OK)  {
		logInfo->print(WMS_WARNING, "Unable to remove the gz file (error while closing the file):", file);
	} else  {
		Utils::removeFile(file);
	}
	return compr;
}
/**
* Checks if a vector of strings contains a string item
*/
bool Utils::hasElement (const std::vector<std::string> &vect, std::string item) {
	bool found = false;
	int size = vect.size( );
	for (int i = 0 ; i < size ; i++) {
		if (vect[i] == item ){
			found = true;
			break;
		}
	}
	return found ;
}
/**
* handles the signal returned by the child of the process, sets handled_sign at true
* @int sign, the signal
*/
void childSignalHandler ( int sign ) {
	if ( sign == SIGCHLD) {
		handled_sign = true;
		wait(&status);
	}
	if ( sign == SIGINT ){
		wait(&status);
		throw WmsClientException(__FILE__,__LINE__, "doExecv",DEFAULT_ERR_CODE,"Interrupt signal","User killed the command execution") ;
	}
}


/**
* Dup class constructor,
* @string script, name of the script generated
*/
Dup::Dup( string script ) {
	outfile = "/tmp/"+script+ boost::lexical_cast<std::string>(getpid());
	int fdO = open(outfile.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600);
	dup2(fdO, 1);
	close(fdO);

	errorfile = "/tmp/"+script+ boost::lexical_cast<std::string>(getpid());
	int fdE = open(errorfile.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600);
	dup2(fdE, 2);
	close(fdE);
}
/**
* Dup class destructor
*/
Dup::~Dup() {
	remove( outfile.c_str() ) ;
	remove( errorfile.c_str() ) ;
}
/**
* getInfo, prints the lines of the file in a string, and returns it if not empty
*/
std::string Dup::getInfo( string stream ) {
	if (( stream.compare("output") == 0 ) && (outfile != "" )) {
		return outfile ;
	} else if (( stream.compare("error") == 0 ) && ( errorfile != "" )) {
		return errorfile ;
	} else {
	return "" ;
	}
}
/**
* Forks the process and execute command line
*/
int Utils::doExecv(const string &command, vector<string> &params, string &errormsg, const int &delay) {
	GLITE_STACK_TRY("doExecv()");
	status = 0;
	pid_t child_pid ;

	handled_sign = false;
	signal ( SIGCHLD, childSignalHandler)  ;
	signal ( SIGINT, childSignalHandler ) ;

	// Set the timeout value
	int timeout = delay;

	// Check if the timeout is valid
	if(timeout <= 0) {
		// Reset
		timeout = INT_MAX;
	}


	//initialize command and arguments to be passed to the execv in the child process
	char **argvs;
	int size = params.size() + 2;
	argvs = (char **) calloc(size, sizeof(char *));
	unsigned int i = 0;

	argvs[0] = (char *) malloc(command.length() + 1);
	strcpy(argvs[0], command.c_str());
	i = 1;
	vector<string>::iterator iter = params.begin();
	vector<string>::iterator const end = params.end();
	while(iter != end) {
		argvs[i] = (char *) malloc((*iter).length() + 1);
		strcpy(argvs[i], (*iter).c_str());
		i++;
		iter++;
	}
	argvs[i] = NULL;

	switch ( child_pid = fork()) {
		case -1:
			// Unable to fork
			errormsg = "Unable to fork process";
			logInfo->print (WMS_WARNING, "Method doExecv: ", errormsg,true,true);
			return FORK_FAILURE;
			break;
		case 0:
			// child: execute the required command, on success does not return
			if (execv (command.c_str(), argvs)) {
				//execv failed
				//duplicates the std.out and std.err of the command into two files
				Dup* dupoe = new Dup ( "doExecv" ) ;
				string output = dupoe->getInfo( "output" );
				string error = dupoe->getInfo( "error" );
				string outFileString = fromFile( output ) ;
				string errFileString = fromFile( error ) ;
				errormsg = strerror(errno);
				logInfo -> print(WMS_WARNING, "Method doExecv: Error message: \n", outFileString+"\n"+errFileString, true, true) ;
				//deletes the files created
				delete( dupoe ) ;
				}
			// the child does not return
			break;
		default:
			//parent
			int current = 0 ;
			//initialize the time limit for the child process
			int limit = current+timeout ;
			while ( (!handled_sign) && (current < limit) ) {
				sleep( 1 );
				current++ ;
			}
			//timeout limit has been reached, the child process will be killed
			if (handled_sign == false) {
				logInfo -> print (WMS_WARNING, "Method doExecv: ", "Timeout reached, command execution will be terminated now", true, true) ;
				//kills the child
				kill( child_pid, SIGKILL ) ;
				return TIMEOUT_FAILURE ;
			}
			if (WIFEXITED(status)) {
				//TO_DO child wait succesfully
				errormsg =WIFEXITED(status);
			}
			if (WIFSIGNALED(status)) {
				errormsg = WIFSIGNALED(status);
				logInfo-> print(WMS_WARNING, "Method doExecv:  " , errormsg, true, true) ;
			}
#ifdef WCOREDUMP
			if (WCOREDUMP(status)) {
					errormsg = "Child dumped core";
				logInfo ->print(WMS_ERROR, "Method doExecv: ", errormsg, true, true) ;
				return COREDUMP_FAILURE;
			}
#endif // WCOREDUMP
			if (status) {
				if (WIFEXITED(status)) {
					errormsg = strerror(WEXITSTATUS(status));
				} else {
					errormsg = "Child failure";
				}
				logInfo-> print(WMS_ERROR, "Method doExecv: ", errormsg, true, true);
				return WIFEXITED(status);
			}
		break;
	}
	for (unsigned int j = 0; j <= i; j++) {
		free(argvs[j]);
	}
	free(argvs);
	return SUCCESS;
	GLITE_STACK_CATCH();
}
/**
* Gets an URL in input, extracts the IP address, and returns the same URL
* with the hostname instead of the address
*/
string Utils::resolveAddress( string relpath ) {
	// Setting the regular expression for an IP address
	boost::regex regExp ( "^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$" ) ;
	int it = 0;
	string pathtemp = "";
	string address = "";
	struct hostent *result = NULL;
	string hostname = "";
	if (relpath ==""){
		throw WmsClientException(__FILE__,__LINE__,"resolveAddress",
			DEFAULT_ERR_CODE,
			"Wrong Value", "Unable to parse empty address");
	}
	boost::char_separator<char> separator(":/");
 	boost::tokenizer<boost::char_separator<char> >tok(relpath, separator);
	boost::tokenizer<boost::char_separator<char> >::iterator token = tok.begin();
	boost::tokenizer<boost::char_separator<char> >::iterator const end = tok.end();
	for( ; token != end; token++) {
		if ( it == 1 ) {
			address = *token ;
		}
		if (it > 1 && it < 7) {
			pathtemp += *token + "/"  ;
		}
		if (it == 7) {
			pathtemp += *token  ;
		}
		it++;
	}
	// Checking if the string extracted is an IP or an hostname
	if ( regex_match ( address, regExp ) ) {
		// IP Address
		struct sockaddr_in ip;
		inet_aton( address.c_str(), &ip.sin_addr ) ;
		if( (result = gethostbyaddr( (char*)&ip.sin_addr, sizeof(ip.sin_addr), AF_INET)) == NULL ){
			throw WmsClientException(__FILE__,__LINE__,"resolveAddress",DEFAULT_ERR_CODE,
					"Wrong Value","Unable to resolve address: "+address);
		}
		hostname = result->h_name;
	} else {
		// Hostname
		hostname = address ;
	}
	string path = "https://"+hostname+":"+pathtemp ;
	return path ;
}


} // glite
} // wms
} // client
} // utilities

