/**
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
*       Authors:        Alessandro Maraschini <alessandro.maraschini@datamat.it>
*                       Marco Sottilaro <marco.sottilaro@datamat.it>
*/

//      $Id$


#include "delegateproxy.h"
// streams
#include "sstream"
#include "iostream"
// time fncts
#include "time.h"
// exceptions
#include "utilities/excman.h"

// wmproxy-api
#include "glite/wms/wmproxyapi/wmproxy_api.h"
#include "glite/wms/wmproxyapi/wmproxy_api_utilities.h"

// wmp-client utilities
#include "utilities/utils.h"
#include "utilities/options_utils.h"

using namespace std ;
using namespace glite::wms::client::utilities ;
using namespace glite::wms::wmproxyapi;
using namespace glite::wms::wmproxyapiutils;

namespace glite {
namespace wms{
namespace client {
namespace services {

const string monthStr[]  = {"Jan", "Feb", "March", "Apr", "May", "June", "July", "Aug", "Sept", "Oct", "Nov", "Dec"};
bool allOpt = false ;

/*
*	Default constructor
*/
DelegateProxy::DelegateProxy( ){ };
/*
*	Default destructor
*/
DelegateProxy::~DelegateProxy( ){ };
/*
* Handles the command line arguments
*/
void DelegateProxy::readOptions (int argc,char **argv){
        Job::readOptions  (argc, argv, Options::JOBDELEGATION);
	allOpt = wmcOpts ->getBoolAttribute( Options::ALL ) ;

};
/*
* Performs the main operations
*/
void DelegateProxy::delegation ( ){
	ostringstream out ;
	string proxy = "" ;
	string endpoint ;
	vector<string> endpoints ;
	string delegationId = getDelegationId();
	// if option --all is specifed, delegates on all the Endpoints
	if ( allOpt ) {
		endpoints = wmcUtils->getWmps();
		for ( int i = 0; i < (int)endpoints.size() ; i++ ) {
			delegateProxyEndpoint(endpoints[i] ) ;
		}
	} else {
		// Endpoint
		endpoint = delegateProxy( );
	}
	// output message
        // OUTPUT MESSAGE ============================================
	out << "\n" << wmcUtils->getStripe(74, "=" , string (wmcOpts->getApplicationName() + " Success") ) << "\n\n";
	out << "Your proxy has been successfully delegated to the WMProxy(s):\n" ;
	if ( allOpt ) {
		for ( int i = 0; i < (int)endpoints.size() ; i++ ) {
			out << endpoints[i] << "\n";
		}
	} else {
		out << endpoint << "\n";
	}
	if (delegationId==""){out << "delegation identifier was automatically generated"<<"\n";}
	else{ out << "with the delegation identifier: " << getDelegationId( )      <<"\n";}
	out << infoToFile( ) ;
	out << "\n" << wmcUtils->getStripe(74, "=") << "\n\n";
	out << getLogFileMsg ( ) << "\n";
        // ==============================================================
        // prints the message on the standard output
	cout << out.str() ;
};


std::string DelegateProxy::infoToFile( ){
	string rm = "";
	if (!m_outOpt.empty()){
		string delegationId=getDelegationId();
		char ws = (char)32;
		time_t now = time(NULL);
		struct tm *ns = localtime(&now);
		// date
		ostringstream date;
		date << Utils::twoDigits(ns->tm_mday) << ws << monthStr[ns->tm_mon]  << ws <<  (ns->tm_year+1900) << "," << ws;
		date << Utils::twoDigits(ns->tm_hour) << ":" << Utils::twoDigits(ns->tm_min) << ":" << Utils::twoDigits(ns->tm_sec) << ws;
		string msg = wmcOpts->getApplicationName( ) +" (" + date.str() + ")\n";
		msg += "=========================================================================\n";
		if (delegationId!=""){msg += "WMProxy: " + getEndPoint( )+ "\ndelegation ID: " +  getDelegationId( ) + "\n";}
		else{msg += "WMProxy: " + getEndPoint( )+ "\ndelegation ID was automatically generated"              + "\n";}
		if( wmcUtils->saveToFile(m_outOpt, msg) < 0 ){
			logInfo->print (WMS_WARNING, "unable to write the delegation operation result " , Utils::getAbsolutePath(m_outOpt));
		} else {
			logInfo->print (WMS_DEBUG, "The DelegateProxy result has been saved in the output file ",
				Utils::getAbsolutePath(m_outOpt));
			rm += "\nThe DelegateProxy result  has been saved in the following file:\n";
			rm += Utils::getAbsolutePath(m_outOpt) + "\n";
		}
	}
        return rm;
}
}}}} // ending namespaces
