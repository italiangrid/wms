

#include "stdio.h"
// i/o streams
#include <iostream>
#include <fstream>
#include <sstream>


// utils
#include "utils.h"
//getuid-pid
#include "unistd.h"
// BOOST
#include "boost/filesystem/path.hpp" // path
#include "boost/filesystem/exception.hpp" //managing boost errors
#include <boost/lexical_cast.hpp>

#include "options_utils.h"

using namespace std;
using namespace glite::wms::wmproxyapi;
namespace fs = boost::filesystem ;

namespace glite {
namespace wms{
namespace client {
namespace utilities {


std::string glite_wms_client_toLower ( const std::string &src){
        std::string result(src);
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
}

// constants for WMProxy  client software version
const std::string WMP_CLT_MAJOR_VERSION = "1";
const std::string WMP_CLT_MINOR_VERSION = "3";
const std::string WMP_CLT_RELEASE_VERSION = "1";
const std::string WMP_CLT_POINT_VERSION = ".";

/**
* WMProxy version since the getTransferProtocols is available
* (see the getVersion service)
*	version >= 2.2.0
*/
const int Options::WMPROXY_GETPROTOCOLS_VERSION  = 2;
const int Options::WMPROXY_GETPROTOCOLS_MINOR_VERSION  = 2;
const int Options::WMPROXY_GETPROTOCOLS_SUBMINOR_VERSION = 0;
/**
 * Help Info messages
*/
const std::string Options::HELP_COPYRIGHT = "Copyright (C) 2005 by DATAMAT SpA";
const std::string Options::HELP_EMAIL = "egee@datamat.it";
const std::string Options::HELP_UI = "WMS User Interface" ;
const std::string Options::HELP_VERSION = "version  " + WMP_CLT_MAJOR_VERSION
        + WMP_CLT_POINT_VERSION + WMP_CLT_MINOR_VERSION
        + WMP_CLT_POINT_VERSION + WMP_CLT_RELEASE_VERSION; ;

const std::string Options::BUG_MSG = "(please report to " + HELP_EMAIL +")";
/*
* Protocols for file transferring operations
*/
const string Options::TRANSFER_FILES_CURL_PROTO = "https" ;
const string Options::TRANSFER_FILES_GUC_PROTO = "gsiftp" ;
const string Options::TRANSFER_FILES_DEF_PROTO = Options::TRANSFER_FILES_GUC_PROTO ;
const string Options::JOBPATH_URI_PROTO ="https" ;
const char* Options::TRANSFER_FILES_PROTOCOLS[ ] = {
	Options::TRANSFER_FILES_GUC_PROTO.c_str(),
	Options::TRANSFER_FILES_CURL_PROTO.c_str()
};
/**
* Constant string to allow specifing "Retrieve all protocols"
* when calling WMP services with "protocol" input parameter
*/
const string Options::WMP_ALL_PROTOCOLS = "all";
/**
* Limitations on File sizes
*/
// Byte offset for tar files
const long Options::TAR_OFFSET = 500;
// Deafult Max file size
const long Options::MAX_DEFAULT_FILE_SIZE = 2147483647;

// Max size (bytes) allowed for tar files
const long Options::MAX_TAR_SIZE = Options::MAX_DEFAULT_FILE_SIZE - Options::TAR_OFFSET;
// Max file size for globus-url-copy
const long Options::MAX_GUC_SIZE = Options::MAX_DEFAULT_FILE_SIZE;
// Max file size for CURL
const long Options::MAX_CURL_SIZE = Options::MAX_DEFAULT_FILE_SIZE;

/*
* Verbosity level
*/
const unsigned int Options::DEFAULT_VERBOSITY = 1;
const unsigned int Options::MAX_VERBOSITY = 3;
/*
*	LONG OPTION STRINGS
*/
const char* Options::LONG_ALL 			= "all";
const char* Options::LONG_CHKPT		= "chkpt";
const char* Options::LONG_COLLECTION	= "collection";
const char* Options::LONG_DAG			= "dag";
const char* Options::LONG_DEBUG		= "debug";
const char* Options::LONG_DEFJDL		= "default-jdl";
const char* Options::LONG_DIR			= "dir";
const char* Options::LONG_FROM		= "from";
const char* Options::LONG_GET			= "get";
const char* Options::LONG_HELP 		= "help";
const char* Options::LONG_INPUTFILE		= "input-file";
const char* Options::LONG_LISTONLY		= "list-only";
const char* Options::LONG_LRMS		= "lrms";
const char* Options::LONG_LOGFILE		= "logfile";
const char* Options::LONG_NODESRES 	= "nodes-resource";
const char* Options::LONG_NODISPLAY 	= "nodisplay";
const char* Options::LONG_NOGUI		= "nogui";
const char* Options::LONG_NOINT		= "noint";
const char* Options::LONG_NOLISTEN		= "nolisten";
const char* Options::LONG_NOMSG		= "nomsg";
const char* Options::LONG_PROTO		= "proto";
const char* Options::LONG_RANK 		= "rank";
const char* Options::LONG_REGISTERONLY = "register-only";
const char* Options::LONG_SET			= "set";
const char* Options::LONG_START 		= "start";
const char* Options::LONG_TO			= "to";
const char* Options::LONG_TRANSFER 	= "transfer-files";
const char* Options::LONG_UNSET		= "unset";
const char* Options::LONG_USERTAG		= "user-tag";
const char* Options::LONG_VERSION		= "version";
const char* Options::LONG_VO			= "vo";


/*
*	LONG OPTION STRINGS & SHORT CHARs
*/
const char* Options::LONG_AUTODG 	= "autm-delegation";
const char Options::SHORT_AUTODG 	= 'a';
// ouput
const char* Options::LONG_OUTPUT	= "output";
const char Options::SHORT_OUTPUT	= 'o' ;
// input
const char* Options::LONG_INPUT	= "input" ;
const char Options::SHORT_INPUT	= 'i' ;
// config
const char*Options::LONG_CONFIG	= "config";
const char Options::SHORT_CONFIG	= 'c' ;
// filename
const char* Options::LONG_FILENAME	= "filename";
const char Options::SHORT_FILENAME 	= 'f';
// resource
const char* Options::LONG_RESOURCE = "resource";
const char Options::SHORT_RESOURCE = 'r' ;
// validity
const char* Options::LONG_VALID	= "valid" ;
const char* Options::LONG_VERBOSE 	= "verbose-level";
const char Options::SHORT_V	= 'v' ;
// status
const char* Options::LONG_STATUS = "status";
const char Options::SHORT_STATUS = 's' ;
// exclude & endpoint
const char* Options::LONG_EXCLUDE = "exclude";
const char* Options::LONG_ENDPOINT	= "endpoint";
const char Options::SHORT_E = 'e';
// port
const char* Options::LONG_PORT		= "port";
const char Options::SHORT_PORT 	= 'p';
// delegation
const char* Options::LONG_DELEGATION	= "delegationid";
const char Options::SHORT_DELEGATION 	= 'd';
// Semicolon and white-space strings used in the definition of the short options
const char Options::short_required_arg = ':' ;
const char Options::short_no_arg = ' ' ;
/*
*	Long options for job-submit
*/
const struct option Options::submitLongOpts[] = {
	{	Options::LONG_LOGFILE,           	required_argument,		0,		Options::LOGFILE},
        {	Options::LONG_DEBUG,             	no_argument,			0,		Options::DBG},
        {	Options::LONG_AUTODG,           	no_argument,			0,		Options::SHORT_AUTODG},
        {	Options::LONG_REGISTERONLY,	no_argument,			0,		Options::REGISTERONLY},
	{	Options::LONG_PROTO	,	required_argument,		0,		Options::PROTO},
	{	Options::LONG_TRANSFER,	no_argument,			0,		Options::TRANSFER},
	{	Options::LONG_START,		required_argument,		0,		Options::START},
 	{	Options::LONG_COLLECTION,    	required_argument,		0,		Options::COLLECTION},
        {	Options::LONG_DAG,    		required_argument,		0,		Options::DAG},
        {	Options::LONG_DEFJDL,    		required_argument,		0,		Options::DEFJDL},
        {	Options::LONG_DELEGATION,  	required_argument,		0,		Options::SHORT_DELEGATION},
        {	Options::LONG_ENDPOINT,        	required_argument,		0,		Options::SHORT_E},
        {	Options::LONG_VO,             	required_argument,		0,		Options::VO	},
	{	Options::LONG_LRMS,              	required_argument,		0,		Options::LRMS},
	{	Options::LONG_TO,              	required_argument,		0,		Options::TO},
	{	Options::LONG_OUTPUT,            	required_argument,		0,		Options::SHORT_OUTPUT},
	{ 	Options::LONG_INPUT,              	required_argument,		0,		Options::SHORT_INPUT},
	{	Options::LONG_CONFIG,            	required_argument,		0,		Options::SHORT_CONFIG},
	{	Options::LONG_NODESRES,  	required_argument,		0,		Options::NODESRES},
	{	Options::LONG_RESOURCE,  	required_argument,		0,		Options::SHORT_RESOURCE},
	{	Options::LONG_VALID,              	required_argument,		0,		Options::SHORT_V},
	{	Options::LONG_NOMSG,		no_argument,			0,		Options::NOMSG	},
	{	Options::LONG_NOLISTEN,		no_argument,			0,		Options::NOLISTEN	},
	{	Options::LONG_NOINT,		no_argument,			0,		Options::NOINT	},
	{	Options::LONG_VERSION,		no_argument,			0,		Options::VERSION	},
	{	Options::LONG_HELP,			no_argument,			0,		Options::HELP	},
	{0, 0, 0, 0}
};
/*
*	Long options for job-status
*/
const struct option Options::statusLongOpts[] = {
	{	Options::LONG_VERSION,		no_argument,			0,		Options::VERSION	},
	{	Options::LONG_HELP,			no_argument,			0,		Options::HELP	},
	{	Options::LONG_ALL,			no_argument,			0,		Options::ALL	},
	{ 	Options::LONG_INPUT,              	required_argument,		0,		Options::SHORT_INPUT},
	{ 	Options::LONG_VERBOSE,         	required_argument,		0,		Options::SHORT_V},
	{	Options::LONG_FROM,              	required_argument,		0,		Options::FROM},
	{	Options::LONG_TO,              	required_argument,		0,		Options::TO},
	{	Options::LONG_CONFIG,            	required_argument,		0,		Options::SHORT_CONFIG},
	{	Options::LONG_USERTAG,         	required_argument,		0,		Options::USERTAG	},
	{	Options::LONG_STATUS,         	required_argument,		0,		Options::SHORT_STATUS},
	{	Options::LONG_EXCLUDE,         	required_argument,		0,		Options::SHORT_E},
	{	Options::LONG_OUTPUT,            	required_argument,		0,		Options::SHORT_OUTPUT},
	{	Options::LONG_NOINT,		no_argument,			0,		Options::NOINT	},
        {	Options::LONG_VO,             	required_argument,		0,		Options::VO	},
	{	Options::LONG_DEBUG,		no_argument,			0,		Options::DBG	},
	{	Options::LONG_LOGFILE,             required_argument,		0,		Options::LOGFILE},
	{0, 0, 0, 0}
};

/*
*	Long options for  job-logging-info
*/
const struct option Options::loginfoLongOpts[] = {
{	Options::LONG_VERSION,			no_argument,			0,		Options::VERSION	},
{	Options::LONG_HELP,				no_argument,			0,		Options::HELP	},
	{ 	Options::LONG_VERBOSE,         required_argument,		0,		Options::SHORT_V},
	{	Options::LONG_CONFIG,            required_argument,		0,		Options::SHORT_CONFIG},
        {	Options::LONG_VO,             	required_argument,		0,		Options::VO	},
	{	Options::LONG_OUTPUT,            required_argument,		0,		Options::SHORT_OUTPUT},
	{	Options::LONG_NOINT,		no_argument,			0,		Options::NOINT	},
	{	Options::LONG_DEBUG,		no_argument,			0,		Options::DBG	},
	{	Options::LONG_LOGFILE,           required_argument,		0,		Options::LOGFILE},
	{0, 0, 0, 0}
};
/*
*	Long options for job-cancel
*/
const struct option Options::cancelLongOpts[] = {
	{	Options::LONG_VERSION,		no_argument,			0,		Options::VERSION	},
	{	Options::LONG_HELP,			no_argument,			0,		Options::HELP	},
	{ 	Options::LONG_INPUT,          	required_argument,		0,		Options::SHORT_INPUT},
	{	Options::LONG_CONFIG,    		required_argument,		0,		Options::SHORT_CONFIG},
	{	Options::LONG_OUTPUT,       	required_argument,		0,		Options::SHORT_OUTPUT},
	{	Options::LONG_NOINT,		no_argument,			0,		Options::NOINT	},
	{	Options::LONG_DEBUG,		no_argument,			0,		Options::DBG	},
	{	Options::LONG_LOGFILE,      	required_argument,		0,		Options::LOGFILE},
        {	Options::LONG_VO,             	 required_argument,	0,		Options::VO	},
	{0, 0, 0, 0}
};
/*
*	Long options for job-list-match
*/
const struct option Options::lsmatchLongOpts[] = {
	{	Options::LONG_VERSION,		no_argument,			0,		Options::VERSION},
	{	Options::LONG_HELP,			no_argument,			0,		Options::HELP	},
        {	Options::LONG_AUTODG,           no_argument,			0,		Options::SHORT_AUTODG},
        {	Options::LONG_DELEGATION,  	required_argument,		0,		Options::SHORT_DELEGATION},
 	{	Options::LONG_ENDPOINT,        required_argument,		0,		Options::SHORT_E},
	{ 	Options::LONG_RANK,              	no_argument,			0,		Options::RANK},
	{	Options::LONG_CONFIG,             required_argument,		0,		Options::SHORT_CONFIG},
        {	Options::LONG_VO,             	required_argument,		0,		Options::VO	},
	{	Options::LONG_OUTPUT,             required_argument,		0,		Options::SHORT_OUTPUT},
	{	Options::LONG_NOINT,		no_argument,			0,		Options::NOINT	},
	{ 	Options::LONG_DEBUG,              no_argument,			0,		Options::DBG},
	{	Options::LONG_LOGFILE,             required_argument,		0,		Options::LOGFILE},
	{0, 0, 0, 0}
};
/*
*	Long options for job-output
*/
const struct option Options::outputLongOpts[] = {
	{	Options::LONG_VERSION,		no_argument,			0,	Options::VERSION	},
	{	Options::LONG_HELP,			no_argument,			0,	Options::HELP	},
	{ 	Options::LONG_OUTPUT,       	required_argument,		0,	Options::SHORT_OUTPUT},
	{ 	Options::LONG_INPUT,        	required_argument,		0,	Options::SHORT_INPUT},
	{	Options::LONG_LISTONLY,		no_argument,			0,	Options::LISTONLY},
	{	Options::LONG_PROTO	,	required_argument,		0,	Options::PROTO},
	{ 	Options::LONG_DIR, 	        	required_argument,		0,	Options::DIR},
	{	Options::LONG_CONFIG,    		required_argument,		0,	Options::SHORT_CONFIG},
        {	Options::LONG_VO,           		required_argument,		0,	Options::VO},
	{	Options::LONG_NOINT,		no_argument,			0,	Options::NOINT	},
	{ 	Options::LONG_DEBUG,      	no_argument,			0,	Options::DBG},
	{	Options::LONG_LOGFILE,    	required_argument,		0,	Options::LOGFILE},
	{0, 0, 0, 0}
};

/*
*	Long options for  job-attach
*/
const struct option Options::attachLongOpts[] = {
	{	Options::LONG_VERSION,		no_argument,			0,		Options::VERSION	},
	{	Options::LONG_HELP,			no_argument,			0,		Options::HELP	},
	{	Options::LONG_PORT,              	required_argument,		0,		Options::SHORT_PORT},
	{	Options::LONG_NOLISTEN,		no_argument,			0,		Options::NOLISTEN	},
	{	Options::LONG_CONFIG,            required_argument,		0,		Options::SHORT_CONFIG},
	{	Options::LONG_VO,           		required_argument,		0,		Options::VO},
	{ 	Options::LONG_INPUT,              	required_argument,		0,		Options::SHORT_INPUT},
	{	Options::LONG_NOINT,		no_argument,			0,		Options::NOINT	},
	{ 	Options::LONG_DEBUG,              no_argument,			0,		Options::DBG},
	{	Options::LONG_LOGFILE,             required_argument,		0,		Options::LOGFILE},
	{0, 0, 0, 0}
};
/*
*	Long options for proxy-delegation
*/
const struct option Options::delegationLongOpts[] = {
	{	Options::LONG_VERSION,		no_argument,			0,		Options::VERSION	},
	{	Options::LONG_LOGFILE,		required_argument,		0,		Options::LOGFILE},
	{	Options::LONG_DEBUG,             	no_argument,			0,		Options::DBG},
	{	Options::LONG_AUTODG,           no_argument,			0,		Options::SHORT_AUTODG},
	{	Options::LONG_DELEGATION,  	required_argument,		0,		Options::SHORT_DELEGATION},
	{	Options::LONG_ENDPOINT,        	required_argument,		0,		Options::SHORT_E},
	{	Options::LONG_CONFIG,    		required_argument,		0,		Options::SHORT_CONFIG},
	{	Options::LONG_VO,           		required_argument,		0,		Options::VO},
	{	Options::LONG_OUTPUT,            required_argument,	0,	Options::SHORT_OUTPUT},
	{	Options::LONG_NOINT,		no_argument,		0,	Options::NOINT	},
	{	Options::LONG_HELP,			no_argument,			0,		Options::HELP	},
	{0, 0, 0, 0}
};
/*
*	Long options for proxy-info
*/
const struct option Options::proxyInfoLongOpts[] = {
	{	Options::LONG_VERSION,		no_argument,			0,		Options::VERSION},
	{	Options::LONG_LOGFILE,		required_argument,		0,		Options::LOGFILE},
	{	Options::LONG_DEBUG,             	no_argument,			0,		Options::DBG},
	{	Options::LONG_DELEGATION,  	required_argument,		0,		Options::SHORT_DELEGATION},
	{	Options::LONG_ENDPOINT,        	required_argument,		0,		Options::SHORT_E},
	{	Options::LONG_CONFIG,    		required_argument,		0,		Options::SHORT_CONFIG},
	{	Options::LONG_VO,           		required_argument,		0,		Options::VO},
	{	Options::LONG_OUTPUT,             required_argument,		0,		Options::SHORT_OUTPUT},
	{	Options::LONG_NOINT,		no_argument,			0,		Options::NOINT	},
	{	Options::LONG_HELP,			no_argument,			0,		Options::HELP	},
	{0, 0, 0, 0}
};
/*
*	Long options for  job-perusal
*/
const struct option Options::perusalLongOpts[]  = {
	{	Options::LONG_ALL			,no_argument,			0,	Options::ALL	},
	{	Options::LONG_VERSION,		no_argument,			0,	Options::VERSION	},
	{	Options::LONG_HELP,			no_argument,			0,	Options::HELP	},
	{	Options::LONG_GET,			no_argument,			0,	Options::GET},
	{ 	Options::LONG_SET, 	        	no_argument,			0,	Options::SET},
	{ 	Options::LONG_UNSET, 	        no_argument,			0,	Options::UNSET},
	{ 	Options::LONG_FILENAME, 	required_argument,		0,	Options::SHORT_FILENAME},
	{ 	Options::LONG_INPUT,        	required_argument,		0,	Options::SHORT_INPUT},
	{	Options::LONG_PROTO	,	required_argument,		0,	Options::PROTO},
	{ 	Options::LONG_DIR,        		required_argument,		0,	Options::DIR},
	{ 	Options::LONG_OUTPUT,        	required_argument,		0,	Options::SHORT_OUTPUT},
	{	Options::LONG_CONFIG,    		required_argument,		0,	Options::SHORT_CONFIG},
        {	Options::LONG_VO,           		required_argument,		0,	Options::VO},
	{	Options::LONG_NODISPLAY,	no_argument,			0,	Options::NODISPLAY	},
	{	Options::LONG_NOINT,		no_argument,			0,	Options::NOINT	},
	{ 	Options::LONG_DEBUG,      	no_argument,			0,	Options::DBG},
	{	Options::LONG_LOGFILE,    	required_argument,		0,	Options::LOGFILE},
	{	Options::LONG_INPUTFILE,    	required_argument,		0,	Options::INPUTFILE},
	{0, 0, 0, 0}
};
/*
*	Short usage constants
*/
const string Options::USG_ALL = "--" + string(LONG_ALL) ;

const string Options::USG_AUTODG = "--" + string(LONG_AUTODG) + ", -" + SHORT_AUTODG ;

const string Options::USG_CHKPT = "--" + string(LONG_CHKPT )	 + "\t\t<file_path>" ;

const string Options::USG_COLLECTION = "--" + string(LONG_COLLECTION)	 + "\t<dir_path>" ;

const string Options::USG_CONFIG = "--" + string(LONG_CONFIG ) +  ", -" + SHORT_CONFIG  + "\t<file_path>"	;

const string Options::USG_DAG = "--" + string(LONG_DAG)	 + "\t<dir_path>" ;

const string Options::USG_DEBUG  = "--" + string(LONG_DEBUG );

const string Options::USG_DEFJDL = "--" + string(LONG_DEFJDL)	 + "\t\t<file_path>" ;

const string Options::USG_DELEGATION  = "--" + string(LONG_DELEGATION )+ ", -" + SHORT_DELEGATION + "\t<id_string>";

const string Options::USG_DIR  = "--" + string(LONG_DIR )+ "\t\t<directory_path>"	;

const string Options::USG_ENDPOINT  = "--" + string(LONG_ENDPOINT )+ ", -" + SHORT_E + "\t<service_URL>";

const string Options::USG_EXCLUDE  = "--" + string(LONG_EXCLUDE )+ ", -" + SHORT_E + "\t<status_value>";

const string Options::USG_FILENAME = "--" + string(LONG_FILENAME) + ", -" + SHORT_FILENAME +  "\t<filename>";

const string Options::USG_FROM  = "--" + string(LONG_FROM )+ "\t\t[MM:DD:]hh:mm[:[CC]YY]";

const string Options::USG_GET  = "--" + string(LONG_GET ) ;

const string Options::USG_HELP = "--" + string(LONG_HELP) ;

const string Options::USG_INPUT = "--" + string(LONG_INPUT )  + ", -" + SHORT_INPUT  + "\t<file_path>";

const string Options::USG_INPUTFILE = "--" + string(LONG_INPUTFILE) + "\t<file_path>";

const string Options::USG_LISTONLY = "--" + string(LONG_LISTONLY) ;

const string Options::USG_LRMS = "--" + string(LONG_LRMS ) + "\t\t<lrms_type>" 	;

const string Options::USG_LOGFILE = "--" + string(LONG_LOGFILE )+ "\t<file_path>" ;

const string Options::USG_NODESRES = "--" + string(LONG_NODESRES)+ " <ce_id>" ;

const string Options::USG_NODISPLAY = "--" + string(LONG_NODISPLAY);

const string Options::USG_NOGUI = "--" + string(LONG_NOGUI);

const string Options::USG_NOINT = "--" + string(LONG_NOINT) ;

const string Options::USG_NOLISTEN  = "--" + string(LONG_NOLISTEN);

const string Options::USG_NOMSG	 = "--" + string(LONG_NOMSG);

const string Options::USG_OUTPUT = "--" + string(LONG_OUTPUT) + ", -" + SHORT_OUTPUT + "\t<file_path>";

const string Options::USG_PORT  = "--" + string(LONG_PORT )+ ", -" + SHORT_PORT + "\t<port_num>";

const string Options::USG_PROTO  = "--" + string(LONG_PROTO ) + "\t\t<protocol>";

const string Options::USG_RANK = "--" + string(LONG_RANK ) ;

const string Options::USG_REGISTERONLY = "--" + string(LONG_REGISTERONLY) ;

const string Options::USG_RESOURCE = "--" + string(LONG_RESOURCE ) + ", -" + SHORT_RESOURCE + "\t<ce_id>";

const string Options::USG_SET  = "--" + string(LONG_SET) ;

const string Options::USG_START = "--" + string(LONG_START) + "\t\t<jobid>";

const string Options::USG_STATUS = "--" + string(LONG_STATUS ) + ", -" + SHORT_STATUS + "\t<status_value>";

const string Options::USG_TO = "--" + string(LONG_TO) + "\t\t[MM:DD:]hh:mm[:[CC]YY]";

const string Options::USG_TRANSFER = "--" + string(LONG_TRANSFER ) ;

const string Options::USG_UNSET  = "--" + string(LONG_UNSET) ;

const string Options::USG_USERTAG = "--" + string(LONG_USERTAG ) + "\t<tag name>=<tag value>";

const string Options::USG_VALID = "--" + string(LONG_VALID ) +  ", -" + SHORT_V + "\thh:mm";

const string Options::USG_VERBOSE  = "--" + string(LONG_VERBOSE ) +  ", -" + SHORT_V + "\t\t[0|1|2|3]";

const string Options::USG_VERSION = "--" + string(LONG_VERSION );

const string Options::USG_VO	 = "--" + string(LONG_VO ) + "\t\t<vo_name>";

/*
*	Prints the help usage message for the job-submit
*	@param exename the name of the executable
*	@param long_usage if the value is true it prints the long help msg
*/
void Options::submit_usage(const char* &exename, const bool &long_usg){
	cerr << "\n" << Options::getVersionMessage( ) << "\n" ;
	cerr << "Usage: " << exename <<   " <delegation-opts> [options]  <jdl_file>\n\n";
	cerr << "delegation-opts:\n" ;
	cerr << "\t" << USG_DELEGATION << "\n";
	cerr << "\t" << USG_AUTODG << "\n\n";
	cerr << "options:\n" ;
	cerr << "\t" << USG_HELP << "\n\n";
	cerr << "\t" << USG_VERSION << "\n\n";
        cerr << "\t" << USG_ENDPOINT << "\n";
	cerr << "\t" << USG_INPUT << "\n";
	cerr << "\t" << USG_RESOURCE << "\n";
	cerr << "\t" << USG_NODESRES << "\n";
	cerr << "\t" << USG_NOLISTEN << "\n";
	cerr << "\t" << USG_NOMSG << "\n";
	cerr << "\t" << USG_LRMS << "\n";
	cerr << "\t" << USG_TO << "\n";
	cerr << "\t" << USG_VALID << "\n";
	cerr << "\t" << USG_CONFIG << "\n";
        cerr << "\t" << USG_VO << "\n";
        cerr << "\t" << USG_REGISTERONLY << "\n";
	cerr << "\t" << USG_TRANSFER << " (*)\n";
	cerr << "\t" << USG_PROTO << "\n";
	cerr << "\t" << USG_START << "\n";
	cerr << "\t" << USG_OUTPUT << "\n";
	cerr << "\t" << USG_NOINT << "\n";
	cerr << "\t" << USG_DEBUG << "\n";
	cerr << "\t" << USG_LOGFILE << "\n";
	cerr << "\t" << USG_DEFJDL << "\n";
        cerr << "\t" << USG_DAG << " (**)\n";
        cerr << "\t" << USG_COLLECTION << " (**)\n\n";
        cerr << "\t" << "(*) To be used only with " << USG_REGISTERONLY  << "\n";
        cerr << "\t" << "(**) Using this option you MUSTN'T specified any JDL file\n\n";
	cerr << "Please report any bug at:\n" ;
	cerr << "\t" << HELP_EMAIL << "\n";
	if (long_usg){
		cerr  << exename << " full help\n\n" ;
	}
};

/*
*	Prints the help usage message for the job-status
*	@param exename the name of the executable
*	@param long_usage if the value is true it prints the long help msg
*/
void Options::status_usage(const char* &exename, const bool &long_usg){
	cerr <<  "\n\n" << Options::getVersionMessage( ) << "\n" ;
	cerr << "Usage: " << exename <<   " [options]   <job Id(s)>\n\n";
	cerr << "options:\n" ;
	cerr << "\t" << USG_HELP << "\n";
	cerr << "\t" << USG_VERSION << "\n\n";
	cerr << "\t" << USG_ALL << "\n";
	cerr << "\t" << USG_VERBOSE << "\n";
	cerr << "\t" << USG_FROM << "\n";
	cerr << "\t" << USG_TO << "\n";
	cerr << "\t" << USG_CONFIG << "\n";
        cerr << "\t" << USG_VO << "\n";
	cerr << "\t" << USG_USERTAG<< "\n";
	cerr << "\t" << USG_STATUS << "\n";
	cerr << "\t" << USG_EXCLUDE << "\n";
	cerr << "\t" << USG_OUTPUT << "\n";
	cerr << "\t" << USG_NOINT << "\n";
	cerr << "\t" << USG_DEBUG << "\n";
	cerr << "\t" << USG_LOGFILE << "\n\n";
	cerr << "Please report any bug at:\n" ;
	cerr << "\t" << HELP_EMAIL << "\n";
	if (long_usg){
		cerr  << exename << " full help\n\n" ;
	}
};
/*
*	Prints the help usage message for the job-logging-info
*	@param exename the name of the executable
*	@param long_usage if the value is true it prints the long help msg
*/
void Options::loginfo_usage(const char* &exename, const bool &long_usg){
	cerr << "\n" << Options::getVersionMessage( ) << "\n" ;
	cerr << "Usage: " << exename <<   " [options]   <job Id(s)>\n\n";
	cerr << "options:\n" ;
	cerr << "\t" << USG_HELP << "\n";
	cerr << "\t" << USG_VERSION << "\n\n";
	cerr << "\t" << USG_VERBOSE << "\n";
	cerr << "\t" << USG_OUTPUT << "\n";
        cerr << "\t" << USG_CONFIG << "\n";
        cerr << "\t" << USG_VO << "\n";
	cerr << "\t" << USG_NOINT << "\n";
	cerr << "\t" << USG_DEBUG << "\n";
	cerr << "\t" << USG_LOGFILE << "\n\n";
	cerr << "Please report any bug at:\n" ;
	cerr << "\t" << HELP_EMAIL << "\n";
	if (long_usg){
		cerr  << exename << " full help\n\n" ;
	}
};
/*
*	Prints the help usage message for the job-cancel
*	@param exename the name of the executable
*	@param long_usage if the value is true it prints the long help msg
*/
void Options::cancel_usage(const char* &exename, const bool &long_usg){
	cerr << "\n" << Options::getVersionMessage( ) << "\n" ;
	cerr << "Usage: " << exename <<   " [options]   <job Id(s)>\n\n";
	cerr << "options:\n" ;
	cerr << "\t" << USG_HELP << "\n";
	cerr << "\t" << USG_VERSION << "\n\n";
	cerr << "\t" << USG_INPUT << "\n";
	cerr << "\t" << USG_CONFIG << "\n";
        cerr << "\t" << USG_VO << "\n";
	cerr << "\t" << USG_OUTPUT << "\n";
	cerr << "\t" << USG_NOINT << "\n";
	cerr << "\t" << USG_DEBUG << "\n";
	cerr << "\t" << USG_LOGFILE << "\n\n";
	cerr << "Please report any bug at:\n" ;
	cerr << "\t" << HELP_EMAIL << "\n";
	if (long_usg){
		cerr  << exename << " full help\n\n" ;
	}
};

/*
*	Prints the help usage message for the job-list-match
*	@param exename the name of the executable
*	@param long_usage if the value is true it prints the long help msg
*/
void Options::lsmatch_usage(const char* &exename, const bool &long_usg){
	cerr << "\n" << Options::getVersionMessage( ) << "\n" ;
        cerr << "Usage: " << exename <<   " <delegation-opts> [options]  <jdl_file>\n\n";
	cerr << "delegation-opts:\n" ;
	cerr << "\t" << USG_DELEGATION << "\n";
	cerr << "\t" << USG_AUTODG << "\n\n";
	cerr << "options:\n" ;
	cerr << "\t" << USG_HELP << "\n";
	cerr << "\t" << USG_VERSION << "\n\n";
        cerr << "\t" << USG_ENDPOINT << "\n";
	cerr << "\t" << USG_RANK << "\n";
	cerr << "\t" << USG_CONFIG << "\n";
        cerr << "\t" << USG_VO << "\n";
	cerr << "\t" << USG_OUTPUT << "\n";
	cerr << "\t" << USG_NOINT << "\n";
	cerr << "\t" << USG_DEBUG << "\n";
	cerr << "\t" << USG_LOGFILE << "\n\n";
	cerr << "Please report any bug at:\n" ;
	cerr << "\t" << HELP_EMAIL << "\n";
	if (long_usg){
		cerr  << exename << " full help\n\n" ;
	}
};
/*
*	Prints the help usage message for the job-output
*	@param exename the name of the executable
*	@param long_usage if the value is true it prints the long help msg
*/
void Options::output_usage(const char* &exename, const bool &long_usg){
	cerr << "\n" << Options::getVersionMessage( ) << "\n" ;
	cerr << "Usage: " << exename <<   " [options]   <job Id(s)>\n\n";
	cerr << "options:\n" ;
	cerr << "\t" << USG_HELP << "\n";
	cerr << "\t" << USG_VERSION << "\n\n";
	cerr << "\t" << USG_INPUT << "\n";
	cerr << "\t" << USG_DIR << "\n";
	cerr << "\t" << USG_PROTO << "\n";
	cerr << "\t" << USG_CONFIG << "\n";
        cerr << "\t" << USG_VO << "\n";
        cerr << "\t" << USG_LISTONLY << "\n";
	cerr << "\t" << USG_NOINT << "\n";
	cerr << "\t" << USG_DEBUG << "\n";
	cerr << "\t" << USG_LOGFILE << "\n\n";
	cerr << "Please report any bug at:\n" ;
	cerr << "\t" << HELP_EMAIL << "\n";
	if (long_usg){
		cerr  << exename << " full help\n\n" ;
	}
};
/*
*	Prints the help usage message for the job-submit
*	@param exename the name of the executable
*	@param long_usage if the value is true it prints the long help msg
*/
void Options::delegation_usage(const char* &exename, const bool &long_usg){
	cerr << "\n" << Options::getVersionMessage( ) << "\n" ;
	cerr << "Usage: " << exename <<   " <delegation-opts> [options]\n\n";
        cerr << "delegation-opts:\n" ;
	cerr << "\t" << USG_DELEGATION << "\n";
	cerr << "\t" << USG_AUTODG << "\n\n";
	cerr << "options:\n" ;
	cerr << "\t" << USG_HELP << "\n";
        cerr << "\t" << USG_ENDPOINT << "\n";
	cerr << "\t" << USG_CONFIG << "\n";
        cerr << "\t" << USG_VO << "\n";
	cerr << "\t" << USG_OUTPUT << "\n";
	cerr << "\t" << USG_NOINT << "\n";
	cerr << "\t" << USG_DEBUG << "\n";
	cerr << "\t" << USG_LOGFILE << "\n\n";
	cerr << "Please report any bug at:\n" ;
	cerr << "\t" << HELP_EMAIL << "\n";
	if (long_usg){
		cerr  << exename << " full help\n\n" ;
	}
};
/*
*	Prints the help usage message for the job-submit
*	@param exename the name of the executable
*	@param long_usage if the value is true it prints the long help msg
*/
void Options::proxyinfo_usage(const char* &exename, const bool &long_usg){
	cerr << "\n" << Options::getVersionMessage( ) << "\n" ;
	cerr << "Usage: " << exename <<   " [options] [ -d <deleg Id> | <job Id> ]\n\n";
        cerr << "options:\n" ;
	cerr << "\t" << USG_HELP << "\n";
        cerr << "\t" << USG_ENDPOINT << "\n";
	cerr << "\t" << USG_CONFIG << "\n";
        cerr << "\t" << USG_VO << "\n";
	cerr << "\t" << USG_OUTPUT << "\n";
	cerr << "\t" << USG_NOINT << "\n";
	cerr << "\t" << USG_DEBUG << "\n";
	cerr << "\t" << USG_LOGFILE << "\n\n";
	cerr << "identifier:\n" ;
	cerr << "\t" << USG_DELEGATION << " = the identifier of a previously delegated proxy\n";
	cerr << "\tor <job Id> = the identifier of a prevoiusly submitted job\n";
	cerr << "Please report any bug at:\n" ;
	cerr << "\t" << HELP_EMAIL << "\n";
	if (long_usg){
		cerr  << exename << " full help\n\n" ;
	}
};/*
*	Prints the help usage message for the job-output
*	@param exename the name of the executable
*	@param long_usage if the value is true it prints the long help msg
*/
void Options::perusal_usage(const char* &exename, const bool &long_usg){
	cerr << "\n" << Options::getVersionMessage( ) << "\n" ;
	cerr << "Usage: " << exename <<   "  <operation options> <file options> [other options]  <job Id>\n\n";
	cerr << "operation options (mandatory):\n";
	cerr << "\t" << USG_GET << "\n";
	cerr << "\t" << USG_SET << "\n";
	cerr << "\t" << USG_UNSET << "\n";
	cerr << "\nfile options: (mandatory for set and get operations)\n";
	cerr << "\t" << USG_FILENAME << " (*)\n";
	cerr << "\t" << USG_INPUTFILE << "\n";
	cerr << "\nother options:\n" ;
	cerr << "\t" << USG_HELP << "\n";
	cerr << "\t" << USG_VERSION << "\n";
	cerr << "\t" << USG_CONFIG << "\n";
        cerr << "\t" << USG_VO << "\n";
	cerr << "\t" << USG_DIR << "\n";
	cerr << "\t" << USG_PROTO << "\n";	
	cerr << "\t" << USG_ALL << " (**)\n";
	cerr << "\t" << USG_OUTPUT << "\n";
        cerr << "\t" << USG_NODISPLAY << "\n";
	cerr << "\t" << USG_NOINT << "\n";
	cerr << "\t" << USG_DEBUG << "\n";
	cerr << "\t" << USG_LOGFILE << "\n\n";
	cerr << "\t" << "(*) With " <<  USG_SET << " multiple files can be specified by repeating the option several times\n";
	cerr << "\t" << "(**) only with " <<  USG_GET << " to returns all chunks of the given file\n\n";
	cerr << "Please report any bug at:\n" ;
	cerr << "\t" << HELP_EMAIL << "\n";
	if (long_usg){
		cerr  << exename << " full help\n\n" ;
	}
};
/*
*	Prints the help usage message for the job-attach
*	@param exename the name of the executable
*	@param long_usage if the value is true it prints the long help msg
*/
void Options::attach_usage(const char* &exename, const bool &long_usg){
	cerr << "\n" << Options::getVersionMessage( ) << "\n" ;
	cerr << "Usage: " << exename <<   " [options]   <job Id>\n\n";
	cerr << "options:\n" ;
	cerr << "\t" << USG_HELP << "\n";
	cerr << "\t" << USG_VERSION << "\n";
	cerr << "\t" << USG_PORT << "\n";
	cerr << "\t" << USG_NOLISTEN << "\n";
	cerr << "\t" << USG_CONFIG << "\n";
 	cerr << "\t" << USG_VO << "\n";
	cerr << "\t" << USG_INPUT << "\n";
	cerr << "\t" << USG_NOINT << "\n";
	cerr << "\t" << USG_DEBUG << "\n";
	cerr << "\t" << USG_LOGFILE << "\n\n";
	cerr << "Please report any bug at:\n" ;
	cerr << "\t" << HELP_EMAIL << "\n";
	if (long_usg){
		cerr  << exename << " full help\n\n" ;
	}
};
/*
*	Default constructor
*	@param command command to be handled
*/
Options::Options (const WMPCommands &command){
	jdlFile = NULL ;
	// init of the string attributes
	chkpt = NULL;
        collection = NULL;
	config = NULL;
	dag = NULL;
        delegation = NULL;
	def_jdl = NULL;
	dir = NULL;
        endpoint = NULL;
	exclude = NULL;
	from = NULL;
	input = NULL;
	lrms = NULL;
	logfile = NULL;
	nodesres = NULL;
	output = NULL;
	resource = NULL ;
	start = NULL;
	status = NULL;
	to = NULL;
	valid = NULL ;
        vo = NULL ;
	inputfile = NULL;
	// init of the boolean attributes
	all  = false ;
        autodg = false;
	debug  = false ;
	get = false;
	help = false  ;
        listonly = false;
	nodisplay = false ;
	nogui  = false ;
	noint  = false ;
	nolisten = false  ;
	nomsg = false  ;
	rank = false  ;
	set = false;
        registeronly = false;
	transfer = false;
	unset = false;
	version  = false ;
        // verbosity level
        verbosityLevel = WMSLOG_UNDEF;
	//application name
        applName = "";
	// init of the numerical attributes
	port = NULL ;
	verbosity = NULL ;
	// Default protocol for File Transfer
	fileprotocol = NULL;
	// definitions of short and long options
	switch (command){
		case (JOBSUBMIT) :{
			// short options
			asprintf (&shortOpts,
				"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				Options::SHORT_E, 			short_required_arg,
				Options::SHORT_AUTODG, 		short_no_arg,
				Options::SHORT_DELEGATION, 	short_required_arg,
				Options::SHORT_OUTPUT, 		short_required_arg,
				Options::SHORT_INPUT,		short_required_arg,
				Options::SHORT_CONFIG,		short_required_arg,
				Options::SHORT_RESOURCE,	short_required_arg,
				Options::SHORT_V,			short_required_arg //valid
			);
			// long options
			longOpts = submitLongOpts ;
			numOpts = (sizeof(submitLongOpts)/sizeof(option)) -1;
			break ;
		} ;
		case(JOBSTATUS):
		{
			// short options
			asprintf (&shortOpts,
				"%c%c%c%c%c%c%c%c%c%c" ,
				Options::SHORT_E,  		short_required_arg, // endpoint
				Options::SHORT_OUTPUT, 	short_required_arg,
				Options::SHORT_INPUT,	short_required_arg,
				Options::SHORT_CONFIG,	short_required_arg,
				Options::SHORT_V,		short_required_arg//verbosity
			);
			// long options
			longOpts = statusLongOpts ;
			numOpts = (sizeof(statusLongOpts)/sizeof(option)) -1;
			break;
		} ;
		case(JOBLOGINFO) :{
			// short options
			asprintf (&shortOpts,
				"%c%c%c%c%c%c%c%c" ,
				Options::SHORT_OUTPUT, 	short_required_arg,
				Options::SHORT_INPUT,	short_required_arg,
				Options::SHORT_CONFIG,	short_required_arg,
				Options::SHORT_V,		short_required_arg //verbosity
			);
			// long options
			longOpts = loginfoLongOpts ;
			numOpts = (sizeof(loginfoLongOpts)/sizeof(option)) -1;
			break;
		} ;
		case(JOBCANCEL) :
		{
			// short options
			asprintf (&shortOpts,
				"%c%c%c%c%c%c" ,
				Options::SHORT_OUTPUT, 		short_required_arg,
				Options::SHORT_INPUT,		short_required_arg,
				Options::SHORT_CONFIG,		short_required_arg
			);
			// long options
			longOpts = cancelLongOpts ;
			numOpts = (sizeof(cancelLongOpts)/sizeof(option)) -1;
			break;
		} ;
		case(JOBMATCH) :{
			// short options
			asprintf (&shortOpts,
				"%c%c%c%c%c%c%c%c%c%c" ,
				Options::SHORT_E,  			short_required_arg, // endpoint
				Options::SHORT_AUTODG, 		short_no_arg,
                                Options::SHORT_DELEGATION, 	short_required_arg,
				Options::SHORT_OUTPUT, 		short_required_arg,
				Options::SHORT_CONFIG,		short_required_arg
			);
			// long options
			longOpts = lsmatchLongOpts ;
			numOpts = (sizeof(lsmatchLongOpts)/sizeof(option)) -1;
			break;
		} ;
		case(JOBOUTPUT) :{
			// short options
			asprintf (&shortOpts,
				"%c%c%c%c" ,
				Options::SHORT_INPUT,	short_required_arg,
				Options::SHORT_CONFIG,	short_required_arg
			);
			// long options
			longOpts = outputLongOpts ;
			numOpts = (sizeof(outputLongOpts)/sizeof(option)) -1;
			break;
		} ;
		case(JOBATTACH) :{
			// short options
			asprintf (&shortOpts,
				"%c%c%c%c%c%c" ,
				Options::SHORT_PORT,	 	short_required_arg,
				Options::SHORT_INPUT,		short_required_arg,
				Options::SHORT_CONFIG,		short_required_arg);
			// long options
			longOpts = attachLongOpts ;
			numOpts = (sizeof(attachLongOpts)/sizeof(option)) -1;
			break;
		} ;
                case (JOBDELEGATION) :{
			// short options
			asprintf (&shortOpts,
				"%c%c%c%c%c%c%c%c%c%c",
				Options::SHORT_E,  			short_required_arg, // endpoint
				Options::SHORT_AUTODG, 		short_no_arg,
				Options::SHORT_DELEGATION, 	short_required_arg,
				Options::SHORT_OUTPUT, 		short_required_arg,
				Options::SHORT_CONFIG,		short_required_arg);

			// long options
			longOpts = delegationLongOpts ;
			numOpts = (sizeof(delegationLongOpts)/sizeof(option)) -1;
			break ;
		} ;
		case (JOBPROXYINFO) :{
			// short options
			asprintf (&shortOpts,
				"%c%c%c%c%c%c%c%c",
				Options::SHORT_E,  			short_required_arg, // endpoint
				Options::SHORT_DELEGATION, 	short_required_arg,
				Options::SHORT_OUTPUT, 		short_required_arg,
				Options::SHORT_CONFIG,		short_required_arg);

			// long options
			longOpts = proxyInfoLongOpts ;
			numOpts = (sizeof(proxyInfoLongOpts)/sizeof(option)) -1;
			break ;
		} ;
		case (JOBPERUSAL) :{
			// short options
			asprintf (&shortOpts,
				"%c%c%c%c%c%c%c%c",
				Options::SHORT_INPUT, 		short_required_arg,
				Options::SHORT_OUTPUT, 		short_required_arg,
				Options::SHORT_CONFIG,		short_required_arg,
				Options::SHORT_FILENAME,	short_required_arg);

			// long options
			longOpts = perusalLongOpts ;
			numOpts = (sizeof(perusalLongOpts)/sizeof(option)) -1;
			break ;
		} ;
		default : {
			throw WmsClientException(__FILE__,__LINE__,"Options",
				DEFAULT_ERR_CODE,
				"Wrong Input Parameter",
                                "unknown command");
		} ;
	};
	// command type attribute
	cmdType = command ;
};
/**
* Default destructor
*/
Options::~Options( ) {
	if (chkpt) { delete(chkpt);}
	if (collection ) { delete(collection );}
	if (config) { delete( config);}
	if (dag) { delete(dag);}
	if (def_jdl) { delete(def_jdl);}
	if (delegation) { delete( delegation);}
	if (dir) { delete(dir );}
	if (endpoint  ) { delete(endpoint  );}
	if ( exclude) { delete( exclude);}
	if (fileprotocol) { delete(fileprotocol);}
	if (from ) { delete( from);}
	if ( input ) { delete(input  );}
	if ( lrms) { delete( lrms);}
	if (logfile ) { delete(logfile );}
	if (nodesres) { delete(nodesres);}
	if (output  ) { delete( output );}
	if (port ) { free(port);}
	if (resource) { delete(resource);}
	if (start ) { delete( start);}
	if (status ) { delete( status);}
	if (to) { delete(to);}
	if (valid ) { delete(valid);}
	if (verbosity  ) { free (verbosity );}
	if (vo) { delete(vo);}
	if (inputfile) { delete(inputfile);}
}
/**
* Returns a string with the version numbers of this client
*/
std::string Options::getVersionMessage( ) {
	ostringstream msg;
	char ws = (char)32;
	msg << Options::HELP_UI << ws << Options::HELP_VERSION << "\n";
        msg << Options::HELP_COPYRIGHT << "\n";
	return msg.str();
}
/**
* Returns a vector with the list of available File Protocols
*/
const std::vector<std::string> Options::getProtocols() {
	vector<string> protos;
	unsigned int size = sizeof(Options::TRANSFER_FILES_PROTOCOLS) / sizeof(char*);
	for (unsigned int i = 0; i < size ; i++){
		protos.push_back(string(Options::TRANSFER_FILES_PROTOCOLS[i]));
	}
	return protos;
}
/**
* Returns a string with the list of available File Protocols
*/
const std::string Options::getProtocolsString() {
	string protos;
	unsigned int size = sizeof(Options::TRANSFER_FILES_PROTOCOLS) / sizeof(char*);
	for (unsigned int i = 0; i < size ; i++){
		if (i>0) { protos += ", ";}
		protos += string(Options::TRANSFER_FILES_PROTOCOLS[i]);
	}
	return protos;
}
/**
* Returns the minimum size allowed for each file in transfer operations.
* The computration is based on the limtation fixed by the required File Transfer Protocol and the limitation
* of the archiving tool (libtar; if zipped feature is allowed).
* If the protocol is not specified and the zipped feature is not allowed, the default value is
* returned
* @param protocol the File Transfer Protocol string (either https or gsiftp ... )
* @param zipped TRUE if Zipped feature is allowed, FALSE otherwise (the default value)
*/
const long Options::getMinimumAllowedFileSize (const std::string &protocol, const bool &zipped) {
	long proto = 0;
	long min = 0;
	if (protocol.size ( ) > 0) {
		if (protocol.compare(TRANSFER_FILES_CURL_PROTO)==0) {
			proto = MAX_CURL_SIZE ;
		} else if (protocol.compare(TRANSFER_FILES_GUC_PROTO)==0) {
			proto = MAX_GUC_SIZE ;
		} else {
			proto = MAX_DEFAULT_FILE_SIZE;
		}
	} else {
		proto = MAX_DEFAULT_FILE_SIZE;
	}
	if (zipped) {
		min = (proto<MAX_TAR_SIZE)?proto:MAX_TAR_SIZE;
	} else {
		min = proto;
	}
	return min ;
}
/**
* Checks whether  a string option is defined for a specific operation
*/
const int Options::checkOpts(const std::string& opt) {
	int r = -1;
	if (opt.compare (0,2,"--")==0){
		string lg = opt.substr(2, (opt.size()-2));
		for (unsigned int i = 0; i < numOpts ; i++){
			struct option s = longOpts[i];
			if (lg.compare(s.name)==0){
				r = 1 ;
				break;
			}
		}
	} else if (opt.compare (0,1,"-")==0){
		for (unsigned int i = 0; i < numOpts ; i++){
			struct option s = longOpts[i];
			string sh = opt.substr(1, (opt.size()-1));
			if (sh.size()==1){
				if (s.val < 128){
					char c = (char)s.val;
					if (sh.compare(string(1,c))==0){
						r = 1;
						break;
					}
				}
			}
		}
	}
	return r ;
};

/*
*	Gets the value of the option string-attribute
*/
string* Options::getStringAttribute (const OptsAttributes &attribute){
	string *value = NULL ;
	switch (attribute){
        	case(COLLECTION) : {
			if (collection){
				value = new string (*collection) ;
			}
			break ;
		}
		case(DIR) : {
			if (dir){
				value = new string (*dir) ;
			}
			break ;
		}
		case(LOGFILE) : {
			if (logfile){
				value = new string (*logfile) ;
			}
			break ;
		}
		case(DAG) : {
			if (dag){
				value = new string (*dag) ;
			}
			break ;
		}
		case(DEFJDL) : {
			if (def_jdl){
				value = new string (*def_jdl) ;
			}
			break ;
		}
                case(DELEGATION) : {
			if (delegation){
				value = new string (*delegation) ;
			}
			break ;
		}
		case(ENDPOINT) : {
			if (endpoint){
				value = new string (*endpoint) ;
			}
			break ;
		}
                case(CHKPT) : {
			if (chkpt){
				value = new string (*chkpt) ;
			}
			break ;
		}
		case(LRMS) : {
			if (lrms){
				value = new string (*lrms) ;
			}
			break ;
		}
		case(VALID) : {
			if (valid){
				value = new string (*valid) ;
			}
			break ;
		}
		case(TO) : {
			if (to){
				value = new string (*to) ;
			}
			break ;
		}
		case(OUTPUT) : {
			if (output){
				value = new string (*output) ;
			}
			break ;
		}
		case(PROTO) : {
			if (fileprotocol){
				value = new string (*fileprotocol) ;
			}
			break ;
		}
		case(INPUT) : {
			if (input){
				value = new string (*input) ;
			}
			break ;
		}
		case(CONFIG) : {
			if (config){
				value = new string (*config) ;
			}
			break ;
		}
                case(VO) : {
			if (vo){
				value = new string (*vo) ;
			}
			break ;
		}
		case(RESOURCE) : {
			if (resource){
				value = new string (*resource) ;
			}
			break ;
		}
		case(NODESRES) : {
			if (nodesres){
				value = new string (*nodesres) ;
			}
			break ;
		}
		case(INPUTFILE) : {
			if (inputfile){
				value = new string (*inputfile) ;
			}
			break ;
		}
		case(START) : {
			value = start;
			break ;
		}
		default : {
			// returns NULL
			break ;
		}
	};
	return value ;
};

/*
*	Gets the value of the option int-attribute
*/
int* Options::getIntAttribute (const OptsAttributes &attribute){
	int *value = NULL ;
	switch (attribute){
		case(PORT) : {
			if (port){
				value = (int*)malloc(sizeof(int));
				*value = *port ;
			}
			break ;
		}
		case(VERBOSE) : {
			if (verbosity){
				value = (int*)malloc(sizeof(int));
				*value = *verbosity ;
			}
			break ;
		}
		default : {
			// returns NULL
			break ;
		}
	};
	return value ;
};
/*
*	Gets the value of the option string-attribute
*/
bool Options::getBoolAttribute (const OptsAttributes &attribute){
	bool value = false ;
	switch (attribute){
		case(ALL) : {
			value = all ;
			break ;
		}
        	case(AUTODG) : {
			value = autodg ;
			break ;
		}
		case(GET) : {
			value = get  ;
			break ;
		}
		case(SET) : {
			value = set  ;
			break ;
		}
		case(UNSET) : {
			value = unset  ;
			break ;
		}
		case(HELP) : {
			value = help  ;
			break ;
		}
                case(LISTONLY) : {
			value = listonly  ;
			break ;
		}
		case(VERSION) : {
			value = version ;
			break ;
		}
		case(NODISPLAY) : {
			value = nodisplay ;
			break ;
		}
		case(NOMSG) : {
			value = nomsg ;
			break ;
		}
		case(NOGUI) : {
			value = nogui ;
			break ;
		}
                case(NOINT) : {
			value = noint ;
			break ;
		}
		case(NOLISTEN) : {
			value = nolisten ;
			break ;
		}
                case(DBG) : {
			value = debug ;
			break ;
		}
                case(REGISTERONLY) : {
			value = registeronly;
			break ;
		}
		case(TRANSFER) : {
			value = transfer;
			break ;
		}
                case(RANK) : {
			value = rank;
			break ;
		}
		default : {
			// returns false
			break ;
		}
	};
	return value ;
};

/*
*	gets the value of the option list of strings-attribute
*/
const vector<string> Options::getListAttribute (const Options::OptsAttributes &attribute){
	vector<string> *vect = NULL ;
	switch (attribute){
		case(USERTAG) : {
			vect = &usertags ;
			break;
		}
		case(FILENAME) : {
			vect = &filenames ;
			break;
		}
		default : {
			// returns an NULL  vector
			break ;
		}
	};
	return (*vect);
};



/*
* Gets the verbosity level
*/
const int Options::getVerbosityLevel ( ){
	LogLevel level = WMSLOG_UNDEF;
        if (verbosityLevel != WMSLOG_UNDEF) {
		level = verbosityLevel ;
        } else {
                if (nomsg && debug){
                	ostringstream info ;
                        info << "the following options cannot be specified together:\n" ;
                        info << this->getAttributeUsage(Options::DBG) << "\n";
                        info << this->getAttributeUsage(Options::NOMSG) ;
                        throw WmsClientException(__FILE__,__LINE__,
                                        "getLogLevel",DEFAULT_ERR_CODE,
                                        "Input Option Error", info.str());
                }
		if (nomsg){
                	// nomsg=1
			level = WMSLOG_ERROR;
                } else if (debug){
                	// info messages on the stdout as well (debug=1)
			level = WMSLOG_DEBUG;
                } else {
                	// (default)info, warning and error messages on the stdout (debug,nomsg=0)
                	level = WMSLOG_INFO;
                }
                verbosityLevel = level;
        }
	return level;
}

/*
*	gets the short help usage message for an option attribute
*/
const string Options::getAttributeUsage (const Options::OptsAttributes &attribute){
	string msg = "";
	switch (attribute){
		case(ALL) : {
			msg = USG_ALL ;
			break ;
		}
		case(COLLECTION) : {
			msg = USG_COLLECTION ;
			break ;
		}
		case(DAG) : {
			msg = USG_DAG ;
			break ;
		}
		case(DEFJDL) : {
			msg = USG_DEFJDL ;
			break ;
		}
		case(DIR) : {
			msg = USG_DIR ;
			break ;
		}
		case(LOGFILE) : {
			msg = USG_LOGFILE ;
			break ;
		}
		case(CHKPT) : {
			msg = USG_CHKPT ;
			break ;
		}
                case(AUTODG) : {
			msg = USG_AUTODG ;
			break ;
		}
  		case(DBG) : {
			msg = USG_DEBUG ;
			break ;
		}
                case(DELEGATION) : {
			msg = USG_DELEGATION ;
			break ;
		}
                case(ENDPOINT) : {
			msg = USG_ENDPOINT ;
			break ;
		}
                case(LISTONLY) : {
			msg = USG_LISTONLY ;
			break ;
		}
                case(REGISTERONLY) : {
			msg = USG_REGISTERONLY ;
			break ;
		}
		 case(TRANSFER) : {
			msg = USG_TRANSFER ;
			break ;
		}
		case(PROTO) : {
			msg = USG_PROTO;
			break ;
		}
		case(START) : {
			msg = USG_START ;
			break ;
		}
		case(LRMS) : {
			msg = USG_LRMS ;
			break ;
		}
		case(TO) : {
			msg = USG_TO ;
			break ;
		}
		case(OUTPUT) : {
			msg = USG_OUTPUT ;
			break ;
		}
		case(INPUT) : {
			msg = USG_INPUT ;
			break ;
		}
		case(CONFIG) : {
			msg = USG_CONFIG ;
			break ;
		}
		case(RESOURCE) : {
			msg = USG_RESOURCE ;
			break ;
		}
		case(NODESRES) : {
			msg = USG_NODESRES ;
			break ;
		}
		case(HELP) : {
			msg = USG_HELP  ;
			break ;
		}
		case(VERSION) : {
			msg = USG_VERSION ;
			break ;
		}
		case(NOMSG) : {
			msg = USG_NOMSG ;
			break ;
		}
		case(NOINT) : {
			msg = USG_NOINT ;
			break ;
		}
		case(NOGUI) : {
			msg = USG_NOGUI ;
			break ;
		}
		case(NOLISTEN) : {
			msg = USG_NOLISTEN ;
			break ;
		}
		case(USERTAG) : {
			msg = USG_USERTAG ;
			break ;
		}
		case(PORT) : {
			msg = USG_PORT ;
			break ;
		}
		case(VALID) : {
			msg = USG_VALID ;
			break ;
		}
		case(VERBOSE) : {
			msg = USG_VERBOSE ;
			break ;
		}
                case(VO) : {
			msg = USG_VO ;
			break ;
		}
		case(GET) : {
			msg = USG_GET ;
			break ;
		}
		case(SET) : {
			msg = USG_SET ;
			break ;
		}
		case(UNSET) : {
			msg = USG_UNSET ;
			break ;
		}
		case(NODISPLAY) : {
			msg = USG_NODISPLAY ;
			break ;
		}
		case(FILENAME) : {
			msg = USG_FILENAME ;
			break ;
		}
		case(INPUTFILE) : {
			msg = USG_INPUTFILE ;
			break ;
		}
		default : {
			// returns an empty string
			break ;
		}
	}
	return msg ;
};
/*
*	Gets the list of job identifiers
*	@return a vector with the list of jobid's
*/
const vector<string> Options::getJobIds () {
	return jobIds;
};
/*
*	Gets the  job identifier for method that need only one JobId
*	@return a vector with the list of jobid's
*/
const string Options::getJobId () {
	return singleId;
};
/*
*	gets the path to the JDL file
*	@return the filepath string
*/
string* Options::getPath2Jdl () {
	return jdlFile ;
};


void Options::printUsage(const char* exename) {
        switch (cmdType){
                case (JOBSUBMIT ) :{
                        submit_usage(exename);
                        break;
                } ;
                case (JOBSTATUS ) :{
                        status_usage(exename);
                        break;
                } ;
                case (JOBLOGINFO ) :{
                        loginfo_usage(exename);
                        break;
                } ;
                case (JOBCANCEL ) :{
                        cancel_usage(exename);
                        break;
                } ;
                case (JOBOUTPUT ) :{
                        output_usage(exename);
                        break;
                } ;
                case (JOBMATCH) :{
                        lsmatch_usage(exename);
                        break;
                } ;
                case (JOBATTACH) :{
                        attach_usage(exename);
                        break;
                } ;
                case (JOBDELEGATION) :{
                        delegation_usage(exename);
                        break;
                } ;
                case (JOBPROXYINFO) :{
                        proxyinfo_usage(exename);
                        break;
                } ;
		case (JOBPERUSAL) :{
                        perusal_usage(exename);
                        break;
                } ;
                default :{
                        break;
                } ;
        }
        exit(-1);
 }
/**
* Returns the name of the command is being executed
*/
std::string Options::getApplicationName() {
	return this->applName;
 }
/**
*	Gets a string with the list of the input options
*/
std::string Options::getOptionsInfo(){
	if (warnsMsg.size()>0){
		return string( this->inCmd + "\n" + warnsMsg );
	} else{
		return this->inCmd;
	}
 }
/**
*	Reads the input options for submission
*/
void Options::readOptions(const int &argc, const char **argv){
        int next_opt = 0;
	string jobid = "";
	string invalid = "";
        ostringstream oss;
	char* last_arg = (char*)argv[(argc-1)];
	string arg = "";
        // the name of the the specific command (submit, cancel, etc.....)
	// that has called this method
        try{
        	//fs::path cp (Utils::normalizePath(argv[0]), fs::system_specific);  boost 1.29.1
		fs::path cp (Utils::normalizePath(argv[0]), fs::native);
 		applName = cp.leaf( );
        } catch (fs::filesystem_error &ex){
		applName = getDefaultApplicationName( );
        }
	if (argc>1){
		// option parsing
		while (next_opt != -1){
			// string with the option currently being parsed
			if (optind < argc){ arg = argv[optind]; }
			else { arg = ""; }
			// Returns the "val"-field of the struct "option"
			next_opt = getopt_long (argc, (char* const*)argv,
						shortOpts, longOpts, NULL);
			// error
			if (next_opt == '?') {
				throw WmsClientException(__FILE__,__LINE__,
					"readOptions", DEFAULT_ERR_CODE,
					"Arguments Error"  ,
					"Invalid Option" );
			} else if ( next_opt != -1 && arg.size() > 0 && checkOpts(arg) < 0  ){
				throw WmsClientException(__FILE__,__LINE__,
					"readOptions", DEFAULT_ERR_CODE,
					"Arguments Error"  ,
					string (arg) + string (": unrecognized option") );
			} else
			// Some operations have common short option letters
			// it determines which one has been specified
			// according to the specific wms command
			if (next_opt != -1 ){
				next_opt = checkCommonShortOpts(next_opt);
				setAttribute (next_opt, argv);
			}
		} ;

	}
	 if (!help && !version) {
		// Error (argc==1) : No arguments have been specified
		if (argc==1) {
			if ( (cmdType == JOBSUBMIT) ||
				cmdType == JOBMATCH  ){
					throw WmsClientException(__FILE__,__LINE__,
						"readOptions", DEFAULT_ERR_CODE,
						"Arguments Error"  ,
						"Last argument of the command must be a JDL file" );
			} else if ( cmdType == JOBATTACH ){
				throw WmsClientException(__FILE__,__LINE__,
							"readOptions", DEFAULT_ERR_CODE,
							"Arguments Error" ,
							"Last argument of the command must be a JobId");
			} else if ( cmdType == JOBSTATUS  ||
				cmdType == JOBLOGINFO ||
				cmdType == JOBCANCEL ||
				cmdType == JOBOUTPUT ) {
						throw WmsClientException(__FILE__,__LINE__,
								"readOptions", DEFAULT_ERR_CODE,
								"Arguments Error",
								"Last argument(s) of the command must be a JobId or a list of JobId's");
			}
		} else
		 // ========================================
		// JobSubmit - Match: checks the JDL file option
		// ========================================
		if (  cmdType == JOBSUBMIT   ||
			cmdType == JOBMATCH  ){
			if (!collection && !start && !dag){
				 if (optind < (argc-1) ){
					throw WmsClientException(__FILE__,__LINE__,
						"readOptions", DEFAULT_ERR_CODE,
						"Wrong Option",
						"Wrong Input Argument: " + string(argv[optind]) );
				} else if ( optind == (argc-1) ) {
					if (Utils::isFile( last_arg ) ) {
						jdlFile = new string(last_arg) ;
					} else {
						throw WmsClientException(__FILE__,__LINE__,
							"readOptions", DEFAULT_ERR_CODE,
							"Wrong Option",
							"The last argument is not a valid path to a JDL file: " + string(last_arg) );
					}
				} else {
					if (optind >= argc) {
						throw WmsClientException(__FILE__,__LINE__,
							"readOptions", DEFAULT_ERR_CODE,
							"Wrong Option",
							"The last argument must be a JDL file");

					} else {
						throw WmsClientException(__FILE__,__LINE__,
							"readOptions", DEFAULT_ERR_CODE,
							"Wrong Option",
							"Unknown input option: " + string(argv[optind]) );
					}
				}

  			 } else {
				if (optind < argc && collection) {
					ostringstream err ;
					err << "Unknown or incompatible option used with --" << LONG_COLLECTION << ":\n";
					err << string(argv[optind]) ;
					throw WmsClientException(__FILE__,__LINE__,
						"readOptions", DEFAULT_ERR_CODE,
						"Wrong Option",
						err.str() );
				} else if (optind < argc && dag) {
					ostringstream err ;
					err << "Unknown or incompatible option used with --" << LONG_DAG<< ":\n";
					err << string(argv[optind]) ;
					throw WmsClientException(__FILE__,__LINE__,
						"readOptions", DEFAULT_ERR_CODE,
						"Wrong Option",
						err.str() );
				} else if (optind < argc && start) {
					ostringstream err ;
					err << "Unknown or incompatible option used with --" << LONG_START << ":\n";
					err << string(argv[optind]) ;
					throw WmsClientException(__FILE__,__LINE__,
						"readOptions", DEFAULT_ERR_CODE,
						"Wrong Option",
						err.str() );
				}
				// --dag & --collection are incomptaible
				if (dag && collection) {
					ostringstream err ;
					err << "The following options cannot be specified together:\n" ;
					err << getAttributeUsage(Options::DAG) << "\n";
					err << getAttributeUsage(Options::COLLECTION) << "\n";
					throw WmsClientException(__FILE__,__LINE__,
						"readOptions", DEFAULT_ERR_CODE,
						"Incompatible Options",
						err.str() );
				}
			 }
		} else
			// =========================================================
			// JobProxyInfo : needs Jobid or --delegatioID option
			// ========================================================
			if ( cmdType == JOBPROXYINFO){
					if (optind == argc-1 ) {
						jobid = Utils::checkJobId (argv[optind]);
						jobIds.push_back(jobid);
						if(delegation){
							ostringstream err ;
							err << "The following option cannot used with the jobId: ";
							err << getAttributeUsage(Options::DELEGATION) << "\n";
							err << "Specify\n";
							err << "- JobId : for a delegated proxy used to submit the job identified by the JobId string ;\n";
							err << "- delegationId : for a proxy previously delegated and identified by the delegationId string.\n";
							throw WmsClientException(__FILE__,__LINE__,
								"readOptions", DEFAULT_ERR_CODE,
								"Incompatible Options"  ,
								err.str());
						}
					} else
					// all the options have been processed by getopt (JobId file is missing)
					if (optind == argc ) {
						if(delegation == NULL){
							throw WmsClientException(__FILE__,__LINE__,
								"readOptions", DEFAULT_ERR_CODE,
								"Wrong Option"  ,
								"Last argument of the command must be a JobId or specify a proxy delegationId string with the option:\n"
									+ getAttributeUsage(Options::DELEGATION) );
						}
					} else

					// all the options have been processed by getopt and we still have more than an argument
					 if (optind != (argc-1) ) {
						ostringstream err ;
						err << "Too many arguments:\n";
						for (int i = optind; i < argc; i++){
							err << argv[i++] ;
						}
						throw WmsClientException(__FILE__,__LINE__,
							"readOptions", DEFAULT_ERR_CODE,
							"Arguments Error",
							err.str() );
					} else {
						ostringstream err ;
						err << "Wrong arguments:\n";
						for (int i = optind; i < argc; i++){
							err << argv[i++] ;
						}
						throw WmsClientException(__FILE__,__LINE__,
							"readOptions", DEFAULT_ERR_CODE,
							"Arguments Error",
							err.str() );
					}
			} else

			// =========================================================
			// JobPerusal /JobAttach : need only one jobid as last argument
			// ========================================================
			 if ( cmdType == JOBPERUSAL ||
			 	cmdType == JOBATTACH ){
				if (input==NULL){
					// all the options have been processed by getopt (JobId file is missing)
					if (input == NULL && optind == argc){
						throw WmsClientException(__FILE__,__LINE__,
							"readOptions", DEFAULT_ERR_CODE,
							"Wrong Option: " + string(last_arg)  ,
							"Last argument of the command must be a JobId" );
					} else if (input == NULL && optind != argc-1) {
						for (int i = optind ; i < argc ; i++ ){
							invalid += string(argv[i]) + " " ;
							jobid = Utils::checkJobId (argv[i]);
							if ( jobid.size( ) >0 ) {
								jobIds.push_back(jobid);
							} else {
								invalid += string(argv[i]) + " " ;
							}
						}
						ostringstream err ;
						err << "Last argument of the command must be a JobId";
						if (invalid.size()>0){
							err << "\n(Unrecognised option(s): " + invalid + " )" ;
						}
						throw WmsClientException(__FILE__,__LINE__,
							"readOptions", DEFAULT_ERR_CODE,
							"Too Many Arguments",
							err.str());
					}
					// Reads the jobid
					jobid = Utils::checkJobId (argv[argc-1]);
					if ( jobid.size( ) >0 ) {
						jobIds.push_back(jobid);
					} else {
						invalid = string(jobid)  ;
					}
					// checks the read jobid
					if (invalid.size() > 0) {
						throw WmsClientException(__FILE__,__LINE__,
							"readOptions", DEFAULT_ERR_CODE,
							"Wrong Input Arguments" ,
							"invalid arguments: " + invalid );
					} else  if ( jobIds.empty()) {
						throw WmsClientException(__FILE__,__LINE__,
							"readOptions", DEFAULT_ERR_CODE,
							"Wrong Input Arguments"  ,
							"Last argument of the command must be a JobId" );
					} else {

						this->singleId = string(jobIds[0]);
					}
				} else
				if (input && optind != argc) {
					// Reads the wrong option !!
					jobid = Utils::checkJobId (argv[argc-1]);
					if ( jobid.size( ) >0 ) {
						throw WmsClientException(__FILE__,__LINE__,
							"readOptions", DEFAULT_ERR_CODE,
							"Too many arguments"  ,
							"The jobId mustn't be specified with the option:\n"
								+ getAttributeUsage(Options::INPUT));
					} else {
						throw WmsClientException(__FILE__,__LINE__,
							"readOptions", DEFAULT_ERR_CODE,
							"Wrong Input Arguments" ,
							"invalid arguments: " + invalid );
					}
				}
		} else
			// =========================================================
			// JobStatus/LogInfo/Cancel/Outpout : checks the last argument (JobId(s))
			// ========================================================

			if ( cmdType == JOBSTATUS  ||
                        	cmdType == JOBLOGINFO ||
                       		 cmdType == JOBCANCEL ||
				 cmdType == JOBPERUSAL ||
                        	cmdType == JOBOUTPUT ) {
				// all the options have been processed by getopt (JobId file is missing)
				if ( ! input && argc==optind){
					throw WmsClientException(__FILE__,__LINE__,
						"readOptions", DEFAULT_ERR_CODE,
						"Wrong Option: " + string(last_arg)  ,
						"Last argument of the command must be a JobId or a list of JobId's" );
				}
                                for (int i = optind ; i < argc ; i++ ){
     						jobIds.push_back(argv[i]);
                                }
                                if ( input && ! jobIds.empty( )){
                                        throw WmsClientException(__FILE__,__LINE__,
                                                        "readOptions", DEFAULT_ERR_CODE,
                                                        "Too many arguments",
                                                        "JobId(s) mustn't be specified with the option:\n" + getAttributeUsage(Options::INPUT));
                                } else  if ( jobIds.empty( ) && ! input){
                                        throw WmsClientException(__FILE__,__LINE__,
                                                        "readOptions", DEFAULT_ERR_CODE,
                                                        "Wrong Option",
                                                        "Last argument(s) of the command must be a JobId or a list of JobId's");
                                }
                } else{
			if (optind < argc ){
                                throw WmsClientException(__FILE__,__LINE__,
                                                "readOptions", DEFAULT_ERR_CODE,
                                                "Too many arguments specified",
                                                "Invalid option: " + string(argv[optind]) );
                        }
                }
	}
};
/************************************************
* private methods
*************************************************/
std::string Options::getDefaultApplicationName() {
	string name = "";
        switch (cmdType){
                case (JOBSUBMIT ) :{
                        name = "glite-wms-job-submit";
                        break;
                } ;
                case (JOBSTATUS ) :{
			name = "glite-wms-job-status";
                        break;
                } ;
                case (JOBLOGINFO ) :{
			name = "glite-wms-job-logging-info";
                        break;
                } ;
                case (JOBCANCEL ) :{
			name = "glite-wms-job-cancel";
                        break;
                } ;
                case (JOBOUTPUT ) :{
			name = "glite-wms-job-output";
                        break;
                } ;
                case ( JOBMATCH) :{
			name = "glite-wms-job-list-match";
                        break;
                } ;
                case ( JOBATTACH) :{
			name = "glite-wms-job-attach";
                        break;
                } ;
                case ( JOBDELEGATION) :{
			name = "glite-wms-job-delegate-proxy";
                        break;
                } ;
		case ( JOBPROXYINFO) :{
			name = "glite-wms-job-proxy-info";
                        break;
                } ;
		case ( JOBPERUSAL) :{
			name = "glite-wms-job-perusal";
                        break;
                } ;
                default :{
                        break;
                } ;
        }
        return name;
 }

/*
* Maps the common short option to the correspondent OptsAttributes enumeration code
*/
const int Options::checkCommonShortOpts (const int &opt ) {
	int r = opt ;
	switch (opt) {
		case (SHORT_E) : {
			if (cmdType==JOBSUBMIT ||
				cmdType==JOBMATCH ||
				cmdType==JOBDELEGATION ||
				cmdType==JOBPROXYINFO){
				r = Options::ENDPOINT;
			} else if (cmdType==JOBSTATUS) {
				r = Options::EXCLUDE;
			}
			break;
		}
		case (SHORT_V) : {
			if (cmdType==JOBSUBMIT) {
				r = Options::VALID;
			} else if (cmdType==JOBSTATUS || cmdType==JOBLOGINFO) {
				r = Options::VERBOSE;
			}
			break;
		}
	}
	return r;
}

const std::string  Options::checkArg(const std::string &opt, const std::string &arg, const Options::OptsAttributes &code, const std::string&shortopt){
	if (arg.compare(0, 1, "-") == 0){
		string err = "Invalid argument value for the option:";
		string usage = getAttributeUsage(code);
		if (usage.size()>0){
			err +=  "\n"  + usage;
		} else{
			err += "\n--" + opt ;
			if (shortopt.size()>0){ err +=  ", -" + string(shortopt) ;}
		}
		throw WmsClientException(__FILE__,__LINE__,
			"checkArg", DEFAULT_ERR_CODE,
			"Wrong Argument Option" , err);
	}
	return arg;

}
/**
*	sets the value of the option attribute
*/
void Options::setAttribute (const int &in_opt, const char **argv) {
	string* dupl = NULL;
        string px = "--";
        string ws = " ";
	string list = "";
	switch (in_opt){
		case ( Options::SHORT_AUTODG ) : {
			if (autodg){
				dupl = new string(LONG_AUTODG) ;
			} else {
				autodg = true;
				inCmd += px + LONG_AUTODG + ";" + ws ;
			}
			break ;
		};
		case ( Options::SHORT_OUTPUT ) : {
			if (output){
				dupl = new string(LONG_OUTPUT) ;
			} else {
				output = new string (checkArg(LONG_OUTPUT , optarg , Options::OUTPUT, string(1, Options::SHORT_OUTPUT)) );
                                inCmd += px + LONG_OUTPUT + ws + *output + ";" + ws ;
			}
			break ;
		};
		case ( Options::SHORT_INPUT) : {
			if (input){
				dupl = new string(LONG_INPUT) ;
			} else {
				input = new string (checkArg(LONG_INPUT ,optarg,Options::INPUT, string(1, Options::SHORT_INPUT))  );
                                inCmd += px + LONG_INPUT + ws + *input  + ";" + ws ;
			}
			break ;
		};
		case ( Options::SHORT_CONFIG) : {
			if (config){
				dupl = new string(LONG_CONFIG) ;
			} else {
				config = new string (checkArg(LONG_CONFIG ,optarg, Options::CONFIG, string(1, Options::SHORT_CONFIG)) );
				inCmd += px + LONG_CONFIG + ws + *config +  ";" + ws ;
			}
			break ;
		};
                case ( Options::SHORT_DELEGATION) : {
			if (delegation){
				dupl = new string(LONG_DELEGATION) ;
			} else {
				delegation = new string (checkArg( LONG_DELEGATION,optarg, Options::DELEGATION, string(1, Options::SHORT_DELEGATION)));
				inCmd += px + LONG_DELEGATION + ws + *delegation +  ";" + ws ;
			}
			break ;
		};
  		case ( Options::SHORT_RESOURCE) : {
			if (resource){
				dupl = new string(LONG_RESOURCE) ;
			} else {
				resource = new string (checkArg(LONG_RESOURCE ,optarg, Options::RESOURCE, string(1,  Options::SHORT_RESOURCE)));
                                inCmd += px + LONG_RESOURCE + ws + *resource  + ";" + ws ;
			}
			break ;
		};
		case ( Options::NODESRES) : {
			if (nodesres){
				dupl = new string(LONG_NODESRES) ;
			} else {
				nodesres = new string (checkArg( LONG_NODESRES, optarg,Options::NODESRES ) );
                                inCmd += px + LONG_NODESRES + ws + *nodesres  + ";" + ws ;
			}
			break ;
		};
		case ( Options::VALID ) : {
			if (valid){
				dupl = new string(LONG_VALID) ;
			} else {
				valid = new string(checkArg(LONG_VALID ,optarg,Options::VALID) );
                                inCmd += px + LONG_VALID + ws + *valid +  ";" + ws ;
			}
			break ;
		};
		case ( Options::VERBOSE ) : {
			if (verbosity){
				dupl = new string(LONG_VERBOSE) ;
			}else {
				verbosity = (unsigned int*) malloc (sizeof(int));
                                ostringstream v ;
                                v << *verbosity ;
                                inCmd += px + LONG_VERBOSE + ws + v.str()+ ";" + ws ;
				string arg =checkArg(LONG_VERBOSE,optarg, Options::VERBOSE);
				*verbosity = atoi (arg.c_str());
			}
			break ;
		};
		case ( Options::SHORT_STATUS ) : {
			if (status){
				dupl = new string(LONG_STATUS) ;
			} else {
				status = new string (checkArg( LONG_STATUS,optarg,Options::STATUS, string(1, Options::SHORT_STATUS ) ) );
                                inCmd += px + LONG_STATUS + ws + *status +";" + ws ;
			}
			break ;
		};
		case ( Options::EXCLUDE ) : {
			if (exclude) {
				dupl = new string(LONG_EXCLUDE) ;
			} else {
				exclude = new string (checkArg(LONG_EXCLUDE ,optarg, Options::EXCLUDE, string(1,Options::SHORT_E)) );
				inCmd += px + LONG_EXCLUDE + ws + *exclude +";" + ws ;
			}
			break;
		} ;
		case ( Options::ENDPOINT ) : {
			if (endpoint){
				dupl = new string(LONG_ENDPOINT) ;
			} else {
				endpoint = new string (checkArg( LONG_ENDPOINT,optarg, Options::ENDPOINT, string(1,Options::SHORT_E) ) );
				inCmd += px + LONG_ENDPOINT + ws + *endpoint  + ";" + ws ;
			}
			break ;
		};
		case ( Options::CHKPT ) : {
			if (chkpt){
				dupl = new string(LONG_CHKPT) ;
			} else {
				chkpt = new string (checkArg( LONG_CHKPT, optarg, Options::CHKPT) );
                                inCmd += px + LONG_CHKPT + ws + *chkpt +";" + ws;
			}
			break ;
		};
                case ( Options::COLLECTION ) : {
			if (collection){
				dupl = new string(LONG_COLLECTION) ;
			} else {
				collection = new string (checkArg(LONG_COLLECTION, optarg, Options::COLLECTION) );
                                inCmd += px + LONG_COLLECTION + ws + *collection  + ";" + ws ;
			}
			break ;
		};
		case ( Options::DAG ) : {
			if (dag){
				dupl = new string(LONG_DAG) ;
			} else {
				dag = new string (checkArg(LONG_DAG, optarg, Options::DAG) );
                                inCmd += px + LONG_DAG + ws + *dag  + ";" + ws ;
			}
			break ;
		};
		case ( Options::DEFJDL ) : {
			if (def_jdl){
				dupl = new string(LONG_DEFJDL) ;
			} else {
				def_jdl = new string (checkArg(LONG_DEFJDL, optarg, Options::DEFJDL) );
                                inCmd += px + LONG_DEFJDL + ws + *def_jdl + ";" + ws ;
			}
			break ;
		};
		case ( Options::DIR ) : {
			if (dir){
				dupl = new string(LONG_DIR ) ;
			} else {
				dir = new string (checkArg(LONG_DIR, optarg , Options::DIR) );
                                inCmd += px + LONG_DIR + ws + *dir + ";" + ws  ;
			}
			break ;
		};

		case ( Options::FROM ) : {
			if (from){
				dupl = new string(LONG_FROM) ;
			} else {
				from = new string (checkArg(LONG_FROM, optarg, Options::FROM) );
                                inCmd += px + LONG_FROM + ws + *from + ";" + ws  ;
			}
			break ;
		};
		case ( Options::PROTO) : {
			if (from){
				dupl = new string(LONG_PROTO) ;
			} else {
				fileprotocol = new string (checkArg(LONG_PROTO ,optarg, Options::PROTO) );
				inCmd += px + LONG_PROTO + ";" + ws ;
			}
			break ;
		};
		case ( Options::START ) : {
			if (start){
				dupl = new string(LONG_START) ;
			} else {
				start = new string(checkArg(LONG_START ,optarg, Options::START) );
				inCmd += px + LONG_START + ws + *start + ";" + ws ;
			}
			break ;
		};
		case ( Options::HELP ) : {
			help = true ;
                        inCmd += px + LONG_HELP + ";" + ws ;
			break;
		};
                case ( Options::ALL ) : {
			if (all){
				dupl = new string(LONG_ALL) ;
			} else {
				all = true;
				inCmd += px + LONG_ALL + ";" + ws ;
			}
			break ;
		};
                case ( Options::LISTONLY ) : {
			if (listonly){
				dupl = new string(LONG_LISTONLY) ;
			} else {
				listonly = true;
				inCmd += px + LONG_LISTONLY + ";" + ws ;
			}
			break ;
		};
                case ( Options::REGISTERONLY ) : {
			if (registeronly){
				dupl = new string(LONG_REGISTERONLY) ;
			} else {
				registeronly = true;
				inCmd += px + LONG_REGISTERONLY + ";" + ws ;
			}
			break ;
		};
		case ( Options::TRANSFER ) : {
			if (transfer){
				dupl = new string(LONG_TRANSFER) ;
			} else {
				transfer = true;
				inCmd += px + LONG_TRANSFER + ";" + ws ;
			}
			break ;
		};
		case ( Options::LRMS ) : {
			if (lrms){
				dupl = new string(LONG_LRMS) ;
			} else {
				lrms = new string (checkArg(LONG_LRMS,optarg,Options::LRMS ) );
				inCmd += px + LONG_LRMS + ";" + ws ;
			}
			break ;
		};
		case ( Options::LOGFILE ) : {
			if (logfile){
				dupl = new string(LONG_LOGFILE) ;
			} else {
				logfile = new string (checkArg(LONG_LOGFILE ,optarg,Options::LOGFILE ) );
				inCmd += px + LONG_LOGFILE + ws + *logfile + ";" + ws ;
			}
			break ;
		};
                case ( Options::VO) : {
			if (vo){
				dupl = new string(LONG_VO) ;
			} else {
				vo = new string (checkArg(LONG_VO,optarg,Options::VO ) );
				inCmd += px + LONG_VO + ";" + ws ;
			}
			break ;
		};
                case ( Options::INPUTFILE) : {
			if (inputfile){
				dupl = new string(LONG_INPUTFILE) ;
			} else {
				inputfile = new string (checkArg(LONG_INPUTFILE,optarg,Options::INPUTFILE ) );
				inCmd += px + LONG_INPUTFILE+ ";" + ws ;
			}
			break ;
		};
		case ( Options::SHORT_PORT ) : {
			if (port){
				dupl = new string(LONG_PORT) ;
			}else {
				port= (unsigned int*) malloc (sizeof(int));
				string arg = checkArg(LONG_PORT ,optarg, Options::PORT, string(1,Options::SHORT_PORT)) ;
				*port = atoi (arg.c_str());

				inCmd += px + LONG_PORT  + ws + boost::lexical_cast<string>(*port)+ ";" + ws ;
			}
			break ;
		};
                case ( Options::TO ) : {
			if (to){
				dupl = new string(LONG_TO) ;
			} else {
				to = new string (checkArg(LONG_TO,optarg,Options::TO ) );
				inCmd += px + LONG_TO + ws + *to + ";" + ws ;
			}
			break ;
		};
                case ( Options::DBG ) : {
                	if (debug){
				dupl = new string(LONG_DEBUG) ;
    			} else {
				debug = true;
  				inCmd += px + LONG_DEBUG + ";" + ws ;
                      }
			break ;
		};
		case ( Options::RANK ) : {
                	if (rank){
				dupl = new string(LONG_RANK) ;
    			} else {
				rank = true;
  				inCmd += px + LONG_RANK  + ";" + ws ;
                      }
                        break ;
		};
                case ( Options::NOGUI ) : {
                	if (nogui){
				dupl = new string(LONG_NOGUI) ;
    			} else {
				nogui= true;
 				inCmd += px + LONG_NOGUI + ";" + ws ;
                       }
                        break ;
		};
                 case ( Options::NOINT ) : {
                	if (noint){
				dupl = new string(LONG_NOINT) ;
    			} else {
				noint= true;
  				inCmd += px + LONG_NOINT + ";" + ws ;
                      }
                        break ;
		};
                case ( Options::NOLISTEN ) : {
                	if (nolisten){
				dupl = new string(LONG_NOLISTEN) ;
    			} else {
				nolisten= true;
  				inCmd += px + LONG_NOLISTEN + ";" + ws ;
                      }
                        break ;
		};
                case ( Options::NOMSG ) : {
                	if (nomsg){
				dupl = new string(LONG_NOMSG) ;
    			} else {
				nomsg = true;
  				inCmd += px + LONG_NOMSG + ";" + ws ;
                      }
                        break ;
		};
                case ( Options::VERSION ) : {
                	if (version){
				dupl = new string(LONG_VERSION) ;
    			} else {
				version = true;
  				inCmd += px + LONG_VERSION + ";" + ws ;
                      }
                        break ;
		};
		case ( Options::NODISPLAY ) : {
                	if (nodisplay){
				dupl = new string(LONG_NODISPLAY) ;
    			} else {
				nodisplay = true;
  				inCmd += px + LONG_NODISPLAY + ";" + ws ;
                      }
                        break ;
		};
		case ( Options::GET ) : {
                	if (get){
				dupl = new string(LONG_GET) ;
    			} else {
				get = true;
  				inCmd += px + LONG_GET + ";" + ws ;
                     	 }
                        break ;
		};
		case ( Options::SET ) : {
                	if (set){
				dupl = new string(LONG_SET) ;
    			} else {
				set = true;
  				inCmd += px + LONG_SET + ";" + ws ;
                     	 }
                        break ;
		};
		case ( Options::UNSET ) : {
                	if (unset){
				dupl = new string(LONG_UNSET) ;
    			} else {
				unset = true;
  				inCmd += px + LONG_UNSET + ";" + ws ;
                     	 }
                        break ;
		};
		// it could be specified more than once
		case ( Options::SHORT_FILENAME ) : {
			string file = checkArg(LONG_FILENAME ,optarg, Options::FILENAME ) ;
			if (Utils::hasElement(filenames, file )) {
				warnsMsg += errMsg(WMS_WARNING,
					string(px + LONG_FILENAME + ws + file) + ": ignored",
					" file specified more than once", true);
			} else{
				filenames.push_back(file);
				inCmd += px + LONG_SET + ws + file + ";" + ws ;
			}
			break ;
		};
		// it could be specified more than once
		case ( Options::USERTAG ) : {
			string tag = checkArg(LONG_USERTAG  ,optarg, Options::USERTAG)  ;
			if (Utils::hasElement(usertags, tag )) {
				warnsMsg += errMsg(WMS_WARNING,
					string(px + LONG_USERTAG + ws + tag) + ": ignored",
					"tag specified more than once", true);
			} else {
				usertags.push_back(tag);
				inCmd += px + LONG_USERTAG + ws + tag + ";" + ws ;
			}
			break ;
		};
		default : {
			throw WmsClientException(__FILE__,__LINE__,"setAttribute",
				DEFAULT_ERR_CODE,
				"Input Option Error",
				"unknown option"  );
			break ;
		};
	};
	if (dupl) {
		throw WmsClientException(__FILE__,__LINE__,"setAttribute",
				DEFAULT_ERR_CODE,
				"Input Option Error",
				string("option already specified: " + *dupl) );
	}
};


} // glite
} // wms
} // client
} // utilities
