#ifndef  ORG_GLITE_WMSUI_WRAPY_LOGWRAPPER_H
#define ORG_GLITE_WMSUI_WRAPY_LOGWRAPPER_H
/*
 * LogWrapper.h
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 */
#include <vector>
#include <list>
#include <string>
#include "glite/lb/producer.h"
#define GLITE_LB_LOG_DESTINATION "EDG_WL_LOG_DESTINATION"
/**
 * Provide a wrapper for the locallogger client methods
 * it allows the user to log all the information the job needs at submission time both for simple jobs and for dags
 *
 * @brief  locallogger clients methods wrapper
 * @version 0.1
 * @date 15 April 2002
 * @author Alessandro Maraschini <alessandro.maraschini@datamat.it>
*/
class LOG {
   public:
	/**
	* Log class constructor
	*/
	LOG() ;
	/**
	* Log class destructor
	*/
	~LOG() ;
	/**
	* Initialise logging context. Must be  called before any other operation just after the constructor
	* @param locallogger server address in the form <host name> [:<port number>] if no port is specified, deafult port 9002 will be used
	* the value is nevetheless overriden by GLITE_WMS_LOG_DESTINATION env variable, if present
	*/
	void init ( const std::string& locallogger ) ;
	/**
	* Register a job to the LB server through a synchronous call
	* @param jobid the job unique identifier
	* @jdl the JobAd string representation for the job
	* @ns the Network Server <host>:<port> string representation
	*/
	void regist( const std::string& jobid , const std::string& jdl , const std::string& ns ) ;
	/**
	* Log the synch event (for Checkpointable jobs)
	* @param state the jobState checkpointable event string representation to be logged (it itself contain the jobid)
	*/
	void logSync (const  std::string& state, const std::string& currentStep="1") ;
	/**
	* Once the job has been successfully registered, before starting submitting the  START async method has to be called
	* @param host the Network Server host name
	* @param port the Network server listening port
	* @param jdl the JobAd string representation for the job
	*/
	void log_start (const std::string& host  , int port , const std::string& jdl ) ;
	/**
	* Log the usertag event (when usertag attribute is specified for the job) this method create a special usertag event for the job
	* @param attrName the name of the user tag to be logged
	* @param attrValue the value of the user tag to be logged
	*/
	void log_tag (const std::string& attrName  , const std::string& attrValue ) ;
	/**
	*  When logging user tags for multiple jobs (i.e. during a DAG submission) this method must be called when referring to a job different for the registered one
	* @param jobid the job identifier referring to the new job
	*/
	void log_jobid(const std::string& jobid ) ;
	/**
	* Finally release the initialized context
	*/
	void free () ;
	/**
	* Retrieve the LB sequence code needed to perform submission
	* @return the string representation of the sequence code: this value must be add to the JobAd Jdl instance
	*/
	std::string getSequence();
	/**
	* Log the synch event (for Checkpointable jobs)
	* @param state the jobState checkpointable event string representation to be logged (it itself contain the jobid)
	*/
	void log_listener( const std::string& jobid ,const std::string& host , int port  ) ;
	/**
	* Once the job has been successfully submitted, the log Transfer OK async method has to be called.
	* @param host the Network Server host name
	* @param port the Network server listening port
	* @param jdl the JobAd string representation for the job
	*/
	void log_tr_ok  ( const std::string& jdl , const std::string&  host , int port ) ;
	/**
	* If the job submission, for any reason failed, then the log Transfer FAILED async method has to be called.
	* @param host the Network Server host name
	* @param port the Network server listening port
	* @param jdl the JobAd string representation for the job
	* @param exc a string representation of the NS submission error
	*/
	void log_tr_fail ( const std::string& jdl , const std::string&  host , int port , const char* exc) ;
	/**
	* Query the logging server for Job State value retrieval (for Checkpointable jobs)
	* @param jobid the job unique identifier
	* @param step the checkpointable step to be retrieved (0 means last logged step, 1 meadn last but one, etc etc...)
	*/
	std::string retrieveState ( const std::string& jobid , int step = 0) ;
	/**Set an attribute value (of string type)
	* @param err the string description passed by reference. Python will consider it as a returning parameter
	* @return a couple of [ int , string ] representing the error code (0 for success) and the error string representation
	*/
	std::vector<std::string>  get_error () ;
	/**
	* Register a dag to the LB server through a synchronous call. perform Sub jobs jdl submission string registering as well
	* @param jdls the sub-jobs string representation
	* @param  jobid the Dag unique identifier
	* @param jdl the DagAd string representation (No nodes info displayed, partial jdl)
	* @param length the number of sub jobs (wich MUST match the size of the jdls vector)
	* @param ns the Network server string representation <host >:<port>
	*/
	std::vector<std::string>  regist_dag ( const std::vector<std::string>& jdls, const std::string& jobid ,const std::string& jdl , int length , const std::string& ns, int regType);
	std::vector<std::string>  generate_sub_jobs( const std::string& jobid, int subjobs);
   private:
	// void log_error ( const std::string& err ,  edg_wll_Context *ctx=NULL ) ;
	void log_error ( const std::string& err ) ;
	edg_wll_Context ctx ;
	std::string error ;
	int error_code ;
};
#endif
