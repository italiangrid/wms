/**
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
*       Authors:        Alessandro Maraschini <alessandro.maraschini@datamat.it>
*                       Marco Sottilaro <marco.sottilaro@datamat.it>
*/

//      $Id$

#include "joboutput.h"
#include "lbapi.h"
#include <string>
#include <sys/stat.h> //mkdir
#include "netdb.h" //gethostbyname
// streams
#include<sstream>
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
// BOOST
#include "boost/filesystem/path.hpp" // path
#include "boost/filesystem/exception.hpp" //managing boost errors
#include <boost/lexical_cast.hpp>

using namespace std ;
using namespace glite::wms::client::utilities ;
using namespace glite::wms::wmproxyapi;
using namespace glite::wms::wmproxyapiutils;
namespace fs = boost::filesystem ;

namespace glite {
namespace wms{
namespace client {
namespace services {

const int SUCCESS = 0;
const int FAILED = -1;
const int FORK_FAILURE = -1;
const int COREDUMP_FAILURE = -2;
const int TIMEOUT_FAILURE = -3;
const int HTTP_OK = 200;
const int TRANSFER_OK = 0;
const bool   GENERATE_NODE_NAME =true;  // Determine whether to use or not node approach
const string GENERATED_JN_FILE  ="ids_nodes.map" ; // Determine the id/nodes filename

/*
* Default constructor
*/
JobOutput::JobOutput () : Job() {
	// init of the string  attributes
	m_inOpt = "";
	m_dirOpt = "";
	// init of the output storage location
	dirCfg = "/tmp";
	// init of the boolean attributes
	listOnlyOpt = false;
	//nopgOpt = false;
	// list of files
	childrenFileList = "";
	parentFileList = "";
	hasFiles = false ;
	successRt = false;
	m_warnsList = "";
}
/*
* Default Destructor
*/
JobOutput::~JobOutput ()  {
}

void JobOutput::readOptions ( int argc,char **argv)  {
	unsigned int njobs = 0;
	ostringstream err ;
 	Job::readOptions  (argc, argv, Options::JOBOUTPUT);
	//--nopurg
	nopgOpt = wmcOpts->getBoolAttribute(Options::NOPURG);
        // --input
        // input file
        m_inOpt = wmcOpts->getStringAttribute(Options::INPUT);
	
	// JobId's
        if (!m_inOpt.empty()){
		// From input file
		logInfo->print (WMS_DEBUG, "Reading JobId(s) from the input file:", Utils::getAbsolutePath(m_inOpt));
        	jobIds = wmcUtils->getItemsFromFile(m_inOpt);
		logInfo->print (WMS_DEBUG, "JobId(s) in the input file:", Utils::getList (jobIds), false);
        } else {
        	jobIds = wmcOpts->getJobIds();
        }
        jobIds = wmcUtils->checkJobIds (jobIds);
	njobs = jobIds.size( ) ;
	if ( njobs > 1 && ! wmcOpts->getBoolAttribute(Options::NOINT) ){
		logInfo->print (WMS_DEBUG, "Multiple JobIds found:", "asking for choosing one or more id(s) in the list ", false);
        	jobIds = wmcUtils->askMenu(jobIds, Utils::MENU_JOBID);
		if (jobIds.size() != njobs) {
			logInfo->print (WMS_DEBUG, "Chosen JobId(s):", Utils::getList (jobIds), false);
		}
         }
        // --dir , OutputStorage or DEFAULT_OUTPUT value
        m_dirOpt = wmcOpts->getStringAttribute(Options::DIR);
	
	  // --listonly
        listOnlyOpt = wmcOpts->getBoolAttribute( Options::LISTONLY ) ;
	if (listOnlyOpt && !m_dirOpt.empty()) {
		ostringstream info ;
		info << "the following options cannot be specified together:\n" ;
		info << wmcOpts->getAttributeUsage(Options::DIR) << "\n";
		info << wmcOpts->getAttributeUsage(Options::LISTONLY) << "\n";
		throw WmsClientException(__FILE__,__LINE__,
				"readOptions",DEFAULT_ERR_CODE,
				"Input Option Error", info.str());
	}
	if (!listOnlyOpt){
		if (m_dirOpt.empty()){
			dirCfg =Utils::getAbsolutePath(wmcUtils->getOutputStorage()) ;
			logInfo->print(WMS_DEBUG, "Output Storage (by configuration file):", dirCfg);
		} else {
			m_dirOpt = Utils::getAbsolutePath(m_dirOpt);
			logInfo->print(WMS_DEBUG, "Output Storage (by --dir option):", m_dirOpt);
		}
	}
	// file Protocol
	m_fileProto = wmcOpts->getStringAttribute(Options::PROTO) ;

	// Perform Check File Transfer Protocol Step
	jobPerformStep(STEP_CHECK_FILE_TP);
}
/******************************
*	getOutput method
******************************/
void JobOutput::getOutput ( ){
	int code = FAILED;
	ostringstream out ;
	string result="";
	int size = 0;
	// checks that the jobids vector is not empty
	if (jobIds.empty()){
		throw WmsClientException(__FILE__,__LINE__,
			"getOutput", DEFAULT_ERR_CODE,
			"JobId Error",
			"No valid JobId for which the output can be retrieved" );
	}
	// number of jobs
	size = jobIds.size ( );
	// performs output retrieval
	char* environ=getenv("LOGNAME");
	if (environ){logName="/"+string(environ);}
	else{logName="/jobOutput";}
	LbApi lbApi;
	// Jobid's loop
	vector<string>::iterator it = jobIds.begin() ;
	vector<string>::iterator const end = jobIds.end();
        for ( ; it != end ; it++){
		// JobId
		lbApi.setJobId(*it);
		try{
			Status status=lbApi.getStatus(true,true);
			// Initialize ENDPOINT (start a new (thread of) job (s)
			setEndPoint (status.getEndpoint());
			// Properly set destination Directory
			if (!m_dirOpt.empty()){
				if ( size == 1 ){
					retrieveOutput (result,status,Utils::getAbsolutePath(m_dirOpt), true);
				} else {
					retrieveOutput (result,status,Utils::getAbsolutePath(m_dirOpt)+logName+"_"+Utils::getUnique(*it), true);
				}
			}else{
				retrieveOutput (result,status,dirCfg+logName+"_"+Utils::getUnique(*it), true);
			}
			// if the output has been successfully retrieved for at least one job
			code = SUCCESS;
		} catch (WmsClientException &exc){
			// cancellation not allowed due to its current status
			// If the request contains only one jobid, an exception is thrown
			if (size == 1){ throw exc ;}
			string wmsg = (*it) + ": not allowed to retrieve the output (" + exc.what( ) +")";
			// if the request is for multiple jobs, a failed-string is built for the final message
			createWarnMsg(wmsg);
			// goes on with the next jobId
			continue ;
		}
	}
	if (code == SUCCESS && successRt){
		out << "\n" << wmcUtils->getStripe(80, "=" , "") << "\n\n";
		out << "\t\t\tJOB GET OUTPUT OUTCOME\n\n";
		if (listOnlyOpt && hasFiles){
			out << parentFileList  << childrenFileList ;
			// Prints the results into the log file
			logInfo->print(WMS_INFO,  string(parentFileList+childrenFileList), "", false );
		} else {
			out << result ;
			// Prints the results into the log file
			logInfo->print (WMS_INFO,  result, "", false );
		}
		out << wmcUtils->getStripe(80, "=" , "" ) << "\n\n";
		// Warnings/errors messages
		if (  wmcOpts->getBoolAttribute(Options::DBG) && !m_warnsList.empty()) {
			out << m_warnsList << "\n";
		}

	} else {
		string err = "";
		if (size==1) { err ="Unable to retrieve the output"; }
		else { err ="Unable to retrieve the output for any job"; }
		throw WmsClientException(__FILE__,__LINE__,
				"output", ECONNABORTED,
				"Operation Failed",
				err  );
	}
	// logfile
	out << getLogFileMsg ( ) << "\n";
	// STD-OUT
	cout << out.str ( );
}
/*********************************************
*	PRIVATE METHODS:
**********************************************/
int JobOutput::retrieveOutput (std::string &result, Status& status, const std::string& dirAbs, bool firstCall, const bool &child){
	string errors = "";
	string wmsg = "" ;
	string id = "";
	// Dir Creation Management
	bool createDir=false;
	bool checkChildren = true;
	// JobId
	glite::wmsutils::jobid::JobId jobid = status.getJobId();
	id = jobid.toString() ;
	logInfo->print(WMS_DEBUG,"Checking the status of the job:" , id);
	logInfo->print(WMS_DEBUG,"Found Status:", status.getStatusName());
	// Check Status (if needed)
	int code = status.checkCodes(Status::OP_OUTPUT, errors, child);
	if (errors.size()>0){
		wmsg = id + ": " + errors ;
		createWarnMsg(wmsg);
	}
	if (!listOnlyOpt &&  (code == 0 || ! child )){
		// It's not a node & retrieval is allowed (code==0)
		if (Utils::isDirectory(dirAbs)){
			logInfo->print(WMS_WARNING, "Directory already exists: ", dirAbs);
			if (!wmcUtils->answerYes ("Do you wish to overwrite it ?", false, true)){
				cout << "bye" << endl ;
				wmcUtils->ending(ECONNABORTED);
			}
		}else {
			createDir=true;
			if (mkdir(dirAbs.c_str(), 0755)){
				// Error while creating directory
				throw WmsClientException(__FILE__,__LINE__,
					"retrieveOutput", ECONNABORTED,
					"Unable create dir",dirAbs);
			}
		}
	}
	std::vector<Status> children = status.getChildrenStates();
	// Retrieve Output Files management
	if (code == 0){
		// OutputFiles Retrieval NOT allowed for DAGS (collection, partitionables, parametrics...)
		// i.e. ANY job that owns children
		if (children.size()==0){
			// reset of the errors string
			errors = string("");
			// actually retrieve files
			if (retrieveFiles (result, errors, id,dirAbs, child)){
				// Something has gone bad, no output files stored then purge directory
				if (createDir) {
					rmdir(dirAbs.c_str());
				}
			}
			if (errors.size()>0){
				wmsg = id + ": " + errors ;
				createWarnMsg(wmsg);
			}
			successRt = true;
			checkChildren = false;
		}else if (listOnlyOpt){
			// IT is a DAG, no output files to be retrieved.
			// Print a simple output
			ostringstream out ;
			out << "\nJobId: " << id << "\n";
			parentFileList = out.str();
		}
	}
	// Children (if present) Management
	if (children.size()){
		string result = "" ;
		string msgNodes ="";
		unsigned int size = children.size();
		if (GENERATE_NODE_NAME){
			msgNodes = "Dag JobId: " + jobid.toString() ;
			std::map< std::string, std::string > map;
			if (checkWMProxyRelease()){
				// Calling wmproxy Server method
				try {
					logInfo->service(WMP_JDL_SERVICE, jobid.toString());
					// Retrieve JDL
			
					// Set the SOAP timeout
					setSoapTimeout(SOAP_GET_JDL_TIMEOUT);
			
					string JDLretrieved=getJDL(jobid.toString(), glite::wms::wmproxyapi::REGISTERED,getContext());
					map = AdUtils::getJobIdMap(JDLretrieved);
					logInfo->result(WMP_JDL_SERVICE, "JDL successfully retrieved for jobid: "+jobid.toString());
				} catch (BaseException &exc) {
					string wmsg =  "";
					if (exc.Description){ wmsg +=" (" + *(exc.Description)+ ")"; }
					throw WmsClientException(__FILE__,__LINE__,
						"retrieveOutput", DEFAULT_ERR_CODE,
						"Unable to retrieve JDL",wmsg);
				}
			}else {
				map= AdUtils::getJobIdMap(status.getJdl());
			}
			for (unsigned int i = 0 ; i < size ;i++){
				// update message with node
				msgNodes+= "\n\t - - -";
				msgNodes+= "\n\tNode Name:\t"
					+ AdUtils::JobId2Node(map,children[i].getJobId()) ;
				msgNodes+= "\n\tJobId:    \t"
					+ children[i].getJobId().toString();
				msgNodes+= "\n\tDir:      \t"
					+ dirAbs+"/"
					+ AdUtils::JobId2Node(map,children[i].getJobId()) ;
				retrieveOutput (result, children[i],
					dirAbs+"/"+
					AdUtils::JobId2Node(map,children[i].getJobId()),false, true);
			}
			// Evantually print info inside file
			wmcUtils->saveToFile(dirAbs+"/"+GENERATED_JN_FILE, msgNodes);
			logInfo->print (WMS_DEBUG, jobid.toString()
				+": Nodes and JobIds info stored inside file:",dirAbs+"/"+GENERATED_JN_FILE);
		}else{
			for (unsigned int i = 0 ; i < size ;i++){
				retrieveOutput (result,children[i],
					dirAbs+"/"+
					children[i].getJobId().getUnique(), false, true);
			}
		}
	}
	//bool parent = status.hasParent ( ) ;
	/* Purge logic: Job can be purged when:
	* 1) enpoint has been specified (parent has specified)
	* 2) --nopurge is not active
	* 3) retrieve output successfully done */
	bool purge = (!listOnlyOpt) && ( getEndPoint() != "" ) && (nopgOpt == false) && (code==0) && (firstCall == true);
	id = jobid.toString() ;
	// checks Children
	if (checkChildren && children.size()>0){
		if (hasFiles){
			result += "Output sandbox files for the DAG/Collection :\n" +  id ;
			result += "\nhave been successfully retrieved and stored in the directory:\n" + dirAbs + "\n\n";
		} else{
			result += "No output files to be retrieved for the DAG/Collection:\n" + id + "\n\n";
			if (createDir) {
				// remove created empty dir
				rmdir(dirAbs.c_str());
			}
		}
		// It's a dag, no outputfiles to be retrieved
	}else if  (!hasFiles){ //It's a job with no output files (and no children)
		if (createDir) {
			// remove created empty dir
			rmdir(dirAbs.c_str());
		}
		result += "No output files to be retrieved for the job:\n" + id + "\n\n";
	}
	 if (purge) {
		try {
			// Check Dir/purge
			logInfo->service(WMP_PURGE_SERVICE, id);

			// Set the SOAP timeout
			setSoapTimeout(SOAP_JOB_PURGE_TIMEOUT);

			jobPurge(jobid.toString(),getContext());
                        logInfo->result(WMP_PURGE_SERVICE, "The purging request has been successfully sent");
		} catch (BaseException &exc) {
			string wmsg =  "";
			if (exc.Description){ wmsg +=" (" + *(exc.Description)+ ")"; }
			logInfo->print (WMS_WARNING, "JobPurging not allowed",wmsg, true );
		}
	} else if ( nopgOpt == true ) {
			string wmsg = "Option --nopurge specified" ;
			logInfo->print (WMS_DEBUG, "Skipping JobPurging: ",wmsg, true );
		}
	return 0;
}
bool JobOutput::retrieveFiles (std::string &result, std::string &errors, const std::string& jobid, const std::string& dirAbs, const bool &child){
	vector <pair<string , long> > files ;
	vector <pair<string,string> > paths ;
	string filename = "";
	string err = "";
	bool ret = true;
	try {
		// gets the list of the out-files from the EndPoint
		logInfo->service(WMP_OUTPUT_SERVICE, jobid);
			
		// Set the SOAP timeout
		setSoapTimeout(SOAP_GET_OUTPUT_FILE_LIST_TIMEOUT);
			
		files = getOutputFileList(jobid, getContext(), m_fileProto);
		logInfo->result(WMP_OUTPUT_SERVICE, "The list of output files has been successfully retrieved");
		hasFiles = hasFiles || (files.size()>0);
	} catch (BaseException &exc) {
		string desc = "";
		if (exc.Description){ desc =" (" + *(exc.Description)+ ")"; }
		if (child) {
			string wmsg = jobid + ": not allowed to retrieve the output" + desc ;
			createWarnMsg (wmsg);
			ret = false ;
		} else {
			throw WmsClientException(__FILE__,__LINE__,
				"retrieveFiles", ECONNABORTED,
				"getOutputFileList Error", desc);
		}
	}
	// files successfully retrieved
	if (files.size() == 0){
		if (listOnlyOpt){
			this->listResult(files, jobid, child);
		}
	}else{
		// match the file to be downloaded to a local directory pathname
		if (!listOnlyOpt){
			// Actually retrieving files
			logInfo->print(WMS_DEBUG, "Retrieving Files for: ", jobid);
			unsigned int size = files.size( );
			//checks if the protocol is available server side
			bool availableproto = false ;
			for (unsigned int i = 0; i < size; i++){
				//wmproxy server < 3.1
				if ( !checkWMProxyRelease()) {
                                                if( m_fileProto.compare(Utils::getProtocol (files[i].first)) == 0) {
							availableproto = true;
						}
                                        } else {
					//wmproxy server > 3.0
					availableproto = true ;
					}
				filename = Utils::getFileName(files[i].first);
				paths.push_back( make_pair (files[i].first, string(dirAbs +"/" + filename) ) );
			}
			//protocol is available, therefore can retrieve files
			if (availableproto) {
				if (m_fileProto.compare(Options::TRANSFER_FILES_GUC_PROTO)==0) {
					this->gsiFtpGetFiles(paths, errors);
				} else if (m_fileProto.compare(Options::TRANSFER_FILES_HTCP_PROTO)==0) {
					this->htcpGetFiles(paths, errors);
				} else {
					err = "File Protocol not supported: " + m_fileProto;
					throw WmsClientException(__FILE__,__LINE__,
						"retrieveFiles", DEFAULT_ERR_CODE,
						"Protocol Error", err);
				}
			} else {
				err = "File Protocol not supported: " + m_fileProto;
				throw WmsClientException(__FILE__,__LINE__,
					"retrieveFiles", DEFAULT_ERR_CODE,
					"Protocol Error", err);
			}
			// Result message
			result += "Output sandbox files for the job:\n" + jobid  ;
			result += "\nhave been successfully retrieved and stored in the directory:\n" + dirAbs + "\n\n";
		} else {
			// Prints file list (only verbose result)
			this->listResult(files, jobid, child);
		}
	}
	return ret;
}
/*
*	prints file list on std output
*/
void JobOutput::listResult(std::vector <std::pair<std::string , long> > &files, const std::string jobid, const bool &child){
	string ws = " ";
	ostringstream out ;
	// output message
	if (child){
		out << "\n\t>> child: " << jobid << "\n";
		if (files.size( ) == 0 ){
			out << "\tno output file to be retrieved\n";
		} else{
			vector <pair<string , long> >::iterator it1  = files.begin();
			vector <pair<string , long> >::iterator const end1 = files.end() ;
			for (  ; it1 != end1 ; it1++ ){
				out << "\t - file: " << it1->first << "\n";
				out << "\t   size (bytes): " << it1->second << "\n";
			}
		}
	} else{
		out << "\nJobId: " << jobid << "\n";
		if (files.size( ) == 0 ){
			out << "no output file to be retrieved\n";
		} else{
			vector <pair<string , long> >::iterator it2  = files.begin();
			vector <pair<string , long> >::iterator const end2 = files.end() ;
			for (  ; it2 != end2 ; it2++ ){
				out << " - file: " << it2->first << "\n";
				out << "   size (bytes): " << it2->second << "\n";
			}
		}
	}
	if (child){
		childrenFileList += out.str();
	} else{
		
	}
}
/*
* Creates the final Warning Msg
*/
void JobOutput::createWarnMsg(const std::string &msg ){
	int size = msg.size();
	if (size>0) {
		logInfo->print(WMS_WARNING, msg , "" , true );
		if (!m_warnsList.empty() ){
			m_warnsList += "- " + msg + "\n";
		} else if (size>0 ){
			m_warnsList = "The following warnings/errors have been found during the operation(s):\n";
			m_warnsList += "========================================================================\n";
			m_warnsList += "- " + msg + "\n";
		}
	}
}

/*
*	gsiFtpGetFiles Method  (WITH_GRID_FTP)
*/
void JobOutput::gsiFtpGetFiles (std::vector <std::pair<std::string , std::string> > &paths, std::string &errors) {
	vector<string> params ;
	ostringstream err ;
	string source = "";
	string destination = "";
	string cmd= "";
	char* reason = NULL ;
	string globurlcp = "globus-url-copy";
	if (getenv("GLOBUS_LOCATION")){
		globurlcp=string(getenv("GLOBUS_LOCATION"))+"/bin/"+globurlcp;
	}else if (Utils::isDirectory ("/opt/globus/bin")){
		globurlcp="/opt/globus/bin/"+globurlcp;
	}else {
		throw WmsClientException(__FILE__,__LINE__,
			"gsiFtpGetFiles", ECONNABORTED,
			"Unable to find",
			"globus-url-copy executable");
	}
	 while ( paths.empty() == false ){
		// command
		cmd= "globus-url-copy";
		logInfo->print(WMS_DEBUG, "FileTransfer (gsiftp):",
			"using " + cmd +" to retrieve the file(s)");
		if (getenv("GLOBUS_LOCATION")){
			cmd=string(getenv("GLOBUS_LOCATION"))+"/bin/"+cmd;
		}else if (Utils::isDirectory ("/opt/globus/bin")){
			cmd="/opt/globus/bin/"+cmd;
		}else {
			throw WmsClientException(__FILE__,__LINE__,
				"gsiFtpGetFiles", ECONNABORTED,
				"Unable to find",
				"globus-url-copy executable");
		}
		source = paths[0].first ;
		destination = paths[0].second ;
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
				err << " - " <<  source << " to " << destination << " - ErrorCode: " << code << "\n";
				reason = strerror(code);
				if (reason!=NULL) {
					err << "   " << reason << "\n";
					logInfo->print(WMS_DEBUG, "File Transfer (gsiftp) - Transfer Failed:", reason );
				}
			}else {
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
			logInfo->print(WMS_DEBUG, "File Transfer (gsiftp):", "File successfully retrieved");
		}
		paths.erase(paths.begin());
         }
	 if ((err.str()).size() >0){
		errors = "Error while downloading the following file(s):\n" + err.str( );
	 }
}
/*
*	File downloading by htcp
*/
void JobOutput::htcpGetFiles (std::vector <std::pair<std::string , std::string> > &paths, std::string &errors) {
	ostringstream err ;
	vector<string> params ;
	string source = "";
	string destination = "";
	string cmd= "";
	string htcp = "htcp";
	char* reason = NULL ;
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
	 while ( paths.empty() == false ){
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
		source = paths[0].first ;
		if (checkWMProxyRelease()) {
			source = wmcUtils->resolveAddress( source ) ;
		}
		destination = paths[0].second ;
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
				err << " - " <<  source << " to " << destination << " - ErrorCode: " << code << "\n";
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
			logInfo->print(WMS_DEBUG, "File Transfer (https):", "File successfully retrieved");
		}
		paths.erase(paths.begin());
         }
	 if ((err.str()).size() >0){
		errors = "Error while downloading the following file(s):\n" + err.str( );
	 }
}

}}}} // ending namespaces

