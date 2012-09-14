/**
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
*       Authors:        Alessandro Maraschini <alessandro.maraschini@datamat.it>
*                       Marco Sottilaro <marco.sottilaro@datamat.it>
*/

//      $Id$

#include "jobperusal.h"
#include <string>
#include <sys/stat.h> //mkdir
// streams
#include<sstream>
#include <sys/stat.h> //mkdir
// wmproxy-api
#include "glite/wms/wmproxyapi/wmproxy_api.h"
#include "glite/wms/wmproxyapi/wmproxy_api_utilities.h"
// JDL
#include "glite/jdl/Ad.h"
#include "glite/jdl/JDLAttributes.h"
// exceptions
#include "utilities/excman.h"
// wmp-client utilities
#include "utilities/utils.h"
#include "utilities/options_utils.h"
// wmp-client LB API
#include"lbapi.h"
//BOOST
#include "boost/lexical_cast.hpp" // types conversion


using namespace std ;
using namespace glite::wms::client::utilities ;
using namespace glite::wms::wmproxyapi;
using namespace glite::wms::wmproxyapiutils;

namespace glite {
namespace wms {
namespace client {
namespace services {

const int SUCCESS = 0;
const int FAILED = -1;
const int FORK_FAILURE = -1;
const int COREDUMP_FAILURE = -2;
const int TIMEOUT_FAILURE = -3;
const string DEFAULT_STORAGE_LOCATION = "/tmp";
const string DISPLAY_CMD = "more";
const int HTTP_OK = 200;
const int TRANSFER_OK = 0;

/*
* 	Default constructor
*/
JobPerusal::JobPerusal () : Job() {
	// init of the string  options
	m_inOpt = "";
	m_inFileOpt = "";
	m_outOpt = "";
	m_dirOpt = "";
	// boolean options
	getOpt = false;
	setOpt = false;
	unsetOpt = false;
	nodisplayOpt = false;
	nointOpt = false;
	allOpt = false;
	jobId = "";
}
/**
* Default Destructor
*/
JobPerusal::~JobPerusal ()  {
}
/**
* Reads the input arguments
*/
void JobPerusal::readOptions ( int argc,char **argv)  {
	ostringstream err ;
	string files = "";
	string dircfg = "";
	string logname = "";
	ostringstream warn;
	vector<string> jobids;
	int njobs = 0;
	// Reads the input options
 	Job::readOptions  (argc, argv, Options::JOBPERUSAL);
        // --get
	getOpt = wmcOpts->getBoolAttribute(Options::GET);
	// --set
	setOpt = wmcOpts->getBoolAttribute(Options::SET);
	// --unset
	unsetOpt = wmcOpts->getBoolAttribute(Options::UNSET);
	// --all
	allOpt = wmcOpts->getBoolAttribute(Options::ALL);
	if (!getOpt && !setOpt && !unsetOpt){
		err << "A mandatory option is missing:\n" ;
		err << wmcOpts->getAttributeUsage(Options::GET) << " | ";
		err << wmcOpts->getAttributeUsage(Options::SET) << " | ";
		err << wmcOpts->getAttributeUsage(Options::UNSET) << "\n";
	} else if ((getOpt && setOpt) ||
			(getOpt &&unsetOpt) ||
			(setOpt &&unsetOpt))	{
		err << "The following options cannot be specified together:\n" ;
		err << wmcOpts->getAttributeUsage(Options::GET) << " | ";
		err << wmcOpts->getAttributeUsage(Options::SET) << " | ";
		err << wmcOpts->getAttributeUsage(Options::UNSET) << "\n";
	} else if (setOpt && allOpt) {
		err << "The following options cannot be specified together:\n" ;
		err << wmcOpts->getAttributeUsage(Options::SET) << " | ";
		err << wmcOpts->getAttributeUsage(Options::ALL) << "\n";
	}
	if (err.str().size() > 0) {
		throw WmsClientException(__FILE__,__LINE__,
				"readOptions",DEFAULT_ERR_CODE,
				"Input Option Error", err.str());
	}
	// --noint
	nointOpt = wmcOpts->getBoolAttribute(Options::NOINT);
	/*
	* The jobid
	*/
	jobId = wmcOpts->getJobId( );
	// --input
	m_inOpt = wmcOpts->getStringAttribute(Options::INPUT);
	// JobId's
        if (!m_inOpt.empty()){
		// no Jobid with --input
		if (jobId.size() > 0) {
			throw WmsClientException(__FILE__,__LINE__,
				"JobPerusal::readOptions", DEFAULT_ERR_CODE,
				"Too many arguments"  ,
				"The jobId mustn't be specified with the option:\n"
				+ wmcOpts->getAttributeUsage(Options::INPUT));
		}
		// From input file
		m_inOpt = Utils::getAbsolutePath(m_inOpt);
		logInfo->print (WMS_INFO, "Reading the jobId from the input file:", m_inOpt);
		jobids = wmcUtils->getItemsFromFile(m_inOpt);
		jobids = wmcUtils->checkJobIds (jobids);
		njobs = jobids.size( ) ;
		if (njobs > 1){
			if (nointOpt) {
				err << "Unable to get the jobId from the input file:" << m_inOpt << "\n";
				err << "interactive questions disabled (" << wmcOpts->getAttributeUsage(Options::NOINT) << ")\n";
				err << "and multiple jobIds found in the file (this command accepts only one JobId).\n";
				err << "Adjust the file or remove the " << wmcOpts->getAttributeUsage(Options::NOINT) << " option\n";
				throw WmsClientException(__FILE__,__LINE__,
					"readOptions",DEFAULT_ERR_CODE,
					"Input Option Error", err.str());
			}
			logInfo->print (WMS_DEBUG, "JobId(s) in the input file:", Utils::getList (jobids), false);
			logInfo->print (WMS_INFO, "Multiple JobIds found:", "asking for choosing one id in the list ", true);
        		jobids = wmcUtils->askMenu(jobids, Utils::MENU_SINGLEJOBID);
			jobId = Utils::checkJobId(jobids[0]);
		}
		jobId = Utils::checkJobId(jobids[0]);
		logInfo->print (WMS_DEBUG, "JobId by input file :", jobId );
        } else {
		// from command line
        	jobId = wmcOpts->getJobId();
		logInfo->print (WMS_DEBUG, "JobId:", jobId );
        }
	// --input-file
        m_inFileOpt = wmcOpts->getStringAttribute(Options::INPUTFILE);
	// -- filename
	peekFiles = wmcOpts->getListAttribute(Options::FILENAME);
	if (!m_inFileOpt.empty() && peekFiles.size() > 0 && (getOpt||setOpt) ) {
		err << "The following options cannot be specified together:\n" ;
		err << wmcOpts->getAttributeUsage(Options::INPUTFILE) << "\n";
		err << wmcOpts->getAttributeUsage(Options::FILENAME) << "\n";
	} else if (getOpt && peekFiles.size() > 1){
		err << wmcOpts->getAttributeUsage(Options::GET) << " : no multiple filenames can be specified.\n";
		err <<  "Use the following option only once to get a job's file:\n";
		err << wmcOpts->getAttributeUsage(Options::FILENAME) << "\n";
		err << "or "  << wmcOpts->getAttributeUsage(Options::ALL) << " to retrieve all job's files.\n";
	}
	if (err.str().size() > 0) {
		throw WmsClientException(__FILE__,__LINE__,
				"readOptions",DEFAULT_ERR_CODE,
				"Input Option Error", err.str());
	}
	// --input
        if (!m_inFileOpt.empty() && (setOpt||getOpt)) {
        	peekFiles = wmcUtils->getItemsFromFile(m_inFileOpt);
		if (peekFiles.empty()) {
			throw WmsClientException(__FILE__,__LINE__,
				"readOptions",DEFAULT_ERR_CODE,
				"Input Option Error",
				"The input file is empty: " + Utils::getAbsolutePath(m_inFileOpt) );
		} else if (peekFiles.size() == 1) {
			logInfo->print (WMS_DEBUG, "filename read by input file: ", peekFiles[0] );
		} else if (nointOpt && getOpt) {
			err <<	wmcOpts->getAttributeUsage(Options::GET)  ;
			err << ": too many items in the input file; only one job's file can be requested.\n";
			err << "To specify a job's file:\nuse " << wmcOpts->getAttributeUsage(Options::FILENAME) << "\n";
			err << "or " << wmcOpts->getAttributeUsage(Options::INPUTFILE) ;
			err <<  " without " << wmcOpts->getAttributeUsage(Options::NOINT) << "\n";
			throw WmsClientException(__FILE__,__LINE__,
				"readOptions",DEFAULT_ERR_CODE,
				"Input Option Error", err.str());

		} else if (getOpt && nointOpt == false) {
			cout << "Filenames in the input file: " << Utils::getAbsolutePath(m_inFileOpt) << "\n";
			cout << wmcUtils->getStripe(74, "-") << "\n";
			peekFiles = wmcUtils->askMenu(peekFiles, Utils::MENU_SINGLEFILE);
		} else if (setOpt && nointOpt == false) {
			cout << "Filenames in the input file: " << Utils::getAbsolutePath(m_inFileOpt) << "\n";
			peekFiles =  wmcUtils->askMenu(peekFiles, Utils::MENU_FILE);
		}
		// Writes in the log file the list of filenames chosen by the input file
		int size =  peekFiles.size();
		if (size > 0) {
			for (int i = 0; i < size; i++) { files += peekFiles[i] + "; ";}
			logInfo->print (WMS_INFO, "filenames from the input file:", files, false);
		}
    	 }
	 // other incompatible options
	if ( peekFiles.empty ()  && ( setOpt||getOpt ) )  {
		err << "No valid job's file specified; use one of these options:\n";
		err << wmcOpts->getAttributeUsage(Options::FILENAME) << "\n";
		err << wmcOpts->getAttributeUsage(Options::INPUTFILE) << "\n";
		throw WmsClientException(__FILE__,__LINE__,
			"readOptions",DEFAULT_ERR_CODE,
			"Input Arguments Error",
			err.str());
	}

	// file Protocol
	m_fileProto = wmcOpts->getStringAttribute(Options::PROTO) ;
	
	if ( (setOpt || unsetOpt)  && !m_fileProto.empty()){
		logInfo->print (WMS_WARNING, "--proto: option ignored (set/unset operation doesn't need any file transfer)\n", "", true );
		m_fileProto = "";
	}

	// output file
	m_outOpt = wmcOpts->getStringAttribute(Options::OUTPUT);
	
	// File directory for --get
	m_dirOpt = wmcOpts->getStringAttribute(Options::DIR);
	
	// --nodisplay
	nodisplayOpt = wmcOpts->getBoolAttribute(Options::NODISPLAY);
	// WARNING MESSAGES
	// =================================
	// Ignored options with --unset
	if (unsetOpt){
		// each --filename specified
		int size = peekFiles.size();
		for (int i=0; i < size; i++ ) {
			warn << "--filename " + peekFiles[i]  + " (unset operation disables perusal of all job's files)\n";
		}
		// --input-file
		if (!m_inFileOpt.empty()) {
			warn << wmcOpts->getAttributeUsage(Options::INPUTFILE)  << " (unset operation disables perusal of all job's files)\n";
		}
		if (warn.str().size()>0 ){
			logInfo->print(WMS_WARNING,
				"Incompatible option(s) with " + wmcOpts->getAttributeUsage(Options::UNSET),
				warn.str( ) );
			 if (wmcUtils->answerYes ( "Do you wish to continue ?", false, true)==false){
                                // exits from the programme execution
                                cout << "bye\n";
                                Utils::ending(1);
                        }
		}
		// --all
		if (allOpt) {
			warn << wmcOpts->getAttributeUsage(Options::ALL)  << " (unset operation disables perusal of all job's files)\n";
		}
		// --nodisplay
		if (nodisplayOpt) {
			warn << wmcOpts->getAttributeUsage(Options::NODISPLAY) << " (no files to be displayed on the standard output)\n";
		}
		//--dir
		if (!m_dirOpt.empty()){
			warn << "--dir " << m_dirOpt << " (no files to be retrieved)\n";
		}
		//--output
		if (!m_outOpt.empty()) {
			warn << "--output " << m_outOpt <<" (no useful information to be saved into a file)\n";
		}
		if (warn.str().size()>0) {
			logInfo->print(WMS_WARNING,
				"The following option(s) is(are) ignored:",
				warn.str());
		}
	} else if (setOpt) {
		//--dir
		if (!m_dirOpt.empty()){
			warn << "--dir " << m_dirOpt << " (no files to be retrieved)\n";
		}
		// --nodisplay
		if (nodisplayOpt) {
			warn << wmcOpts->getAttributeUsage(Options::NODISPLAY) << " (no files to be displayed on the standard output)\n";
		}
		if (warn.str().size()>0) {
			logInfo->print(WMS_WARNING,
				"The following option(s) is(are) ignored:",
				warn.str());
		}

	} else if (getOpt) {
		// Directory path for file downloading (only for --get)
		if (m_dirOpt.empty()){
			char* environ=getenv("LOGNAME");
			if (environ){logname="/"+string(environ);}
			else{logname="/jobPerusal";}
			dircfg = Utils::getAbsolutePath(wmcUtils->getOutputStorage()) ;
			logInfo->print(WMS_DEBUG, "Output Storage (by configuration file):", dircfg);
			m_dirOpt = dircfg + logname + "_" + Utils::getUnique(jobId);
		} else {
			m_dirOpt = Utils::getAbsolutePath(m_dirOpt);
			logInfo->print(WMS_DEBUG, "Output Storage (by --dir option):", m_dirOpt);
		}
		// makes directory
		if (! Utils::isDirectory(m_dirOpt)) {
			logInfo->print(WMS_DEBUG, "Creating directory:", m_dirOpt);
			if (mkdir(m_dirOpt.c_str(), 0755)){
				// Error while creating directory
				throw WmsClientException(__FILE__,__LINE__,
				"retrieveOutput", ECONNABORTED,
				"Unable create dir: ",m_dirOpt);
			}
		}
	}
}
/**
* Performs the main operations
*/
void JobPerusal::jobPerusal ( ){
	vector<string> paths;
	// Retrieve Version
	// checks the status of the job
	checkStatus( );
	// Operation
	if (setOpt){
		perusalSet( );
		printResult(PERUSAL_SET, paths);
	} else if (getOpt) {
		perusalGet(paths);
		printResult(PERUSAL_GET, paths);
	} else if (unsetOpt) {
		perusalUnset( );
		printResult(PERUSAL_UNSET, paths);
	}
}


/******************************
*	PRIVATE methods
******************************/
/**
* Checks the status of the job
*/
void JobPerusal::checkStatus( ){
	string warnings = "";
	int code = 0;
	LbApi lbApi;
	lbApi.setJobId(jobId);
	Status status=lbApi.getStatus(true,true);
	if (getOpt){
		code = status.checkCodes(Status::OP_PERUSAL_GET, warnings);
	}else if (setOpt){
		code = status.checkCodes(Status::OP_PERUSAL_SET, warnings);
	}else if (unsetOpt){
		code = status.checkCodes(Status::OP_PERUSAL_UNSET, warnings);
	}
	if (warnings.size()>0){ logInfo->print(WMS_WARNING, warnings, "", true);}
	if (code == 0){
		// Initialize ENDPOINT (start a new (thread of) job (s)
		setEndPoint (status.getEndpoint());
		// checks if --endpoint optstatus.getEndpoint()ion has been specified with a different endpoint url
		string endpoint =  wmcOpts->getStringAttribute (Options::ENDPOINT) ;
		if (!endpoint.empty()){
			if (endpoint.compare(getEndPoint( )) !=0 ) {
				logInfo->print(WMS_WARNING, "--endpoint " + endpoint + " : option ignored", "");
			}
		}
	}
}
/**
* GET operation
*/
void JobPerusal::perusalGet (std::vector<std::string> &paths){
	vector<string> uris;
	string errors = "";
	string file = "";
	int size = 0;
	try {
		// Perform Check File Transfer Protocol Step
		jobPerformStep(STEP_CHECK_FILE_TP);
		if (peekFiles.empty()){
			throw WmsClientException(__FILE__,__LINE__,
				"perusalGet",DEFAULT_ERR_CODE,
				"Input Arguments Error",
				"No valid job's files specified");
		} else {
			file = peekFiles[0] ;
		}
		logInfo->service(WMP_GETPERUSAL_SERVICE, jobId);
			
		// Set the SOAP timeout
		setSoapTimeout(SOAP_GET_PERUSAL_FILES_TIMEOUT);
			
		uris = getPerusalFiles (jobId, file , allOpt, getContext());
	} catch (BaseException &exc) {
		throw WmsClientException(__FILE__,__LINE__,
			"perusalGet", ECONNABORTED,
			"WMProxy Server Error", errMsg(exc));
	}
	size = uris.size();
	if (size>0) {
		logInfo->result(WMP_GETPERUSAL_SERVICE, "operation successfully ended; number of files to be retrieved :" + boost::lexical_cast<string>(size));
		if (m_fileProto.compare(Options::TRANSFER_FILES_GUC_PROTO)==0) {
			this->gsiFtpGetFiles(uris, paths, errors);
		} else if (m_fileProto.compare(Options::TRANSFER_FILES_HTCP_PROTO)==0) {
			this->htcpGetFiles(uris, paths, errors);
		} else {
			errors = "File Protocol not supported: " + m_fileProto;
			errors += "List of available protocols for this client:" + Options::getProtocolsString( ) ;
			throw WmsClientException(__FILE__,__LINE__,
				"perusalGet", DEFAULT_ERR_CODE,
				"Protocol Error",
				errors);
		}
		if (paths.empty() && errors.size( )>0){
			throw WmsClientException(__FILE__,__LINE__,
				"perusalGet", DEFAULT_ERR_CODE,
				"Get Files Error",
				"GET - The following error(s) occured while transferring the file(s):\n" + errors);
		} else if  (errors.size( )>0){
			logInfo->print(WMS_WARNING,
				"GET - The following error(s) occured while transferring the file(s)\n" +errors, "");
		}
        } else {
                logInfo->result(WMP_GETPERUSAL_SERVICE, "operation successfully ended; no files to be retrieved");
        }
}
/**
* SET operation
*/
void JobPerusal::perusalSet ( ){
	try {
		logInfo->service(WMP_SETPERUSAL_SERVICE, jobId);
			
		// Set the SOAP timeout
		setSoapTimeout(SOAP_ENABLE_FILE_PERUSAL_TIMEOUT);
			
		enableFilePerusal (jobId, peekFiles, getContext());
		
	} catch (BaseException &exc) {
		throw WmsClientException(__FILE__,__LINE__,
			"enablePerusalFiles", ECONNABORTED,
			"WMProxy Server Error", errMsg(exc));
	}
	logInfo->result(WMP_SETPERUSAL_SERVICE, "operation successfully ended");
}
/**
* UNSET operation
*/
void JobPerusal::perusalUnset( ){
	vector<string> empty;
        logInfo->print(WMS_DEBUG, "Calling the " + string(WMP_SETPERUSAL_SERVICE) + " to unset the peeking for the job", jobId);
        try {
			
		// Set the SOAP timeout
		setSoapTimeout(SOAP_ENABLE_FILE_PERUSAL_TIMEOUT);
			
		enableFilePerusal (jobId, empty, getContext());
	} catch (BaseException &exc) {
		throw WmsClientException(__FILE__,__LINE__,
			"enablePerusalFiles", ECONNABORTED,
			"WMProxy Server Error", errMsg(exc));
	}
}
/**
* File downloading with globus-url-copy
*/
void JobPerusal::gsiFtpGetFiles (std::vector <std::string> &uris, std::vector<std::string> &paths, std::string &errors) {
	vector<string> params ;
	ostringstream err ;
	string source = "";
	string destination = "";
	string cmd = "";
	char* reason = NULL ;
	logInfo->print(WMS_DEBUG, "FileTransfer (gsiftp):",
		"using globus-url-copy to retrieve the file(s)");
	 while ( uris.empty() == false ){
		cmd= "globus-url-copy";
		if (getenv("GLOBUS_LOCATION")){
			cmd=string(getenv("GLOBUS_LOCATION"))+"/bin/"+cmd;
		} else if (Utils::isDirectory ("/opt/globus/bin")){
			cmd="/opt/globus/bin/"+cmd;
		} else {
			throw WmsClientException(__FILE__,__LINE__,
				"gsiFtpGetFiles", ECONNABORTED,
				"Unable to find",
				"globus-url-copy executable");
		}
		// command
		source = uris[0];
		destination = m_dirOpt + "/" + Utils::getFileName (source) ;
		if (wmcUtils->askForFileOverwriting(destination)){
			params.resize(0);
			params.push_back(source);
			params.push_back("file://"+destination);
			logInfo->print(WMS_DEBUG, "File Transfer (gsiftp) \n", "Command: "+cmd+"\n"+"Source: "+params[0]+"\n"+"Destination: "+params[1]);
			string errormsg = "";
	
			// Set the default value;
			int timeout = 0;
	
			// Check if exists the attribute SystemCallTimeout
			if(wmcUtils->getConf()->hasAttribute(JDL_SYSTEM_CALL_TIMEOUT)) {
				// Retrieve and set the attribute SystemCallTimeout
				timeout = wmcUtils->getConf()->getInt(JDL_SYSTEM_CALL_TIMEOUT);
			}
			
			// launches the command
			if (int code = wmcUtils->doExecv(cmd, params, errormsg, timeout)) {
				if (code > 0) {
					// EXIT CODE > 0
					err << " - " <<  source << "\nto: " << destination << " - ErrorCode: " << code << "\n";
					reason = strerror(code);
					if (reason!=NULL) {
						err << "   " << reason << "\n";
						logInfo->print(WMS_DEBUG, "File Transfer (gsiftp) - Transfer Failed:", reason );
					}
				} else {
					switch (code) {
						case FORK_FAILURE:
							err << "Fork Failure" << "\n" ;
							logInfo->print(WMS_DEBUG, "File Transfer (gsiftp) - Transfer Failed: ", "Fork Failure");
						case TIMEOUT_FAILURE:
							err << "Timeout Failure" << "\n" ;
							logInfo->print(WMS_DEBUG, "File Transfer (gsfitp) - Transfer Failed: ", "Timeout Failure");
						case COREDUMP_FAILURE:
							err << "Coredump Failure" << "\n" ;
							logInfo->print(WMS_DEBUG, "File Transfer (gsfitp) - Transfer Failed: ", "Coredump Failure");
					}
				}
			} else {
				paths.push_back(destination);
				logInfo->print(WMS_DEBUG, "File Transfer (gsiftp) -", "File successfully retrieved");
			}
		} else {
			logInfo->print(WMS_DEBUG, "WARNING - existing file not overwritten:", destination);
			errors += "Warning - existing file not overwritten: " + destination + "\n";
		}
		uris.erase(uris.begin());
	}
	if ((err.str()).size() >0){
		errors = "Error while downloading the following file(s):\n" + err.str( );
	}
}
/**
* File downloading with htcp
*/
void JobPerusal::htcpGetFiles (std::vector <std::string> &uris, std::vector<std::string> &paths, std::string &errors) {
	ostringstream err ;
	vector<string> params ;
	string source = "";
	string destination = "";
	string cmd= "";
	string htcp = "htcp";
	char* reason = NULL ;
	logInfo->print(WMS_DEBUG, "FileTransfer (https):",
		"using htcp to retrieve the file(s)");
	if (Utils::isDirectory ("/usr/bin")){
		htcp="/usr/bin/"+htcp;
	} else if (getenv("GLITE_LOCATION")){
		htcp=string(getenv("GLITE_LOCATION"))+"/bin/"+htcp;
	}else if (Utils::isDirectory ("/opt/glite/bin")){
		htcp="/opt/glite/bin/"+htcp;
	}else {
		throw WmsClientException(__FILE__,__LINE__,
			"htcpGetFiles", ECONNABORTED,
			"Unable to find",
			"htcp executable");
	}
	 while ( uris.empty() == false ){
		// command
		cmd= "htcp";
		logInfo->print(WMS_DEBUG, "FileTransfer (https):",
			"using " + cmd +" to retrieve the file(s)");
		if (Utils::isDirectory ("/usr/bin")){
			cmd="/usr/bin/"+cmd;
		} else if (getenv("GLITE_LOCATION")){
			cmd=string(getenv("GLITE_LOCATION"))+"/bin/"+cmd;
		}else if (Utils::isDirectory ("/opt/glite/bin")){
			cmd="/opt/glite/bin/"+cmd;
		}else {
			throw WmsClientException(__FILE__,__LINE__,
				"htcpGetFiles", ECONNABORTED,
				"Unable to find",
				"htcp executable");
		}
		//command
		source = uris[0];
		destination = m_dirOpt + "/" + Utils::getFileName (source) ;
		if (wmcUtils->askForFileOverwriting(destination)){
			params.resize(0);
			params.push_back(source);
			params.push_back("file://"+destination);
			logInfo->print(WMS_DEBUG, "File Transfer (https) \n", "Command: "+cmd+"\n"+"Source: "+params[0]+"\n"+"Destination: "+params[1]);
			string errormsg = "";
	
			// Set the default value;
			int timeout = 0;
	
			// Check if exists the attribute SystemCallTimeout
			if(wmcUtils->getConf()->hasAttribute(JDL_SYSTEM_CALL_TIMEOUT)) {
				// Retrieve and set the attribute SystemCallTimeout
				timeout = wmcUtils->getConf()->getInt(JDL_SYSTEM_CALL_TIMEOUT);
			}
			
			// launches the command
			if (int code = wmcUtils->doExecv(cmd, params, errormsg, timeout)) {
				if (code > 0) {
					// EXIT CODE > 0
					err << " - " <<  source << "\nto: " << destination << " - ErrorCode: " << code << "\n";
					reason = strerror(code);
					if (reason!=NULL) {
						err << "   " << reason << "\n";
						logInfo->print(WMS_DEBUG, "File Transfer (https) - Transfer Failed:", reason );
					}
				} else {
					switch (code) {
						case FORK_FAILURE:
							err << "Fork Failure" << "\n" ;
							logInfo->print(WMS_DEBUG, "File Transfer (https) - Transfer Failed: ", "Fork Failure");
						case TIMEOUT_FAILURE:
							err << "Timeout Failure" << "\n" ;
							logInfo->print(WMS_DEBUG, "File Transfer (https) - Transfer Failed: ", "Timeout Failure");
						case COREDUMP_FAILURE:
							err << "Coredump Failure" << "\n" ;
							logInfo->print(WMS_DEBUG, "File Transfer (https) - Transfer Failed: ", "Coredump Failure");
					}
				}
			} else {
				paths.push_back(destination);
				logInfo->print(WMS_DEBUG, "File Transfer (https) -", "File successfully retrieved");
			}
		} else {
			logInfo->print(WMS_DEBUG, "WARNING - existing file not overwritten:", destination);
			errors += "Warning - existing file not overwritten: " + destination + "\n";
		}
		uris.erase(uris.begin());
	}
	if ((err.str()).size() >0){
		errors = "Error while downloading the following file(s):\n" + err.str( );
	}
}
/**
* Prints out the final results on the std-out
*/
void JobPerusal::printResult(const perusalOperations &operation, std::vector<std::string> &paths){
	ostringstream out;
	vector<string> params ;
	string subj = "";
	string verb = "";
	string count = "";
	string cmd = "";
	string ws = " ";
	string header = "";
	int size = 0;
	out << "\n" << wmcUtils->getStripe(74, "=" , string (wmcOpts->getApplicationName() + " Success") ) << "\n\n";
	if (peekFiles.size() == 1 ) {
		subj = "File perusal";
		verb = "has";
		count = "name of the file";
	} else {
		subj = "Files perusal";
		verb = "have";
		count = "list of files";
	}
	if (operation == PERUSAL_SET) {
		out << subj << ws << "has been successfully enabled for the job:\n";
		out << jobId << "\n";
		// saves the result
		if (!m_outOpt.empty()){
			header = "###" + subj + ws + "enabled for the job " + jobId + "###";
			if ( wmcUtils->saveListToFile(m_outOpt,  peekFiles, header) < 0 ){
				logInfo->print (WMS_WARNING, "unable to save the result into the output file " ,
						 Utils::getAbsolutePath(m_outOpt));
			} else {
				out << "\nThe" + ws + count + ws + "to peruse" + ws + "has been saved in the following file:\n";
				out << Utils::getAbsolutePath(m_outOpt) << "\n";
			}
		}
	} else if (operation == PERUSAL_GET) {
		if (paths.empty()) {
			out << "No" << ws << "files" << ws << "to be retrieved for the job:\n";
			out << jobId << "\n";
		} else {
			out << "The retrieved files have been successfully stored in:\n";
			out << m_dirOpt << "\n";
			if (!m_outOpt.empty()){
				header = "###Perusal: file(s) retrieved for the job " + jobId + "###";
				if ( wmcUtils->saveListToFile(m_outOpt, paths, header) < 0 ){
					logInfo->print (WMS_WARNING, "unable to save" + ws + count +  ws + "to peruse" + ws + "in the output file " ,
						Utils::getAbsolutePath(m_outOpt));
				} else {
					out << "\nThe"<< ws << count <<  ws << "to peruse" << ws << "is stored in the file:\n";
					out << Utils::getAbsolutePath(m_outOpt) << "\n";
				}
			}
		}
	} else if (operation == PERUSAL_UNSET) {
		out << "File(s) perusal has been successfully disabled for the job:" << endl;
		out << jobId << "\n";
	}
	out << "\n" << wmcUtils->getStripe(74, "=") << "\n\n";
	out << getLogFileMsg ( ) << "\n";
	// Prints out the msg on the std out
	cout << out.str();
	// GET: shows the files
	if ( operation == PERUSAL_GET && ! nodisplayOpt && paths.size( ) > 0 ) {
		size = paths.size( );
		for (int i = 0; i < size ; i++) {
			cout << wmcUtils->getStripe(74, "-") << "\n";
			cout << "file " << (i+1) << "/" << size << ": " << Utils::getFileName(paths[i]) << "\n";
			cout << wmcUtils->getStripe(74, "-") << "\n\n";
			params.resize(0);
			params.push_back(string(paths[i]));
			string errormsg = "";
			cmd = DISPLAY_CMD;
	
			// Set the default value;
			int timeout = 0;
	
			// Check if exists the attribute SystemCallTimeout
			if(wmcUtils->getConf()->hasAttribute(JDL_SYSTEM_CALL_TIMEOUT)) {
				// Retrieve and set the attribute SystemCallTimeout
				timeout = wmcUtils->getConf()->getInt(JDL_SYSTEM_CALL_TIMEOUT);
			}

			// launches the command
			if (int outcome = wmcUtils->doExecv(cmd, params, errormsg, timeout)) {
                                // EXIT CODE !=0
				switch (outcome) {
					case FORK_FAILURE:
						logInfo->print(WMS_DEBUG, "File Transfer (gsiftp) - Transfer Failed: ", "Fork Failure");
						break;
					case TIMEOUT_FAILURE:
						logInfo->print(WMS_DEBUG, "File Transfer (gsiftp) - Transfer Failed: ", "Timeout Failure");
						break;
					case COREDUMP_FAILURE:
						logInfo->print(WMS_DEBUG, "File Transfer (gsiftp) - Transfer Failed: ", "Coredump Failure");
						break;
				}
			}
		}
	}
}

}}}} // ending namespaces
