/**
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
* 	Authors:	Alessandro Maraschini <alessandro.maraschini@datamat.it>
* 			Marco Sottilaro <marco.sottilaro@datamat.it>
*/

// 	$Id$

#ifndef GLITE_WMS_CLIENT_SERVICES_JOB_H
#define GLITE_WMS_CLIENT_SERVICES_JOB_H

// options utilities
#include "utilities/options_utils.h"
#include "utilities/utils.h"
#include "utilities/logman.h"
// wmproxy API
#include "glite/wms/wmproxyapi/wmproxy_api.h"

namespace glite {
namespace wms{
namespace client {
namespace services {

#define WMP_NS1_GETPROXY_SERVICE        "ns1__getProxyReq"
#define WMP_NS1_PUTPROXY_SERVICE        "ns1__putProxyReq"
#define WMP_NS2_GETPROXY_SERVICE        "ns2__getProxyReq"
#define WMP_NS2_PUTPROXY_SERVICE        "ns2__putProxyReq"
#define WMP_VERSION_SERVICE             "getVersion"
#define WMP_REGISTER_SERVICE            "jobRegister"
#define WMP_START_SERVICE               "jobStart"
#define WMP_SUBMIT_SERVICE              "jobSubmit"
#define WMP_CANCEL_SERVICE              "jobCancel"
#define WMP_OUTPUT_SERVICE              "getOutputFileList"
#define WMP_PURGE_SERVICE               "jobPurgeQuota"
#define WMP_LISTMATCH_SERVICE           "jobListMatch"
#define WMP_SETPERUSAL_SERVICE          "enableFilePerusal"
#define WMP_GETPERUSAL_SERVICE          "getPerusalFiles"
#define WMP_MAXISBSIZE_SERVICE          "getMaxInputSandboxSize"
#define WMP_FREEQUOTA_SERVICE           "getFreeQuota"
#define WMP_SBDESTURI_SERVICE           "getSandboxDestURI"
#define WMP_BULKDESTURI_SERVICE         "getSandboxBulkDestURI"
#define WMP_GETPROTOCOLS_SERVICE           "getTransferProtocols"
#define WMP_JDL_SERVICE                 		"getJDL"
#define WMP_DELEG_PROXYINFO    		"getDelegatedProxyInfo"
#define WMP_JOB_PROXYINFO    		"getJobProxyInfo"
/*
* WMProxy version numbers
* (major.minor.subminor)
*/
struct wmpVersionType {
	int major ;
	int minor ;
	int subminor ;
};


class Job{
	public :
		/**
		* Default constructor
		*/
		Job();
		/*
		* Default destructror
		*/
		virtual ~Job();
		/*
		*	reads the command-line user arguments and sets all the class attributes
		*	@param argc number of the input arguments
		*	@param argv string of the input arguments
                *	@return a string with the list of the specified options
		*/
		virtual void readOptions (int argc,char **argv,
			glite::wms::client::utilities::Options::WMPCommands);
                /*
		*	prints the error messages for an exception
		*	@param header header text
		* 	@param exc the exception
		* 	@param exename the executable name
		*/
		virtual void excMsg(const std::string &header, glite::wmsutils::exception::Exception &exc, const std::string &exename="");
		/*
		*  Returns a string with the information on the log filename
		* @return the string containg the information
		*/
		virtual std::string getLogFileMsg ( ) ;
		/**
		* Checks the user input options related to the credential delegation
		* (--autodelegation and --delegation-Id ).
		* The delegationId string is automatically generated If autodelegation has been requested
		* An exception is thrown if both optionshas not been specified.
		*/
		virtual void setDelegationId ( );
		/**
		* Returns the delegationId String to be used for the operation that is being performed
		* (either the string specified by user option or autogenerated in case of automatic delegation)
		* @returns the string with the delegation identifier
		*/
		virtual const std::string getDelegationId ( );
		/**
		* Performs credential delegation to endpointURL established checking
		* the following objects in this order:
		*	> the --enpoint user option;
		*	> the GLITE_WMS_WMPROXY_ENDPOINT environment variable
		*	> the list of URLs specified in the configuration file (In this last case the choice is randomically performed)
		*/
		virtual const std::string delegateProxy( );
		/**
		* Sets the endpoint URL where the operation will be performed. The URL is established checking
		* the following objects in this order:
		*	> the --enpoint user option;
		*	> the GLITE_WMS_WMPROXY_ENDPOINT environment variable
		*	> the list of URLs specified in the configuration file (In this last case the choice is randomically performed)
		* If the input parameter is TRUE the delegationId string is set (reading the string specified with the
		* --delegationId option if specified, otherwise autogeneration is perfomed in case of --autodelegation;
		* an exception is thrown if both optionshas not been specified).
		* @param delegation if TRUE delegationId string is set
		*/
		virtual void retrieveEndPointURL (const bool &delegation=true);
		/**
		* Sets the endpoint URL where the operation will be performed.
		* @param endpoint the string with the endpoint URL
		* @param check if TRUE the version numbers are retrieved and the credemtial delegation options are checked
		*
		*/
		virtual void setEndPoint (const std::string &endpoint, const bool check=false);
		/**
		* Returns the URL of the endpoint where the operation is being performed
		*/
		virtual const std::string getEndPoint ();
		/**
		* Returns the object with the configuration context (proxy file, endpoint URL and trusted certifcates location)
		*/
		virtual glite::wms::wmproxyapi::ConfigContext* getContext( );
		/**
		* Returns the path location to the user proxy file
		* @returns the string with the filepath
		*/
		virtual const char* getProxyPath ( );
		/**
		* Returns the path location to the directory containing the trusted Certification Authorities
		* @returns the string with the filepath
		*/
		virtual const char* getCertsPath ( );
		/**
		* Checks if the getTransferProtocols (and getJDL) service (s) are
		* available on the WMProxy server
		* (according to the WMProxy version)
		*/
		virtual bool checkVersionForTransferProtocols ( );
		/**
		* Checks if the WMProxy server supports the File Transfer Protocol
		* specified with the --proto option.
		* If this option has not been specified, this method checks which protocols
		* that are available for this client are supported by the server.
		* In case this check is not possible, the client default protocol will be used
		* (see the utilities::Option class).
 		* The WMProxy getTransferProtocols service returns the list of available protocols
		* on the server (since the WMProxy version 2.1.1)
		*/
		void checkFileTransferProtocol( ) ;
	private:
		/**
		* Retrieves the version of one or more WMProxy services.
		* A set of WMProxy to be contacted can be specified as input by the 'urls'  vector.
		* If no urls is specified as input, the following objects are checked to retrieve to service to be contacted:
		* the 'endPoint' private attribute of this class; the specific user environment variable ; the specific list in the configuration file.
		* An exception is throw if either no endpoint is specified or
		* all specified endpoints don't allow performing the requested operation.
		* @param urls the list of the endpoint that can be contacted; at the end of the execution this parameter
		* will contain a list of the endpoints that are not be contacted
		* @param endpoint the url of the contacted endpoint
		* @param version the version number of the contacted endpoint
		* @param all if TRUE, it contacts all endpoints specified
		*/
		void checkWmpList (std::vector<std::string> &urls, std::string &endpoint, std::string &version, const bool &all=false) ;
		/*
		* Retrieves the string with WMProxy version information
		* @return the string with the version information having the format x.y.z
		*/
		virtual const std::string getWmpVersion (const std::string &endpoint) ;
		/**
		* Sets the attributes of this class related to the version neumbers of the WMProxy server
		* @param version the version string got calling the getVersion service of the WMProxy server
		*/
		void setVersionNumbers(const std::string& version) ;
		/**
		* Performs user credential delegation to the input enpoint
		* @param endpoint the enpoint URL
		*/
       		 void delegateUserProxy(const std::string &endpoint);
		/**
		* Sets the attribute of this class related to the path location of the user proxy file
		*/
		virtual void setProxyPath ( );
		 /**
		* Sets the attribute of this class related to the
		* path location of the directory containing the trusted Certification Authorities
		*/
		virtual void setCertsPath ( );
		 /**
        	* Gets the version message
        	*/
    		virtual void printClientVersion( );
		/**
		* Contacts the endpoint to retrieve the version
		*/
		virtual void printServerVersion( );
	protected:
		/** Common post-options Checks (proxy time left..)*/
		void postOptionchecks(unsigned int proxyMinTime=0);
		/** Print a particular set (expressed with title) of warning messages (expressed with warnings)*/
		void printWarnings   (const std::string& title, const std::vector<std::string> &warnings);
		/** Input arguments */
		std::string* logOpt ;	// --logfile <file>
		std::string* outOpt ;	// --output <file>
		std::string* cfgOpt ; 	// --config <file>
		std::string* voOpt ;	// --vo <VO_Name>
		std::string* dgOpt ;	// --delegationid
		std::string* fileProto; 	// --proto
		bool autodgOpt ;		// --autm-delegation,
		bool nointOpt ;		// --noint
		bool dbgOpt ;		// --debug
		/** handles the input options*/
		glite::wms::client::utilities::Options *wmcOpts ;
		/** utilities object */
		glite::wms::client::utilities::Utils::Utils *wmcUtils ;
		/** log file */
		glite::wms::client::utilities::Log *logInfo ;
		/** endpoint*/
		std::string* endPoint ;

		/** Configuration contex */
		glite::wms::wmproxyapi::ConfigContext *cfgCxt ;

		private :
		/*
		* Version numbers of the server
		*/
		wmpVersionType wmpVersion;
		/**
		* Path to the local user proxy
		*/
		std::string* proxyFile ;
		/**
		* Path to the CertAuth's directory
		*/
		std::string* trustedCerts ;
};
}}}} // ending namespaces
#endif //GLITE_WMS_CLIENT_SERVICES_JOB_H

