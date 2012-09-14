/***
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
* 	Authors:	Alessandro Maraschini <alessandro.maraschini@datamat.it>
* 			Marco Sottilaro <marco.sottilaro@datamat.it>
*
*/

// 	$Id$

#ifndef GLITE_WMS_CLIENT_SERVICES_JOBPERUSAL_H
#define GLITE_WMS_CLIENT_SERVICES_JOBPERUSAL_H

// inheritance
#include "job.h"
#include "lbapi.h"
// utilities
#include "utilities/options_utils.h"
// WMProxy API's
#include "glite/wms/wmproxyapi/wmproxy_api.h"

namespace glite {
namespace wms{
namespace client {
namespace services {

/***
* Type of  Operations
*/
enum perusalOperations {
	PERUSAL_NONE,
	PERUSAL_GET,
	PERUSAL_SET,
	PERUSAL_UNSET
};

class JobPerusal  : public Job {

	public :
        	/**
                *	Default constructor
                */
		JobPerusal ( );
                 /**
                * Default destructror
                */
                ~JobPerusal();
		/**
		*	Reads the command-line user arguments and sets all the class attributes
		*	@param argc number of the input arguments
		*	@param argv string of the input arguments
		*/
               void readOptions ( int argc,char **argv)   ;
		/**
                *	Performs the main operations
                */
		void jobPerusal ( ) ;

        private :
		/**
		* Performs the perusal-get operation: retrieves a list of DestinationURI's of
		* the peek files available on the server and downloads the files to the local machine
		* @param paths gets the list of local pathnames to the downloaded files
		*/
		void perusalGet (std::vector <std::string> &paths) ;
		/**
		* Performs the perusal-set operation: enables the job-peeking for one or
		* more files
		*/
		void perusalSet ( ) ;
		/**
		* Performs the perusal-unset operation: disabled the job-peeking for all
		* the files previously set
		*/
		void perusalUnset ( );
		/**
		* Retireves the status of the job in order to check whether its status allows
		* retrieving of its peek files
		*/
		void checkStatus( );
		/**
		* Performs downloading of the peeks files to the local machine from
		* the Destination URI's provided as input; it gets back a list of pathnames
		* of the downloaded files
		* @param uris the list of Destination URi's of the files to be retrieved
		* @param paths gets the list of local pathnames to the downloaded files
		* @param errors string with information on errors occurred during the downloading operations
		*/
		void gsiFtpGetFiles (std::vector <std::string> &uris, std::vector<std::string> &paths, std::string &errors) ;
				/**
		* Performs downloading of the peeks files to the local machine from
		* the Destination URI's provided as input with htcp;
		* it gets back a list of pathnames of the downloaded files
		* @param uris the list of Destination URi's of the files to be retrieved
		* @param paths gets the list of local pathnames to the downloaded files
		* @param errors string with information on errors occurred during the downloading operations
		*/
		void htcpGetFiles (std::vector <std::string> &uris, std::vector<std::string> &paths, std::string &errors) ;
		/**
		* Prints out information on the operation results on the standard output
		* @param operation type of operation (set, get or unset)
		* @param paths the list of local pathnames to the downloaded files
		*/
		void printResult(const perusalOperations &operation, std::vector<std::string> &paths);
        	 /**
                *	String input arguments
                */
		std::string m_inOpt ; 		// --input <file>
		std::string m_inFileOpt ; 		// --input-file <file>
		std::string m_outOpt ; 	// --ouput <path>
		std::string m_dirOpt ; 		// --dir <path>
		 /**
                *	Boolean input arguments
                */
		bool getOpt ;
		bool setOpt ;
		bool unsetOpt ;
		bool allOpt ;
                bool nodisplayOpt ;
		bool nointOpt;
		/***
		* JobId
		*/
		std::string jobId ;
                /***
                * List of the files provided as input
                */
                std::vector<std::string> peekFiles ;

  };
}}}} // ending namespaces

#endif //GLITE_WMS_CLIENT_SERVICES_JOBPERUSAL_H

