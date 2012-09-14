#ifndef  ORG_GLITE_WMSUI_WRAPY_NSWRAPPER_H
#define ORG_GLITE_WMSUI_WRAPY_NSWRAPPER_H
/*
 * NsWrapper.h
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 *
 */
#include "glite/wms/ns-client/NSClient.h"
#include "glite/wmsutils/exception/Exception.h"
#include "glite/wms/common/logger/common.h"
#include "glite/lb/JobStatus.h"
#include <vector>


/*********************
* VOMS includes:
*********************/
#include "glite/security/voms/voms_api.h"
// #include "glite/wmsutils/thirdparty/globus_ssl_utils/sslutils.h"
/**
 * Provide a wrapper for the glite::wms::manager::ns::client:NSClient class
 * it allows the user to submit, cancel, and retrieve output files from a job (as well as for a dag)
 *
 *
 * @brief Network Server client methods wrapper
 * @version 0.1
 * @date 15 April 2002
 * @author Alessandro Maraschini <alessandro.maraschini@datamat.it>
*/
class NS {
public:
	/** NS wrapper constructor
	*/
	NS();
	/** NS wrapper destructor
	*/
	~NS();
	/** Initialise the NS wrapper intance with the server parameters
	* @param host the Network Server host name
	* @param port the Network server listening port
	* @param logLevel the debugging verbosity level (0=No debug, 6= max verbosity)
	*/
	void ns_init (const std::string& host , int port, int logLevel=0 )  ;
	/** Submit a job to the network server
	* @param jdl  the JobAd jdl string representation
	* @param DAG for submitting dag must be !=0
	*/
	void ns_submit (const std::string& jdl , int DAG = 0)  ;
	/** Kill a job which has not ended yet
	* @param jobid the job identifier referring to the new job
	*/
	int ns_cancel (const std::string& jobid )  ;

	/** 
	* this method has been DEPRECATED
	*/
	std::string ns_get_root(const std::string& jobid)  ;
	/** After the output files of a job have been successfully retrieved, the purge method is needed to clean up the NS stored files owned bt the job
	*@param jobid the job identifier referring to the new job
	*/
	void ns_purge(const std::string& jobid)  ;

	/**
	* this Method has been DEPRECATED
	*/
	std::string toDir( const std::string& url) ;
	/** Retrieve all the attributes that are multi valued i.e. could be used in a member-Ismember expression
	* @return a vector containing all the multi-valued attributes */
	std::vector <std::string> ns_multi()  ;

	/** Retrieve the list of all the paths pointing to the job output files
	* @param jobid the job identifier referring to the new job
	*/
	std::vector <std::string> ns_output_sandbox(const std::string& jobid)  ;

	/** Retrieve all the Computing Elements that match with the Job rank and requirements
	* @param jdl the JobAd string representation for the job
	*/
	std::vector <std::string> ns_match (const std::string& jdl)  ;
	/**
	* Release any initialised memory for NS
	*/
	void ns_free () ;
	/**Set an attribute value (of string type)
	* @param err the string description passed by reference. Python will consider it as a returning parameter
	* @return a couple of [ int , string ] representing the error code (0 for success) and the error string representation
	*/
	std::vector<std::string> get_error () ;
	/** Create a job unique identificator in the form <lb host>:<lb port>/<unique string>
	* @param host the LB host name
	* @param port the LB port name
	* @return the Job unique string identification
	*/
	std::string create_job_id(const std::string& host , int port) ;
	/** Register a job to the HLR server for accounting
	* @param jobid the job identifier referring to the new job
	* @param hlr the HLRlocation JobAd value as specified in the job's jdl
	*/
	std::string dgas_jobAuth(const std::string& jobid ,const std::string& hlr ) ;
private :
	glite::wms::manager::ns::client::NSClient * ns ;
	void log_error ( const std::string& ) ;
	std::string error ;
	bool error_code ;
} ;

/**
 * Provide a wrapper that manages user cerificate general information
 * it allows to check the proxy cerfticate's parameters such us validity, retrieve issuer etc etc.
 * it also allows to manipulate VirtualOrganisation certificate properties, retreving, if present, VO default names and groups
 *
 * @brief LB Event wrapper class
 * @version 0.1
 * @date 15 April 2002
 * @author Alessandro Maraschini <alessandro.maraschini@datamat.it>
*/
class  UserCredential {
	public:
		/** Constructor */
		UserCredential ( const std::string& proxy_file) ;
		/**User Credential default Destructor
		*/
		~UserCredential();
		/** Retrieve the default Virtual Organisation name
		* @return the default VO
		*/
		std::string  getDefaultVoName ();
		/** Retrieve the default FQAN for the current certificate
		* @return the default FQAN
		*/
		std::string  getDefaultFQAN ();
		/** Retrieve the Certificate Issuer*/
		std::string  getIssuer() ;
		/** Retrieve the last allowed date*/
		int getExpiration() ;
		/** Retrieve the vector of all  the Virtual Organisation names */
		std::vector <std::string> getVoNames ();
		/***Retrieve all groups belonging to the specified VirtualOrganisation
		* @param voName the name of the Virtual Organisation where to retrieve groups */
		std::vector <std::string> getGroups ( const std::string& voName ) ;
		/** Returns the groups belonging to the default VirtualOrganisation*/
		std::vector <std::string > getDefaultGroups () ;
		/** Check wheater the specifie Virtual Organisation is contained in the Vo certificate extension*/
		bool containsVo ( const std::string& voName )  ;
		/**Set an attribute value (of string type)
		* @param err the string description passed by reference. Python will consider it as a returning parameter
		* @return a couple of [ int , string ] representing the error code (0 for success) and the error string representation
		*/
		std::string get_error () ;
	private:
		int vo_data_error , timeleft ;
		std::string   proxy_file ; 
		int load_voms ( vomsdata& d ) ;
};

#endif

