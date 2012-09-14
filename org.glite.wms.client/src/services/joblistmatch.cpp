/**
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
*       Authors:        Alessandro Maraschini <alessandro.maraschini@datamat.it>
*                       Marco Sottilaro <marco.sottilaro@datamat.it>
*/

//      $Id$

#include "joblistmatch.h"
// streams
#include <sstream>
#include <iostream>
// exceptions
#include "utilities/excman.h"
// wmp-client utilities
#include "utilities/utils.h"
// wmproxy API
#include "glite/wms/wmproxyapi/wmproxy_api_utilities.h"
// Ad attributes and JDL methods
#include "glite/jdl/jdl_attributes.h"
#include "glite/jdl/JDLAttributes.h"
#include "glite/jdl/extractfiles.h"
#include "glite/jdl/adconverter.h"

using namespace std ;
using namespace glite::wms::client::utilities ;
using namespace glite::wms::wmproxyapi;
using namespace glite::wms::wmproxyapiutils;
using namespace glite::jdl;

namespace glite {
namespace wms{
namespace client {
namespace services {


JobListMatch::JobListMatch(){
	// init of the boolean attributes
        rankOpt  = false ;
	// parameters
        m_jdlFile = "";
        m_jdlString = "";
        //Ad
        jobAd = NULL;
  };
/*
*	Default destructor
*/
JobListMatch::~JobListMatch( ){
        if (jobAd) { delete(jobAd);}
};
/*
* performs the main operations
*/
void JobListMatch::readOptions (int argc,char **argv) {
	Job::readOptions  (argc, argv, Options::JOBMATCH);
 	// path to the JDL file
  	m_jdlFile = wmcOpts->getPath2Jdl( );
	//EndPoint
	retrieveEndPointURL( );
	// rank
        rankOpt =  wmcOpts->getBoolAttribute (Options::RANK);
	// checks if the output file already exists
	if (!m_outOpt.empty() && !wmcUtils->askForFileOverwriting(m_outOpt) ){
		cout << "bye\n";
		getLogFileMsg ( );
		Utils::ending(ECONNABORTED);
	}
};
/*
* performs the main operations
*/
void JobListMatch::listMatching ( ){
	const int tab = 60 ;
        const string ws = " ";
        int spaces = 0 ;
        string ce = "";
        vector <pair<string , long> > list ;
	ostringstream out ;
	ostringstream os;
        // Reads and checks the JDL
        checkAd( );
        // list matching ....
        list = jobMatching( );
	if (list.empty()){
		// if no resource has been found
		out << wmcUtils->getStripe(74, "=" , string (wmcOpts->getApplicationName() + " failure") ) << "\n";
		out << "No Computing Element matching your job requirements has been found!";
	} else {


		out  << wmcUtils->getStripe(74, "=") << "\n\n";
		out << "\t\t     COMPUTING ELEMENT IDs LIST ";
		out << "\n The following CE(s) matching your job requirements have been found:\n";
		out << "\n\t*CEId*";
		if (rankOpt) {
			spaces = tab - 9  ;
			for ( int i = 0; i < spaces ; i++ ) { out << ws ; }
			out << "*Rank*\n";
		}
		out << "\n";
		vector <pair<string , long> > ::iterator it = list.begin( );
		vector <pair<string , long> > ::iterator const end = list.end( );
		for (  ; it != end;  it++ ){
			ce = it->first ;
			out << " - " << it->first ;
			if(rankOpt) {
				spaces = tab - ce.size ( );
				for ( int i = 0; i < spaces+5 ; i++ ) { out << ws ; }
				out << it->second ;
			}
			out << "\n";
		}
	}
        // saves the result
        if (!m_outOpt.empty()){
        	string tofile = "\n" + out.str() + "\n" + wmcUtils->getStripe(74, "=") + "\n";
                if ( wmcUtils->saveToFile(m_outOpt, tofile) < 0 ){
                        logInfo->print (WMS_WARNING, "Unable to write the list of CeId's to the output file: " , Utils::getAbsolutePath(m_outOpt));
                } else{
                        logInfo->print (WMS_DEBUG, "Computing Element(s) matching your job requirements have been stored in the file:",
				Utils::getAbsolutePath(m_outOpt), false);
			os << wmcUtils->getStripe(84, "=" , string (wmcOpts->getApplicationName() + " success") ) << "\n";
                        os << "\nComputing Element(s) matching your job requirements have been stored in the file:\n";
                        os << Utils::getAbsolutePath(m_outOpt) << "\n";
			os << "\n" << wmcUtils->getStripe(84, "=") << "\n\n";
			cout << os.str();
                }
        } else{
		// if no output file ....
		out << "\n" << wmcUtils->getStripe(74, "=") << "\n\n";
		cout << out.str( );
	}
	// log file info
	cout << getLogFileMsg ( ) << "\n";
};

/***********************************************
* private methods
************************************************/
/*
* checks the JDL
*/
void JobListMatch::checkAd ( ){
	glite::jdl::Ad *wmcConf =wmcUtils->getConf();
	if ( !jobAd ){
		jobAd = new Ad ( );
        }
	if (m_jdlFile.empty()){
		throw WmsClientException(__FILE__,__LINE__,
			"checkAd",  DEFAULT_ERR_CODE,
			"JDL File Missing",
                         "uknown JDL file pathame  (Last argument of the command must be a JDL file)"   );
        }
	logInfo->print (WMS_DEBUG, "The JDL file is:", Utils::getAbsolutePath(m_jdlFile));
	jobAd->fromFile (m_jdlFile);
        // check submitTo
	 if ( jobAd->hasAttribute(JDL::SUBMIT_TO) ){
		throw WmsClientException(__FILE__,__LINE__,
			"checkAd",  DEFAULT_ERR_CODE,
        		"submitTo" ,
                        "Forcing CEId for job-list-match does not make sense");
        }

	if ( jobAd->hasAttribute(JDL::TYPE , JDL_TYPE_COLLECTION) ) {
		throw WmsClientException(__FILE__,__LINE__,
			"checkAd",  DEFAULT_ERR_CODE,
        		"Not supported Type" ,
                        "Collection type is not supported for list match");
        } else if ( jobAd->hasAttribute(JDL::TYPE , JDL_TYPE_DAG) ) {
		throw WmsClientException(__FILE__,__LINE__,
			"checkAd",  DEFAULT_ERR_CODE,
        		"Not supported Type" ,
                        "DAG type is not supported for list match");
	}
	// Simple Ad manipulation
	if (!jobAd->hasAttribute (JDL::VIRTUAL_ORGANISATION)){
		jobAd->setAttribute(JDL::VIRTUAL_ORGANISATION, wmcUtils->getVirtualOrganisation());
	}
	AdUtils::setDefaultValuesAd(jobAd,wmcConf);
	JobAd *pass= new JobAd(*(jobAd->ad()));
	AdUtils::setDefaultValues(pass,wmcConf);
	// JDL STRING
	m_jdlString = pass->toSubmissionString();
	logInfo->print(WMS_DEBUG, "JDL" , m_jdlString );
};

/*
* Retrieves the list of matching resources
*/
std::vector <std::pair<std::string , long> > JobListMatch::jobMatching( ) {
	vector <pair<string , long> > list ;
	// checks if jdlstring is not null
        if (m_jdlString.empty()){
                throw WmsClientException(__FILE__,__LINE__,
                        "jobMatching",  DEFAULT_ERR_CODE ,
                        "Null Pointer Error",
                        "Null pointer to JDL string");
        }
	logInfo->print(WMS_DEBUG, "Sending the request to the service", getEndPoint( ));
 	try{
  		// ListMatch
		logInfo->service(WMP_LISTMATCH_SERVICE);
			
		// Set the SOAP timeout
		setSoapTimeout(SOAP_JOB_LIST_MATCH_TIMEOUT);
			
    		list = jobListMatch(m_jdlString, getDelegationId( ), getContext());
		logInfo->result(WMP_LISTMATCH_SERVICE, "The MatchMaking operations have been successfully performed");
      } catch (BaseException &exc) {
		throw WmsClientException(__FILE__,__LINE__,
		"jobListMatch", ECONNABORTED,
		"Operation failed",
		"Unable to perform the operation: "  + errMsg(exc));
  	}
       return list ;
}

}}}} // ending namespaces

