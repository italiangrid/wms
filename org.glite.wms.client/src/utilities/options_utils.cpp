

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
const string Options::TRANSFER_FILES_HTCP_PROTO = "https" ;
const string Options::TRANSFER_FILES_GUC_PROTO = "gsiftp" ;
const string Options::TRANSFER_FILES_DEF_PROTO = Options::TRANSFER_FILES_GUC_PROTO ;
const string Options::JOBPATH_URI_PROTO ="https" ;
const char* Options::TRANSFER_FILES_PROTOCOLS[ ] = {
	Options::TRANSFER_FILES_GUC_PROTO.c_str(),
	Options::TRANSFER_FILES_HTCP_PROTO.c_str()
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
// Max file size for HTCP
const long Options::MAX_HTCP_SIZE = Options::MAX_DEFAULT_FILE_SIZE;

/*
* Verbosity level
*/
const unsigned int Options::DEFAULT_VERBOSITY = 1;
const unsigned int Options::MAX_VERBOSITY = 3;
/*
*	LONG OPTION STRINGS
*/
const char* Options::LONG_ALL 			= "all";
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
const char* Options::LONG_NOLISTEN	= "nolisten";
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
const char* Options::LONG_PROXY		= "proxy";
const char Options::SHORT_P		 	= 'p';
// delegation
const char* Options::LONG_DELEGATION	= "delegationid";
const char Options::SHORT_DELEGATION 	= 'd';
//JDL original
const char* Options::LONG_JDL			= "jdl";
const char* Options::LONG_JDLORIG		= "jdl-original";
const char Options::SHORT_JDLORIG		= 'j';
// no purger
const char* Options::LONG_NOPURG		= "nopurge";
const char Options::SHORT_NOPURG	= 'n';

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
        {	Options::LONG_DEFJDL,    		required_argument,		0,		Options::DEFJDL},
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
	{	Options::LONG_HELP,		no_argument,			0,	Options::HELP	},
	{ 	Options::LONG_OUTPUT,       	required_argument,		0,	Options::SHORT_OUTPUT},
	{ 	Options::LONG_INPUT,        	required_argument,		0,	Options::SHORT_INPUT},
	{	Options::LONG_LISTONLY,	no_argument,			0,	Options::LISTONLY},
	{	Options::LONG_PROTO	,	required_argument,		0,	Options::PROTO},
	{ 	Options::LONG_DIR, 	        	required_argument,		0,	Options::DIR},
	{	Options::LONG_CONFIG,    	required_argument,		0,	Options::SHORT_CONFIG},
        {	Options::LONG_VO,           	required_argument,		0,	Options::VO},
	{	Options::LONG_NOINT,		no_argument,			0,	Options::NOINT	},
	{ 	Options::LONG_DEBUG,      	no_argument,			0,	Options::DBG},
	{	Options::LONG_LOGFILE,    	required_argument,		0,	Options::LOGFILE},
	{	Options::LONG_NOPURG,    	no_argument,			0,	Options::SHORT_NOPURG},
	{0, 0, 0, 0}
};

/*
*	Long options for  job-attach
*/
const struct option Options::attachLongOpts[] = {
	{	Options::LONG_VERSION,		no_argument,			0,		Options::VERSION	},
	{	Options::LONG_HELP,			no_argument,			0,		Options::HELP	},
	{	Options::LONG_PORT,              	required_argument,		0,		Options::SHORT_P},
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
	{	Options::LONG_OUTPUT,            required_argument,		0,	Options::SHORT_OUTPUT},
	{	Options::LONG_NOINT,		no_argument,			0,	Options::NOINT	},
	{	Options::LONG_ALL,			no_argument,			0,		Options::ALL	},
	{	Options::LONG_HELP,			no_argument,			0,		Options::HELP	},
	{0, 0, 0, 0}
};
/*
*	Long options for proxy-info
*/
const struct option Options::jobInfoLongOpts[] = {
	{	Options::LONG_VERSION,		no_argument,			0,		Options::VERSION},
	{	Options::LONG_LOGFILE,		required_argument,		0,		Options::LOGFILE},
	{	Options::LONG_DEBUG,             	no_argument,			0,		Options::DBG},
	{	Options::LONG_PROXY,			no_argument,		0,		Options::SHORT_P},
	{	Options::LONG_DELEGATION,  	required_argument,		0,		Options::SHORT_DELEGATION},
	{	Options::LONG_JDLORIG,	  		no_argument,		0,		Options::SHORT_JDLORIG},
	{	Options::LONG_JDL	,	  		no_argument,		0,		Options::JDL},
	{	Options::LONG_ENDPOINT,        	required_argument,		0,		Options::SHORT_E},
	{	Options::LONG_CONFIG,    		required_argument,		0,		Options::SHORT_CONFIG},
	{	Options::LONG_VO,           		required_argument,		0,		Options::VO},
	{ 	Options::LONG_INPUT,        	required_argument,		0,	Options::SHORT_INPUT},
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

const string Options::USG_COLLECTION = "--" + string(LONG_COLLECTION)	 + "\t<dir_path>" ;

const string Options::USG_CONFIG = "--" + string(LONG_CONFIG ) +  ", -" + SHORT_CONFIG  + "\t<file_path>"	;

const string Options::USG_DAG = "--" + string(LONG_DAG)	 + "\t<dir_path>" ;

const string Options::USG_DEBUG  = "--" + string(LONG_DEBUG );

const string Options::USG_DEFJDL = "--" + string(LONG_DEFJDL)	 + "\t\t<file_path>" ;

const string Options::USG_DELEGATION  = "--" + string(LONG_DELEGATION )+ ", -" + SHORT_DELEGATION + " <id_string>";

const string Options::USG_DIR  = "--" + string(LONG_DIR )+ "\t\t<directory_path>"	;

const string Options::USG_ENDPOINT  = "--" + string(LONG_ENDPOINT )+ ", -" + SHORT_E + "\t<service_URL>";

const string Options::USG_EXCLUDE  = "--" + string(LONG_EXCLUDE )+ ", -" + SHORT_E + "\t<status_value>";

const string Options::USG_FILENAME = "--" + string(LONG_FILENAME) + ", -" + SHORT_FILENAME +  "\t<filename>";

const string Options::USG_FROM  = "--" + string(LONG_FROM )+ "\t\t[MM:DD:]hh:mm[:[CC]YY]";

const string Options::USG_GET  = "--" + string(LONG_GET ) ;

const string Options::USG_HELP = "--" + string(LONG_HELP) ;

const string Options::USG_JDL = "--" + string(LONG_JDL) ;

const string Options::USG_JDLORIG = "--" + string(LONG_JDLORIG)+ ", -" + SHORT_JDLORIG ;

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

const string Options::USG_NOPURG	 = "--" + string(LONG_NOPURG) + ", -" + SHORT_NOPURG;

const string Options::USG_OUTPUT = "--" + string(LONG_OUTPUT) + ", -" + SHORT_OUTPUT + "\t<file_path>";

const string Options::USG_PORT  = "--" + string(LONG_PORT )+ ", -" + SHORT_P + "\t<port_num>";

const string Options::USG_PROTO  = "--" + string(LONG_PROTO ) + "\t\t<protocol>";

const string Options::USG_PROXY = "--" + string(LONG_PROXY) + ", -" + SHORT_P ;

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
	cerr << "\t" << USG_DEFJDL << "\n";
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
	cerr << "\t" << USG_OUTPUT << "\n";
	cerr << "\t" << USG_DIR << "\n";
	cerr << "\t" << USG_PROTO << "\n";
	cerr << "\t" << USG_CONFIG << "\n";
        cerr << "\t" << USG_VO << "\n";
        cerr << "\t" << USG_LISTONLY << "\n";
	cerr << "\t" << USG_NOINT << "\n";
	cerr << "\t" << USG_NOPURG << "\n";
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
	cerr << "\t" << USG_ALL << "\n";
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
void Options::jobinfo_usage(const char* &exename, const bool &long_usg){
	cerr << "\n" << Options::getVersionMessage( ) << "\n" ;
	cerr << "Usage: " << exename <<   " [options] <operation options> <job Id>\n\n";
	cerr << "operation options (mandatory):\n";
	cerr << "\t" << USG_JDL << "\n";
	cerr << "\t" << USG_JDLORIG << "\n";
	cerr << "\t" << USG_PROXY << "\n";
	cerr << "\t" << USG_DELEGATION << " (*)\n";
        cerr << "options:\n" ;
	cerr << "\t" << USG_HELP << "\n";
        cerr << "\t" << USG_ENDPOINT << "\n";
	cerr << "\t" << USG_CONFIG << "\n";
        cerr << "\t" << USG_VO << "\n";
        cerr << "\t" << USG_INPUT << " (*)\n";
	cerr << "\t" << USG_OUTPUT << "\n";
	cerr << "\t" << USG_NOINT << "\n";
	cerr << "\t" << USG_DEBUG << "\n";
	cerr << "\t" << USG_LOGFILE << "\n\n";
	cerr << "\t" << "(*) argument <job Id> is not required\n";
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
	cerr << "\t" << USG_INPUT << "\n";
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
	m_jdlFile = "" ;
	// init of the string attributes
        m_collection = "";
	m_config = "";
	m_dag = "";
        m_delegation = "";
	m_def_jdl = "";
	m_dir = "";
        m_endpoint = "";
	m_exclude = "";
	m_from = "";
	m_input = "";
	m_lrms = "";
	m_logfile = "";
	m_nodesres = "";
	m_output = "";
	m_resource = "";
	m_start = "";
	m_status = "";
	m_to = "";
	m_valid = "";
        m_vo = "";
	m_inputfile = "";
	// init of the boolean attributes
	all  = false ;
        autodg = false;
	debug  = false ;
	get = false;
	help = false  ;
	jdl = false ;
	jdlorig = false ;
        listonly = false;
	nodisplay = false ;
	nogui  = false ;
	noint  = false ;
	nolisten = false  ;
	nomsg = false  ;
	nopurg = false ;
	proxy = false ;
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
	m_fileprotocol = "";
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
				Options::SHORT_E,  	short_required_arg, // endpoint
				Options::SHORT_OUTPUT, 	short_required_arg,
				Options::SHORT_INPUT,	short_required_arg,
				Options::SHORT_CONFIG,	short_required_arg,
				Options::SHORT_V,	short_required_arg//verbosity
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
				"%c%c%c%c%c%c%c%c" ,
				Options::SHORT_INPUT, short_required_arg,
				Options::SHORT_OUTPUT, short_required_arg,
				Options::SHORT_NOPURG, short_no_arg,
				Options::SHORT_CONFIG, short_required_arg);
			// long options
			longOpts = outputLongOpts ;
			numOpts = (sizeof(outputLongOpts)/sizeof(option)) -1;
			break;
		} ;
		case(JOBATTACH) :{
			// short options
			asprintf (&shortOpts,
				"%c%c%c%c%c%c" ,
				Options::SHORT_P, short_required_arg,
				Options::SHORT_INPUT, short_required_arg,
				Options::SHORT_CONFIG, short_required_arg);
			// long options
			longOpts = attachLongOpts ;
			numOpts = (sizeof(attachLongOpts)/sizeof(option)) -1;
			break;
		} ;
                case (JOBDELEGATION) :{
			// short options
			asprintf (&shortOpts,
				"%c%c%c%c%c%c%c%c%c%c",
				Options::SHORT_E,  		short_required_arg, // endpoint
				Options::SHORT_AUTODG, 		short_no_arg,
				Options::SHORT_DELEGATION, 	short_required_arg,
				Options::SHORT_OUTPUT, 		short_required_arg,
				Options::SHORT_CONFIG,		short_required_arg);

			// long options
			longOpts = delegationLongOpts ;
			numOpts = (sizeof(delegationLongOpts)/sizeof(option)) -1;
			break ;
		} ;
		case (JOBINFO) :{
			// short options
			asprintf (&shortOpts,
				"%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				Options::SHORT_E, short_required_arg, // endpoint
				Options::SHORT_P, short_no_arg, // proxy
				Options::SHORT_JDLORIG, short_no_arg,
				Options::SHORT_DELEGATION, short_required_arg,
				Options::SHORT_INPUT, short_required_arg,
				Options::SHORT_OUTPUT, short_required_arg,
				Options::SHORT_CONFIG, short_required_arg);

			// long options
			longOpts = jobInfoLongOpts ;
			numOpts = (sizeof(jobInfoLongOpts)/sizeof(option)) -1;
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
	if (port ) { free(port);}
	if (verbosity  ) { free (verbosity );}
	if (shortOpts) { free(shortOpts); }
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
		if (protocol.compare(TRANSFER_FILES_HTCP_PROTO)==0) {
			proto = MAX_HTCP_SIZE ;
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
string Options::getStringAttribute (const OptsAttributes &attribute){
	string value = "";
	switch (attribute){
        	case(COLLECTION) : {
			value = m_collection;
			break ;
		}
		case(DIR) : {
			value = m_dir;
			break ;
		}
		case(LOGFILE) : {
			value = m_logfile;
			break ;
		}
		case(DAG) : {
			value = m_dag;
			break ;
		}
		case(DEFJDL) : {
			value = m_def_jdl;
			break ;
		}
                case(DELEGATION) : {
			value = m_delegation;
			break ;
		}
		case(ENDPOINT) : {
			value = m_endpoint;
			break ;
		}
		case(LRMS) : {
			value = m_lrms;
			break ;
		}
		case(VALID) : {
			value = m_valid;
			break ;
		}
		case(TO) : {
			value = m_to;
			break ;
		}
		case(OUTPUT) : {
			value = m_output;
			break ;
		}
		case(PROTO) : {
			value = m_fileprotocol;
			break ;
		}
		case(INPUT) : {
			value = m_input;
			break ;
		}
		case(CONFIG) : {
			value = m_config;
			break ;
		}
                case(VO) : {
			value = m_vo;
			break ;
		}
		case(RESOURCE) : {
			value = m_resource;
			break ;
		}
		case(NODESRES) : {
			value = m_nodesres;
			break ;
		}
		case(INPUTFILE) : {
			value = m_inputfile;
			break ;
		}
		case(START) : {
			value = m_start;
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
		case(NOPURG) : {
			value = nopurg ;
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
		case(PROXY) : {
			if (proxy) {
				value = proxy;
			}
			break ;
		}
                case(JDLORIG) : {
			if (jdlorig) {
				value = jdlorig;
			}
			break ;
		}
                case(JDL) : {
			if (jdl) {
				value = jdl;
			}
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
		case(JDL) : {
			msg = USG_JDL ;
			break ;
		}
		case(JDLORIG) : {
			msg = USG_JDLORIG ;
			break ;
		}
		case(PROXY) : {
			msg = USG_PROXY;
			break ;
		}
		case(NOPURG) : {
			msg = USG_NOPURG ;
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
string Options::getPath2Jdl () {
	return m_jdlFile ;
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
                case (JOBINFO) :{
                        jobinfo_usage(exename);
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
	if (argc == 1) {
		printUsage(applName.c_str());
	} else if (argc>1) {
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
				printUsage(applName.c_str());
				throw WmsClientException(__FILE__,__LINE__,
					"readOptions", DEFAULT_ERR_CODE,
					"Arguments Error"  ,
					"Invalid Option");
			} else if ( next_opt != -1 && arg.size() > 0 && checkOpts(arg) < 0  ){
				printUsage(applName.c_str());
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
			if (m_collection.empty() && m_start.empty() && m_dag.empty()){
				 if (optind < (argc-1) ){
					throw WmsClientException(__FILE__,__LINE__,
						"readOptions", DEFAULT_ERR_CODE,
						"Wrong Option",
						"Wrong Input Argument: " + string(argv[optind]) );
				} else if ( optind == (argc-1) ) {
					if (Utils::isFile( last_arg ) ) {
						m_jdlFile = last_arg;
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
				if (optind < argc && !m_collection.empty()) {
					ostringstream err ;
					err << "Unknown or incompatible option used with --" << LONG_COLLECTION << ":\n";
					err << string(argv[optind]) ;
					throw WmsClientException(__FILE__,__LINE__,
						"readOptions", DEFAULT_ERR_CODE,
						"Wrong Option",
						err.str() );
				} else if (optind < argc && !m_dag.empty()) {
					ostringstream err ;
					err << "Unknown or incompatible option used with --" << LONG_DAG<< ":\n";
					err << string(argv[optind]) ;
					throw WmsClientException(__FILE__,__LINE__,
						"readOptions", DEFAULT_ERR_CODE,
						"Wrong Option",
						err.str() );
				} else if (optind < argc && !m_start.empty()) {
					ostringstream err ;
					err << "Unknown or incompatible option used with --" << LONG_START << ":\n";
					err << string(argv[optind]) ;
					throw WmsClientException(__FILE__,__LINE__,
						"readOptions", DEFAULT_ERR_CODE,
						"Wrong Option",
						err.str() );
				}
				// --dag & --collection are incomptaible
				if (!m_dag.empty() && !m_collection.empty()) {
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
                        // ===========================================================
                        // JobProxyInfo : needs Jobid (from input file or command line), or --delegation-id
                        // ===========================================================
                        if ( cmdType == JOBINFO) {
			     if (m_input.empty() && m_delegation.empty()) {
				jobid = Utils::checkJobId (argv[argc-1]);
				if ( jobid.size( ) >0 )  {
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
			} else if ((!m_delegation.empty() || !m_input.empty()) && optind != argc) {
				throw WmsClientException(__FILE__,__LINE__,
					"readOptions", DEFAULT_ERR_CODE,
					"Too Many Arguments",
					"The jobId must not be specified with the option:\n"
					 +getAttributeUsage(Options::DELEGATION)+"\n"
					 +getAttributeUsage(Options::INPUT));
			}
               } else
			// =========================================================
			// JobPerusal /JobAttach : need only one jobid as last argument
			// ========================================================
			 if ( cmdType == JOBPERUSAL ||
			 	cmdType == JOBATTACH ) {
				if (m_input.empty()){
					// all the options have been processed by getopt (JobId file is missing)
					if (m_input.empty() && optind == argc){
						throw WmsClientException(__FILE__,__LINE__,
							"readOptions", DEFAULT_ERR_CODE,
							"Wrong Option: " + string(last_arg)  ,
							"Last argument of the command must be a JobId" );
					} else if (m_input.empty() && optind != argc-1) {
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
				if (!m_input.empty() && optind != argc) {
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
				if (m_input.empty() && argc==optind){
					throw WmsClientException(__FILE__,__LINE__,
						"readOptions", DEFAULT_ERR_CODE,
						"Wrong Option: " + string(last_arg)  ,
						"Last argument of the command must be a JobId or a list of JobId's" );
				}
                                for (int i = optind ; i < argc ; i++ ){
     						jobIds.push_back(argv[i]);
                                }
                                if (!m_input.empty() && ! jobIds.empty( )){
                                        throw WmsClientException(__FILE__,__LINE__,
                                                        "readOptions", DEFAULT_ERR_CODE,
                                                        "Too many arguments",
                                                        "JobId(s) mustn't be specified with the option:\n" + getAttributeUsage(Options::INPUT));
                                } else  if ( jobIds.empty( ) && m_input.empty()){
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
		case ( JOBINFO) :{
			name = "glite-wms-job-info";
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
				cmdType==JOBINFO){
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
		case (SHORT_P) : {
			if (cmdType==JOBSUBMIT || cmdType==JOBATTACH) {
				r = Options::PORT;
			} else if (cmdType==JOBINFO) {
				r = Options::PROXY;
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
	string dupl = "";
        string px = "--";
        string ws = " ";
	string list = "";
	switch (in_opt){
		case ( Options::SHORT_AUTODG ) : {
			if (autodg){
				dupl = LONG_AUTODG;
			} else {
				autodg = true;
				inCmd += px + LONG_AUTODG + ";" + ws ;
			}
			break ;
		};
		case ( Options::SHORT_OUTPUT ) : {
			if (m_output.empty()){
				m_output = checkArg(LONG_OUTPUT , optarg , Options::OUTPUT, string(1, Options::SHORT_OUTPUT));
                                inCmd += px + LONG_OUTPUT + ws + m_output + ";" + ws ;
			} else {
				dupl = LONG_OUTPUT;
			}
			break ;
		};
		case ( Options::SHORT_INPUT) : {
			if (m_input.empty()){
				m_input = checkArg(LONG_INPUT ,optarg,Options::INPUT, string(1, Options::SHORT_INPUT));
                                inCmd += px + LONG_INPUT + ws + m_input  + ";" + ws ;
			} else {
				dupl = LONG_INPUT;
			}
			break ;
		};
		case ( Options::SHORT_CONFIG) : {
			if (m_config.empty()){
				m_config = checkArg(LONG_CONFIG ,optarg, Options::CONFIG, string(1, Options::SHORT_CONFIG));
				inCmd += px + LONG_CONFIG + ws + m_config +  ";" + ws ;
			} else {
				dupl = LONG_CONFIG;
			}
			break ;
		};
                case ( Options::SHORT_DELEGATION) : {
			if (m_delegation.empty()){
				m_delegation = checkArg( LONG_DELEGATION,optarg, Options::DELEGATION, string(1, Options::SHORT_DELEGATION));
				inCmd += px + LONG_DELEGATION + ws + m_delegation +  ";" + ws ;
			} else {
				dupl = LONG_DELEGATION;
			}
			break ;
		};
  		case ( Options::SHORT_RESOURCE) : {
			if (m_resource.empty()){
				m_resource = checkArg(LONG_RESOURCE ,optarg, Options::RESOURCE, string(1,  Options::SHORT_RESOURCE));
                                inCmd += px + LONG_RESOURCE + ws + m_resource  + ";" + ws ;
			} else {
				dupl = LONG_RESOURCE;
			}
			break ;
		};
		case ( Options::NODESRES) : {
			if (m_nodesres.empty()){
				m_nodesres = checkArg( LONG_NODESRES, optarg,Options::NODESRES );
                                inCmd += px + LONG_NODESRES + ws + m_nodesres  + ";" + ws ;
			} else {
				dupl = LONG_NODESRES;
			}
			break ;
		};
		case ( Options::VALID ) : {
			if (m_valid.empty()){
				m_valid = checkArg(LONG_VALID ,optarg,Options::VALID);
                                inCmd += px + LONG_VALID + ws + m_valid +  ";" + ws ;
			} else {
				dupl = LONG_VALID;
			}
			break ;
		};
		case ( Options::VERBOSE ) : {
			if (verbosity){
				dupl = LONG_VERBOSE;
			}else {
				verbosity = (unsigned int*) malloc (sizeof(int));
                                ostringstream v ;
                                v << *verbosity ;
                                inCmd += px + LONG_VERBOSE + ws + v.str()+ ";" + ws ;
				string arg = checkArg(LONG_VERBOSE,optarg, Options::VERBOSE);
				*verbosity = atoi (arg.c_str());
			}
			break ;
		};
		case ( Options::SHORT_STATUS ) : {
			if (m_status.empty()){
				m_status = checkArg( LONG_STATUS,optarg,Options::STATUS, string(1, Options::SHORT_STATUS ) );
                                inCmd += px + LONG_STATUS + ws + m_status +";" + ws ;
			} else {
				dupl = LONG_STATUS;
			}
			break ;
		};
		case ( Options::EXCLUDE ) : {
			if (m_exclude.empty()) {
				m_exclude = checkArg(LONG_EXCLUDE ,optarg, Options::EXCLUDE, string(1,Options::SHORT_E));
				inCmd += px + LONG_EXCLUDE + ws + m_exclude +";" + ws ;
			} else {
				dupl = LONG_EXCLUDE;
			}
			break;
		} ;
		case ( Options::ENDPOINT ) : {
			if (m_endpoint.empty()){
				m_endpoint = checkArg( LONG_ENDPOINT,optarg, Options::ENDPOINT, string(1,Options::SHORT_E) );
				inCmd += px + LONG_ENDPOINT + ws + m_endpoint  + ";" + ws ;
			} else {
				dupl = LONG_ENDPOINT;
			}
			break ;
		};
                case ( Options::COLLECTION ) : {
			if (m_collection.empty()){
				m_collection = checkArg(LONG_COLLECTION, optarg, Options::COLLECTION);
                                inCmd += px + LONG_COLLECTION + ws + m_collection  + ";" + ws ;
			} else {
				dupl = LONG_COLLECTION;
			}
			break ;
		};
		case ( Options::DAG ) : {
			if (m_dag.empty()){
				m_dag = checkArg(LONG_DAG, optarg, Options::DAG);
                                inCmd += px + LONG_DAG + ws + m_dag  + ";" + ws ;
			} else {
				dupl = LONG_DAG;
			}
			break ;
		};
		case ( Options::DEFJDL ) : {
			if (m_def_jdl.empty()){
				m_def_jdl = checkArg(LONG_DEFJDL, optarg, Options::DEFJDL);
                                inCmd += px + LONG_DEFJDL + ws + m_def_jdl + ";" + ws ;
			} else {
				dupl = LONG_DEFJDL;
			}
			break ;
		};
		case ( Options::DIR ) : {
			if (m_dir.empty()){
				m_dir = checkArg(LONG_DIR, optarg , Options::DIR);
                                inCmd += px + LONG_DIR + ws + m_dir + ";" + ws  ;
			} else {
				dupl = LONG_DIR;
			}
			break ;
		};

		case ( Options::FROM ) : {
			if (m_from.empty()){
				m_from = checkArg(LONG_FROM, optarg, Options::FROM);
                                inCmd += px + LONG_FROM + ws + m_from + ";" + ws  ;
			} else {
				dupl = LONG_FROM;
			}
			break ;
		};
		case ( Options::PROTO) : {
			if (m_fileprotocol.empty()){
				m_fileprotocol = checkArg(LONG_PROTO ,optarg, Options::PROTO);
				inCmd += px + LONG_PROTO + ";" + ws ;
			} else {
				dupl = LONG_PROTO;
			}
			break ;
		};
		case ( Options::START ) : {
			if (m_start.empty()){
				m_start = checkArg(LONG_START ,optarg, Options::START);
				inCmd += px + LONG_START + ws + m_start + ";" + ws ;
			} else {
				dupl = LONG_START;
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
				dupl = LONG_ALL;
			} else {
				all = true;
				inCmd += px + LONG_ALL + ";" + ws ;
			}
			break ;
		};
                case ( Options::LISTONLY ) : {
			if (listonly){
				dupl = LONG_LISTONLY;
			} else {
				listonly = true;
				inCmd += px + LONG_LISTONLY + ";" + ws ;
			}
			break ;
		};
                case ( Options::REGISTERONLY ) : {
			if (registeronly){
				dupl = LONG_REGISTERONLY;
			} else {
				registeronly = true;
				inCmd += px + LONG_REGISTERONLY + ";" + ws ;
			}
			break ;
		};
		case ( Options::TRANSFER ) : {
			if (transfer){
				dupl = LONG_TRANSFER;
			} else {
				transfer = true;
				inCmd += px + LONG_TRANSFER + ";" + ws ;
			}
			break ;
		};
		case ( Options::LRMS ) : {
			if (m_lrms.empty()){
				m_lrms = checkArg(LONG_LRMS,optarg,Options::LRMS );
				inCmd += px + LONG_LRMS + ";" + ws ;
			} else {
				dupl = LONG_LRMS;
			}
			break ;
		};
		case ( Options::LOGFILE ) : {
			if (m_logfile.empty()){
				m_logfile = checkArg(LONG_LOGFILE ,optarg,Options::LOGFILE );
				inCmd += px + LONG_LOGFILE + ws + m_logfile + ";" + ws ;
			} else {
				dupl = LONG_LOGFILE;
			}
			break ;
		};
                case ( Options::VO) : {
			if (m_vo.empty()){
				m_vo = checkArg(LONG_VO,optarg,Options::VO );
				inCmd += px + LONG_VO + ";" + ws ;
			} else {
				dupl = LONG_VO;
			}
			break ;
		};
                case ( Options::INPUTFILE) : {
			if (m_inputfile.empty()){
				m_inputfile = checkArg(LONG_INPUTFILE,optarg,Options::INPUTFILE );
				inCmd += px + LONG_INPUTFILE+ ";" + ws ;
			} else {
				dupl = LONG_INPUTFILE;
			}
			break ;
		};
		case ( Options::PORT ) : {
			if (port){
				dupl = LONG_PORT;
			}else {
				port= (unsigned int*) malloc (sizeof(int));
				string arg = checkArg(LONG_PORT ,optarg, Options::PORT, string(1,Options::SHORT_P)) ;
				*port = atoi (arg.c_str());

				inCmd += px + LONG_PORT  + ws + boost::lexical_cast<string>(*port)+ ";" + ws ;
			}
			break ;
		};
                case ( Options::TO ) : {
			if (m_to.empty()){
				m_to = checkArg(LONG_TO,optarg,Options::TO );
				inCmd += px + LONG_TO + ws + m_to + ";" + ws ;
			} else {
				dupl = LONG_TO;
			}
			break ;
		};
                case ( Options::DBG ) : {
                	if (debug){
				dupl = LONG_DEBUG;
    			} else {
				debug = true;
  				inCmd += px + LONG_DEBUG + ";" + ws ;
                      }
			break ;
		};
		case ( Options::RANK ) : {
                	if (rank){
				dupl = LONG_RANK;
    			} else {
				rank = true;
  				inCmd += px + LONG_RANK  + ";" + ws ;
                      }
                        break ;
		};
                case ( Options::NOGUI ) : {
                	if (nogui){
				dupl = LONG_NOGUI;
    			} else {
				nogui= true;
 				inCmd += px + LONG_NOGUI + ";" + ws ;
                       }
                        break ;
		};
                 case ( Options::NOINT ) : {
                	if (noint){
				dupl = LONG_NOINT;
    			} else {
				noint= true;
  				inCmd += px + LONG_NOINT + ";" + ws ;
                      }
                        break ;
		};
                case ( Options::NOLISTEN ) : {
                	if (nolisten){
				dupl = LONG_NOLISTEN;
    			} else {
				nolisten= true;
  				inCmd += px + LONG_NOLISTEN + ";" + ws ;
                      }
                        break ;
		};
                case ( Options::NOMSG ) : {
                	if (nomsg){
				dupl = LONG_NOMSG;
    			} else {
				nomsg = true;
  				inCmd += px + LONG_NOMSG + ";" + ws ;
                      }
                        break ;
		};
                case ( Options::SHORT_NOPURG ) : {
                	if (nopurg){
				dupl = LONG_NOPURG;
    			} else {
				nopurg = true;
  				inCmd += px + LONG_NOPURG + ws + ";" + ws ;
                      }
                        break ;
		};
                case ( Options::VERSION ) : {
                	if (version){
				dupl = LONG_VERSION;
    			} else {
				version = true;
  				inCmd += px + LONG_VERSION + ";" + ws ;
                      }
                        break ;
		};
		case ( Options::NODISPLAY ) : {
                	if (nodisplay){
				dupl = LONG_NODISPLAY;
    			} else {
				nodisplay = true;
  				inCmd += px + LONG_NODISPLAY + ";" + ws ;
                      }
                        break ;
		};
		case ( Options::GET ) : {
                	if (get){
				dupl = LONG_GET;
    			} else {
				get = true;
  				inCmd += px + LONG_GET + ";" + ws ;
                     	 }
                        break ;
		};
		case ( Options::SET ) : {
                	if (set){
				dupl = LONG_SET;
    			} else {
				set = true;
  				inCmd += px + LONG_SET + ";" + ws ;
                     	 }
                        break ;
		};
		case ( Options::UNSET ) : {
                	if (unset){
				dupl = LONG_UNSET;
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
		case ( Options::JDL ) : {
                	if (jdl){
				dupl = LONG_JDL;
    			} else {
				jdl = true ;
  				inCmd += px + LONG_JDL + ws +  ";" + ws ;
                     	 }
                        break ;
		};
		case ( Options::SHORT_JDLORIG ) : {
                	if (jdlorig){
				dupl = LONG_JDLORIG;
    			} else {
				jdlorig = true ;
  				inCmd += px + LONG_JDLORIG + ws +  ";" + ws ;
                     	 }
                        break ;
		};
		case ( Options::PROXY ) : {
                	if (unset){
				dupl = LONG_PROXY;
    			} else {
				proxy = true ;
  				inCmd += px + LONG_PROXY + ws + ";" + ws ;
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
	if (!dupl.empty()) {
		throw WmsClientException(__FILE__,__LINE__,"setAttribute",
				DEFAULT_ERR_CODE,
				"Input Option Error",
				string("option already specified: " + dupl) );
	}
};


} // glite
} // wms
} // client
} // utilities
