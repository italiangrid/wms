// Copyright (c) Members of the EGEE Collaboration. 2004. 
// See http://www.eu-egee.org/partners/ for details on the copyright
// holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 

//     http://www.apache.org/licenses/LICENSE-2.0 

// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

#ifndef  GLITE_WMS_WMPROXYAPICPP_H
#define GLITE_WMS_WMPROXYAPICPP_H
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/**
* \file wmproxy_api.h
* \brief wsdl wmproxy service wrapper
* A wrapper around wmproxy Web Service. It provides primitive or simple structure to access more complicated service methods
*/
/** \mainpage Workload Manager Proxy API C++
*
* \version 3.1
* \section intro_sec Description
* <p>
* This User Interface API supplies the client applications with a set of  functions providing an easy access to the WMProxy Web Services.<BR>
* The users are allowed:
* <UL>
*
*
* <LI>\link glite::wms::wmproxyapi::getProxyReq delegating the credential \endlink
* <LI>\link glite::wms::wmproxyapi::jobRegister registering\endlink and \link glite::wms::wmproxyapi::jobSubmit submitting\endlink jobs
* <LI>\link glite::wms::wmproxyapi::jobCancel cancelling the job  \endlink during its life-cycle
* <LI>retrieving information on the location \link glite::wms::wmproxyapi::getSandboxDestURI where the job input sandbox files can be stored \endlink
* <LI> retrieving the \link glite::wms::wmproxyapi::getOutputFileList output sandbox files list \endlink
* <LI> retrieving a list of \link glite::wms::wmproxyapi::jobListMatch Computer Elements matching the specified JDL\endlink
* <LI> getting JDL templates for \link glite::wms::wmproxyapi::getJobTemplate jobs\endlink,  \link glite::wms::wmproxyapi::getDAGTemplate Dags\endlink, \link glite::wms::wmproxyapi::getCollectionTemplate collections\endlink, etc
* <LI> getting information on the \link glite::wms::wmproxyapi::getFreeQuota user disk quota on the server \endlink
* <LI> retrieving information on the \link glite::wms::wmproxyapi::getJDL JDL submitted to WMProxy or registered to LB \endlink
* </UL>
* </p>
* <p>
* Job requirements are expressed by Job Description Language (JDL).
* The types of jobs supported by the WM service are:
* <UL>
* <LI>Normal - a simple application
* <LI>DAG - a direct acyclic graph of dependent jobs
* <LI>Collection - a set of independent jobs
* <LI>Parametric - jobs with JDL's containing some parameters
* </UL>
*  The API is divided into two groups of functions:
*<UL>
* <LI><B>glite::wms::wmproxyapi</B> :  with the functions that allow calling the server and handle possible fault exceptions;
* <LI><B>glite::wms::wmproxyapiutils</B>: with some utility functions that allow handling the X509 user proxy files needed by some functions in wmproxyapi.
*</UL>
* */

namespace glite {
namespace wms {
namespace wmproxyapi {



/**@name Exceptions*/

/**  Base exception wrap */
struct BaseException {
	/** Empty Constructor */
	BaseException():ErrorCode(NULL),Description(NULL),FaultCause(NULL){};
	/** The name of the method throwing the exception */
	std::string                          methodName  ;
	/** Element Timestamp of type xs:dateTime */
	time_t                               Timestamp     ;
	 /** Element ErrorCode of type xs:string*/
	std::string*                         ErrorCode   ;
	/** Element Description of type xs:string*/
	std::string*                         Description ;
	/** Vector of std::string with length 0..unbounded*/
	std::vector<std::string> *FaultCause   ;
};
/**
* Creates a BaseException object
* @param b_ex pointer to an exception object
* @param method the name of the method throwing the exception
* @param description the description of the fault
* @return a pointer to the object created
* @see for the <I>BaseException *b_ex</I> input parameter:
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException
* @see GetQuotaManagementException, NoSuitableResourcesException, JobUnknownException
* @see OperationNotAllowedException, OperationNotAllowedException, ProxyFileException, GenericException
*/
BaseException* createWmpException (BaseException *b_ex ,const std::string &method ,  const std::string &description );

/** Generic Authentication problem.*/
struct AuthenticationException:BaseException{};
/** Client is not authorized to perform the required operation.*/
struct AuthorizationException:BaseException{};
/** One or more of the given input parameters is not valid.*/
struct InvalidArgumentException:BaseException{};
/** Quota management is not active on the WM. */
struct GetQuotaManagementException:BaseException{};
/** No resources matching job requirements have been found. */
struct NoSuitableResourcesException:BaseException{};
/** The provided job has not been registered to the system.*/
struct JobUnknownException:BaseException{};
/** Current job status does not allow requested operation.*/
struct OperationNotAllowedException:BaseException{};
/** Server is too busy*/
struct ServerOverloadedException:BaseException{};
/** Proxy file errors */
struct ProxyFileException:BaseException{};
/** Error during delegation operations with Gridsite methods (grstXXXX)  - since 1.2.0*/
struct GrstDelegationException:BaseException{};
/** Generic problem. More details in the message*/
struct GenericException:BaseException{};

/**@name Enumeration*/

/** Used to specify the JdlType, whether is original or registered
 @see #getJDL
*/
enum JdlType {
  /** The original JDL submitted by the user to the WMPROXY service*/
  ORIGINAL,
  /** The JDL registered to the LB server by WMProxy service
  * In some cases (collections, partitionables, parametrics)
  * it is heavily different from the orinal submitted one
  **/
  REGISTERED
};
/** Used to specify the jobtype. multiple jobtype can be specified togheter by the bitwise (|) or operation */
enum jobtype {
	/**Normal Jobs */
	JOBTYPE_NORMAL=1,
	/** Parametric Jobs */
	JOBTYPE_PARAMETRIC =2,
	/**  Interactive Jobs */
	JOBTYPE_INTERACTIVE=4,
	/** Mpi Jobs */
	JOBTYPE_MPICH=8,
	/** PArtitionable Jobs */
	JOBTYPE_PARTITIONABLE=16,
	/** Checkpointable Jobs */
	JOBTYPE_CHECKPOINTABLE =32
};


/**
* Used to define the the structure of a dag
* the name of the node might be NULL for the first instance (if it's representing the dag itself) while all the children have to be properly initialised. */
struct NodeStruct {
	/** The name of the node (might be NULL for the first instance) */
	std::string* nodeName ;
	/** A list of all the children for this node*/
	std::vector< NodeStruct* > childrenNodes ;
};
/**
* Used to define the jobid hierarchy of a job or a dag. the nodeName is mandatory only when the jobid refers to a dag node
*/
struct JobIdApi {
	/** The identifier of the job */
	std::string jobid ;
	/** The name of the node (for DAG, collection of jobs and parametric jobs, NULL otherwise) */
	std::string* nodeName ;
	/** The server ISB relative path */
        std::string* jobPath ;
	/** A list of all the children for this node (for DAG, collection of jobs and parametric jobs, empty-vector otherwise) */
	std::vector< JobIdApi* > children ;
};

/**
* Used as a member of ProxyInfoStructType
* @see #ProxyInfoStructType
*/
struct VOProxyInfoStructType {
    std::string user ;
    std::string userCA;
    std::string server;
    std::string serverCA;
    std::string voName;
    std::string URI;
    std::string startTime;
    std::string endTime ;
    std::vector<std::string> attribute;
};

/**
* Used for describing all proxy parameters
* @see #getDelegatedProxyInfo
* @see #getJobProxyInfo
*/
struct ProxyInfoStructType {
    std::string                          subject;
    std::string                          issuer;
    std::string                          identity;
    std::string                          type;
    std::string                          strength;
    std::string                          startTime;
    std::string                          endTime;
    std::vector<VOProxyInfoStructType*> vosInfo ;
};


struct ProxyReqStruct {
    std::string 	proxy;
    std::string	delegationId;
};


struct JobStatusStructType {
    std::string                          jobid;
    std::string                          status;
    std::vector<JobStatusStructType*> childrenJob;
};
/**
* Used to configure non-default properties such as:
* <UL>
* <LI>a proxy file different from the default one ;</LI>
* <LI>an endpoint service different from the one specified in the wsdl ;</LI>
* <LI>a further directory where to look for CA certificates.</LI>
* </UL>
*/
struct ConfigContext{
	/** Default constructor */
	ConfigContext( std::string p , std::string s, std::string t);
	/** Default destructor */
	virtual ~ConfigContext() throw();
	/** User proxy file (default value is: <I>/tmp/x509_u(uid)</I> ) */
	std::string proxy_file;
	/** Endpoint URL (default value in the wsdl file)*/
	std::string endpoint;
	/** Trusted certificates location (default value is: <I>/etc/grid-security/certificates</I>) */
	std::string trusted_cert_dir;
	/** SOAP Timeout */
	int soap_timeout;
	/** server authentication flag */
	bool server_authentication ;
};

/**
* Checks if CA verification has been disabled by user
*/
void setServerAuthentication ( ConfigContext *cfs, bool auth );

/**@name WMProxy Submission services*/

/**
*   Registers a job for submission.The unique identifier assigned to the job is returned to the client.
*  This operation only registers the job and assigns it with an identifier.
*  The processing of the job (matchmaking, scheduling etc.) within the WM is not started.
* The job is "held" by the system in the "Registered" status until the jobStart operation is called.
* Between the two calls the client can perform all preparatory actions needed by the job to run
* (e.g. the registration of input data, the upload of the job input sandbox files etc); especially
* those actions requiring the specification of the job identifier.
* The service supports registration (and consequently submission) of simple jobs, parametric jobs, partitionable jobs, DAGs and collections of jobs.
* The description is always provided through a single JDL description.
* When a clients requests for registration of a complex object, i.e. parametric and partitionable jobs, DAGs and collections of jobs (all those requests represent in fact a set of jobs);
*  the operation returns a structure containing the main identifier of the complex object and the identifiers of all related sub jobs.
* @param jdl The JDL string representation of the job
* @param delegationId The id string used to identify a previously delegated proxy
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ; if NULL default parameters are used
* @return The structure associated to the registered job, with its jobid(s)
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException If the given JDL expression is not valid
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see #getProxyReq, #putProxy (proxy delegation)
* @see thrown exceptions: AuthenticationException, AuthorizationException, InvalidArgumentException. GenericException, BaseException
*/
JobIdApi jobRegister (const std::string &jdl, const std::string &delegationId, ConfigContext *cfs=NULL);


/**
*   Registers a job for submission.The unique identifier assigned to the job is returned to the client.
*  This operation only registers the job and assigns it with an identifier.
*  The processing of the job (matchmaking, scheduling etc.) within the WM is not started.
* The job is "held" by the system in the "Registered" status until the jobStart operation is called.
* Between the two calls the client can perform all preparatory actions needed by the job to run
* (e.g. the registration of input data, the upload of the job input sandbox files etc); especially
* those actions requiring the specification of the job identifier.
* The service supports registration (and consequently submission) of simple jobs, parametric jobs, partitionable jobs, DAGs and collections of jobs.
* The description is always provided through a single JDL description.
* When a clients requests for registration of a complex object, i.e. parametric and partitionable jobs, DAGs and collections of jobs (all those requests represent in fact a set of jobs);
*  the operation returns a structure containing the main identifier of the complex object and the identifiers of all related sub jobs.
* @param jsdl The JSDL file stream that represents the job
* @param delegationId The id string used to identify a previously delegated proxy
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ; if NULL default parameters are used
* @return The structure associated to the registered job, with its jobid(s)
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException If the given JDL expression is not valid
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see #getProxyReq, #putProxy (proxy delegation)
* @see thrown exceptions: AuthenticationException, AuthorizationException, InvalidArgumentException. GenericException, BaseException
*/
JobIdApi jobRegisterJSDL (std::ifstream &jsdlFile, const std::string &delegationId, ConfigContext *cfs=NULL);



/**
*  Submits a job: performs registration and triggers the submission
* The JDL description of the job provided by the client is validated by the service,
* registered to the LB and finally passed to the Workload Manager.
* The unique identifier assigned to the job is returned to the client.
* Usage of this operation (instead of jobRegister + jobStart) is indeed recommended when the job identifier is not needed prior to its submission
* (e.g. jobs without input sandbox or with a sandbox entirely available on a GridFTP server managed by the client or her organisation).
* The service supports submission of simple jobs, parametric jobs, partitionable jobs, DAGs and collections of jobs;
* the description is always provided through a single JDL description.
* When a clients requests for submission of a complex object, i.e. parametric and partitionable jobs, DAGs and collections of jobs
* (all those requests represent in fact a set of jobs), the operations returns a structure containing the main identifier of the complex object
*  and the identifiers of all related sub jobs.
* This operation needs that a valid proxy (identified by an id string -delegationId string-) has been previously delegated to the endpoint.
* @param jdl The JDL string representation of the job
* @param delegationId The id string used to identify a previously delegated proxy
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ; if NULL default parameters are used
* @return The structure associated to the submitted job, with its jobid(s)
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException If the given JDL expression is not valid
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see #getProxyReq, #putProxy (proxy delegation)
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException. GenericException, BaseException
*/
JobIdApi jobSubmit(const std::string &jdl, const std::string &delegationId, ConfigContext *cfs=NULL);


/**
*  Submits a job: performs registration and triggers the submission
* The JDL description of the job provided by the client is validated by the service,
* registered to the LB and finally passed to the Workload Manager.
* The unique identifier assigned to the job is returned to the client.
* Usage of this operation (instead of jobRegister + jobStart) is indeed recommended when the job identifier is not needed prior to its submission
* (e.g. jobs without input sandbox or with a sandbox entirely available on a GridFTP server managed by the client or her organisation).
* The service supports submission of simple jobs, parametric jobs, partitionable jobs, DAGs and collections of jobs;
* the description is always provided through a single JDL description.
* When a clients requests for submission of a complex object, i.e. parametric and partitionable jobs, DAGs and collections of jobs
* (all those requests represent in fact a set of jobs), the operations returns a structure containing the main identifier of the complex object
*  and the identifiers of all related sub jobs.
* This operation needs that a valid proxy (identified by an id string -delegationId string-) has been previously delegated to the endpoint.
* @param jsdl The JSDL file stream that represents the job
* @param delegationId The id string used to identify a previously delegated proxy
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ; if NULL default parameters are used
* @return The structure associated to the submitted job, with its jobid(s)
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException If the given JDL expression is not valid
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see #getProxyReq, #putProxy (proxy delegation)
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException. GenericException, BaseException
*/
JobIdApi jobSubmitJSDL(std::ifstream &jsdlFile, const std::string &delegationId, ConfigContext *cfs=NULL);


/**
*  Triggers the submission of a previously registered job. It starts the actual processing of the registered job within the Workload Manager.
* It is assumed that when this operation is called, all the work preparatory to the job
* (e.g. input sandbox upload, registration of input data to the Data Management service etc.) has been completed by the client.<BR>
* Here follows an example of the sequence of operations to be performed for submitting a job:<BR>
* <UL>
* <LI> jobId = jobRegister(JDL)
* <LI> destURI = getSandboxDestURI(jobId) (for collections and DAG's it is performed for each node with InputSanbox files)
* <LI> transferring of the job Input Sandbox files to the returned destURI's (e.g. using GridFTP)
* <LI> jobStart(jobId) triggers the submission for a previously registered job (for collections and DAG's it is performed only for the job parent)
* </UL>
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ; if NULL default parameters are used
* @param jobid the string identification of the job (returned by jobRegister method)
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException If the given jobId expression is not valid
* @throws JobUnknownException The provided jobId has not been registered to the system
* @throws OperationNotAllowedException The current job status does not allow performing of the requested operation.
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException, JobUnknownException, OperationNotAllowedException, GenericException, BaseException
*/
void jobStart(const std::string &jobid, ConfigContext *cfs=NULL);


/**@name WMProxy Job operation services*/
/**
*  Sends cancellation request for a previously submitted job identified by its JobId.
* If the job is still managed by the WM then it is removed from the WM tasks queue.
* If the job has been already sent to the CE, the WM simply forwards the request to the CE.
* For suspending/releasing and sending signals to a submitted job the user has to check that the job has been scheduled to a CE
* and access directly the corresponding operations made available by the CE service.
* @param jobid The string identification of the job
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException If the given jobId is not valid
* @throws JobUnknownException The provided jobId has not been registered to the system
* @throws OperationNotAllowedException The current job status does not allow performing of the requested operation.
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see #jobPurge
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException, JobUnknownException, OperationNotAllowedException, GenericException, BaseException
*/
void jobCancel(const std::string &jobid, ConfigContext *cfs=NULL);
/**
*  Removes from the WM managed space all files related to the job identified by the jobId provided as input.
*  It can only be applied for job related files that are managed by the WM.
*  E.g. Input/Output sandbox files that have been specified in the JDL through a URI will be not subjected to this management.
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @param jobid The string identification of the job
* @return A pair containing the soft and the hard limit quota
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException If the given jobId is not valid
* @throws JobUnknownException The provided jobId has not been registered to the system
* @throws OperationNotAllowedException The current job status does not allow performing of the requested operation.
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see #jobCancel
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException, JobUnknownException, OperationNotAllowedException, GenericException, BaseException
**/
void jobPurge(const std::string &jobid, ConfigContext *cfs=NULL);

/**
* Returns the list of CE Ids satisfying the job Requirements specified in the JDL, ordered according to the decreasing Rank.
* The fault NoSuitableResourcesFault is returned if there are no resources matching job constraints.
* @param jdl the job description string
* @param delegationId The id string used to identify a previously delegated proxy
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return A vector containing, for each recource found, its full name and its rank
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException If the given JDL is not valid
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see #getProxyReq, #putProxy (proxy delegation)
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException, GenericException, BaseException
*/
std::vector <std::pair<std::string , long> > jobListMatch (const std::string &jdl, const std::string &delegationId, ConfigContext *cfs=NULL);


/**
* Returns the list of CE Ids satisfying the job Requirements specified in the JDL, ordered according to the decreasing Rank.
* The fault NoSuitableResourcesFault is returned if there are no resources matching job constraints.
* @param jsdl The JSDL file stream that represents the job
* @param delegationId The id string used to identify a previously delegated proxy
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return A vector containing, for each recource found, its full name and its rank
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException If the given JDL is not valid
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see #getProxyReq, #putProxy (proxy delegation)
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException, GenericException, BaseException
*/
std::vector <std::pair<std::string , long> > jobListMatchJSDL (std::ifstream &jsdlFile, const std::string &delegationId, ConfigContext *cfs=NULL);


/**@name WMProxy Info retrieval services*/

/** Retrieve the service version information
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ; if NULL default parameters are used
* @throws AuthenticationException An authentication problem occurred
* @throws GenericException A generic problem occurred
* @throws BaseException any other error occurred
* @return An alpha-numeric version representation
* @see thrown exceptions:  AuthenticationException, GenericException, BaseException
*/
std::string getVersion(ConfigContext *cfs=NULL);

/**
* Returns the maximum Input sandbox size (in bytes) that a user can count on for a job submission if using the space managed by the WM.
* This is a static value in the WM configuration (on a job-basis) set by the VO administrator
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ; if NULL default parameters are used
* @return The input sandbox maximum size in bytes
* @throws AuthenticationException An authentication problem occurred
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see thrown exceptions:  AuthenticationException, GenericException, BaseException
*/
long getMaxInputSandboxSize(ConfigContext *cfs=NULL);
/**
* Returns a list with the available transfer protocols on the WMProxy server
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ; if NULL default parameters are used
* @returns a vector if strings with the list of available protocols
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationExceptionBaseException The user is not authorized to perform this operation
* @throws GenericException A generic problem occurred
*/
std::vector<std::string> getTransferProtocols(ConfigContext *cfs);
/**
* Returns a list of destination URI's associated to the job (identified by the jobId provided as input)
* where the job input sandbox files can be uploaded by the client.
* The location is created in the storage managed by the WM (if needed) and
* the corresponding URI is returned to the operation caller if no problems has been arised during creation.
* Files of the job input sandbox that have been referenced in the JDL as relative or absolute paths
* are expected to be found in the returned location when the job lands on the CE.
* @param jobid The string identification of the job
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @param protocol string containing the protocol to use in the returned URI (Server available protocols are those returned by getTransferProtocols
* operation. Possible standard values are: "all" to get URIs with all available protocols; "default" to get URIs with the server default protocol
*  not mandatory, default value is "all")
* @return A vector containing a list of string representations of the Destination URI in all the available protocols
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationExceptionBaseException The user is not authorized to perform this operation
* @throws InvalidArgumentException If the given jobId is not valid
* @throws JobUnknownException The provided jobId has not been registered to the system
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see getSandboxBulkDestURI
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException, JobUnknownException, GenericException, BaseException
*/
std::vector<std::string>  getSandboxDestURI(const std::string &jobid, ConfigContext *cfs=NULL, const std::string &protocol="" );

/**
* Returns a list of destination URI's associated to the job and its children in case of DAG's and collections, where the job input sandbox files can be uploaded
* by the client on the WMS node. The job is identified by the jobId provided as input (the parent jobid for DAG's and collections).
* The locations are created in the storage managed by the WM and the corresponding URI is returned to the operation caller
* if no problems has been arised during creation.
* Files of the job input sandbox that have been referenced in the JDL as relative or absolute paths are expected to be found
* in the returned location when the job lands on the CE.
* Note that the WM service only provides a the URI of a location where the job input sandbox files can be stored but does not perform any file transfer.
* File upload is indeed responsibility of the client (through the GridFTP service available on the WMS node).
* The user can also specify in the JDL the complete URI of files that are stored on a GridFTP server (e.g. managed by her organisation);
* those files will be directly downloaded (by the JobWrapper) on the WN where the job will run without transiting on the WM machine.
* The same applies to the output sandbox files list, i.e. the user can specify in the JDL the complete URIs for the files of the output sandbox;
* those files will be directly uploaded (by the JobWrapper) from the WN to the specified GridFTP servers without transiting on the WMS machine.
* @param jobid The string identification of the job
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @param protocol string containing the protocol to use in the returned URI (Server available protocols are those returned by getTransferProtocols
* operation. Possible standard values are: "all" to get URIs with all available protocols; "default" to get URIs with the server default protocol
*  not mandatory, default value is "all")
* @return a vector of pair in which the first element is the jobid and the second one is a vector containing the list of URI's associated to the job in all the available protocols
:* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationExceptionBaseException The user is not authorized to perform this operation
* @throws InvalidArgumentException If the given jobId is not valid
* @throws JobUnknownException The provided jobId has not been registered to the system
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see #getSandboxDestURI
*/
 std::vector< std::pair<std::string ,std::vector<std::string > > > getSandboxBulkDestURI(std::string jobid, ConfigContext *cfs=NULL, const std::string &protocol="");

/*
* Returns the available user space quota on the storage managed by the WM.
* The fault GetQuotaManagementFault is returned if the quota management is not active on the WM.
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return A pair containing the soft and the hard limit quota
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws QuotaManagementException Quota management is not active on the WM
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see #getTotalQuota
* @see thrown exceptions:  AuthenticationException, AuthorizationException, QuotaManagementException, GenericException, BaseException
**/
std::pair<long, long> getFreeQuota(ConfigContext *cfs=NULL);
/**
* Returns the remaining free part of available user disk quota (in bytes).
* The fault GetQuotaManagementFault is returned if the quota management is not active.
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return A pair containing the soft and the hard limit quota
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws QuotaManagementException Quota management is not active on the WM
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see #getFreeQuota
* @see thrown exceptions:  AuthenticationException, AuthorizationException, QuotaManagementException, GenericException, BaseException
**/
std::pair<long, long> getTotalQuota(ConfigContext *cfs=NULL);
/**
*  Retrieves the list of URIs where the output files created during job execution have been stored in the WM managed space and the corresponding sizes in bytes.
* It can only be applied for files of the Output Sandbox that are managed by the WM (i.e. not specified as URI in the JDL).
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @param jobid The string identification of the job
* @param protocol string containing the protocol to use in the returned URI (Server available protocols are those returned by getTransferProtocols
* operation. Possible standard values are: "all" to get URIs with all available protocols; "default" to get URIs with the server default protocol
*  not mandatory, default value is "all")
* @return A vector containing, for each element, the URI of the output file and corresponding size in bytes
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException If the given jobId is not valid
* @throws JobUnknownException The provided jobId has not been registered to the system
* @throws OperationNotAllowedException The current job status does not allow performing of the requested operation.
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException, JobUnknownException, OperationNotAllowedException, GenericException, BaseException
*/
std::vector <std::pair<std::string , long> > getOutputFileList (const std::string &jobid, ConfigContext *cfs=NULL, const std::string &protocol="");



/**
* Returns the JDL string which identifier is the input JobId
* @param jobid the identifier of the job
* @param jdlType the type of the JDL to be retrieved (either ORIGINAL or REGISTERED)
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return the string with the JDL
* @throws AuthenticationFaultException : 	a generic authentication problem occured.
* @throws AuthorizationFaultException : 	the client is not authorized to perform this operation
* @throws InvalidArgumentFaultException : 	the given delegation id is not valid.
* @throws GenericFaultException : 		another problem occured.
* @throws BaseException Any other error occurred
*/
std::string getJDL(const std::string &jobid, const JdlType &jdlType, ConfigContext *cfs=NULL);


/**@name WMProxy Perusal services*/


/**
* Enables file perusal functionalities if not disabled with the specific jdl attribute during job
* register operation.
* Calling this operation, the user enables perusal for job identified by jobId, for files specified with fileList.
* After this operation, the URIs of perusal files generated during job execution can be retrieved by calling the getPerusalFiles service
* An empty fileList disables perusal.
* This method can be only used invoking WMProxy servers with version greater than or equal to 2.0.0;
*  the version of the server can be retrieved by calling the getVersion service.
* @param jobid the string with the job identifier
* @param files vector with the list of filenames to be enabled
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException If the given JDL is not valid
* @throws JobUnknownException The provided jobId has not been registered to the system
* @throws BaseException Any other error occurred
* @throws OperationNotAllowedException perusal was disabled with the specific jdl attribute.
* @see #getPerusalFiles
* @see #getVersion
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException, JobUnknownException, OperationNotAllowedException, BaseException
*/
void enableFilePerusal (const std::string &jobid, const std::vector<std::string> &files, ConfigContext *cfs=NULL);
/**
* Gets the URIs of perusal files generated during job execution for the specified file file.
* If allChunks is set to true all perusal URIs will be returned; also the URIs already requested with a
* previous getPerusalFiles operation. Default value is false.
* Perusal files have to be presiuosly enabled by calling the enableFilePerusal service
* This method can be only used invoking WMProxy servers with version greater than or equal to 2.0.0;
*  the version of the server can be retrieved by calling the getVersion service.
* @param jobid the string with the job identifier
* @param allchunks boolean value to specify when to get all chuncks
* @param jobid the string with the job identifier
* @param file the name of the perusal file be enabled
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @param protocol string containing the protocol to use in the returned URI (Server available protocols are those returned by getTransferProtocols
* operation. Possible standard values are: "all" to get URIs with all available protocols; "default" to get URIs with the server default protocol
*  not mandatory, default value is "all")
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException If the given JDL is not valid
* @throws JobUnknownException The provided jobId has not been registered to the system
* @throws OperationNotAllowedException perusal was disabled with the specific jdl attribute
* @throws BaseException Any other error occurred
* @see #enableFilePerusal
* @see #getVersion
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException, JobUnknownException, OperationNotAllowedException, BaseException
*/
std::vector<std::string> getPerusalFiles (const std::string &jobid, const std::string &file, const bool &allchunks, ConfigContext *cfs=NULL, const std::string &protocol="");



/**@name WMProxy Templates creation services*/


/**
* Creates a valid template ready for submission for a job
* @param type The jobtype of the job. Multiple jobtype can be specified toghether through the bitwise '|' operator
* @param executable The name of the executable to be executed
* @param arguments The arguments needed by the executable (empty string if not necessary)
* @param requirements A string representing the expression describing the requirements (which is an attribute of boolean type) for the job
* @param rank A string representing the expression for the rank (which is an attribute of double type) for the job
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return The JDL string representation of the job
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException one or more of the provided input parameters is not valid
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException, GenericException, BaseException
*/
std::string getJobTemplate (int type, const std::string &executable,const std::string &arguments,
			const std::string &requirements,const std::string &rank, ConfigContext *cfs=NULL);
/**
* Creates a valid template ready for submission for a DAG
* @param dependencies The dependency structure of the dag: each node must list all the nodes that depends on it.
* @param requirements A string representing the expression describing the requirements (which is an attribute of boolean type) for all the nodes of the dag
* @param rank A string representing the expression for the rank (which is an attribute of double type) for all the nodes of the dag
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return The JDL string representation of the DAG
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException one or more of the provided input parameters is not valid
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException, GenericException, BaseException
*/
std::string getDAGTemplate(NodeStruct dependencies, const std::string &requirements,const std::string &rank, ConfigContext *cfs=NULL);
/**
* Creates a valid template JDL for a collection of jobs
* @param jobNumber  The number of jobs to be created for the collection
* @param requirements A string representing the expression describing the requirements (which is an attribute of boolean type) for all the jobs in the collection
* @param rank A string representing the expression for the rank (which is an attribute of double type) for all the jobs in the collection
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return The JDL string representation of the collection of jobs
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException one or more of the provided input parameters is not valid
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException, GenericException, BaseException
*/
std::string getCollectionTemplate(int jobNumber, const std::string &requirements,const std::string &rank, ConfigContext *cfs=NULL);
/**
* Creates a valid template JDL for a parametric job
* @param attributes All the attributes containing a reference to a parameter. Multiple attributes can be specified toghegher through the bitwise '|' operator ( as specified in attribute)
* @param parameters  The number of different parameters to be created
* @param start  The starting point where to begin to parametrise (default value is 0)
* @param step  The step between one parameter and the next one among param_start (default value is 1)
* @param requirements A string representing the expression describing all the Job requirements (which is an attribute of boolean type)
* @param rank A string representing the expression for the rank (which is an attribute of double type) of the resource
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return The JDL string representation of the parametric job
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException one or more of the provided input parameters is not valid
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException, GenericException, BaseException
*/
std::string getIntParametricJobTemplate (std::vector<std::string> attributes , int parameters , int start , int step ,
				const std::string &requirements,const std::string &rank, ConfigContext *cfs=NULL);
/**
* Creates a valid template JDL for a parametric job
* @param attributes All the attributes that contains reference to a parameter. Multiple attributes can be specified toghegher through the bitwise '|' operator ( as specified in attribute)
* @param parameters A vector containing all the parameters
* @param requirements A string representing the expression describing all the Job requirements (which is an attribute of boolean type)
* @param rank A string representing the expression for the rank (which is an attribute of double type) of the resource
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return The JDL string representation of the parametric job
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws InvalidArgumentException one or more of the provided input parameters is not valid
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see thrown exceptions:  AuthenticationException, AuthorizationException, InvalidArgumentException, GenericException, BaseException
*/
std::string getStringParametricJobTemplate (std::vector<std::string>attributes, std::vector<std::string> parameters,
				const std::string &requirements,const std::string &rank, ConfigContext *cfs=NULL);

/**@name WMProxy Certificates manipulation/info retrieval services*/
/**
*  Generates a proxy certificate in PEM format with Base64 encoding. This method must be followed by a putProxy call
*  This method remains to keep compatibility with the version 1.0.0 of WMProxy servers,
*  but it will be soon deprecated. The version of the server can be retrieved by calling the getVersion service
* @param delegationId The id of the delegation to be created
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return The string representing the request, which has to be used as input while performing a putProxy for the created delegation Id
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see getVersion
* @see BaseException
*/

std::string getProxyReq(const std::string &delegationId, ConfigContext *cfs=NULL);
/**
*   Generates a proxy certificate in PEM format with Base64 encoding. This method must be followed by a putProxy call.
* This method can be only used invoking WMProxy servers with version greater than or equal to 2.0.0;
*  the version of the server can be retrieved by calling the getVersion service.
* @param delegationId The id of the delegation to be created
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return The string representing the request, which has to be used as input while performing a putProxy for the created delegation Id
* @throws BaseException::DelegationException If the request failed
* @throws BaseException Any other error occurred
* @see #getVersion
* @see #BaseException
*/
std::string grst1GetProxyReq(const std::string &delegationId, ConfigContext *cfs=NULL);
std::string grstGetProxyReq(const std::string &delegationId, ConfigContext *cfs=NULL);
/**
* Generates a proxy certificate in PEM format with Base64 encoding that is associated to a server side generated sting identifier (delegation ID)
* This method must be followed by a putProxy call.
* This method can be only used invoking WMProxy servers with version greater than or equal to 2.0.0;
*  the version of the server can be retrieved by calling the getVersion service.
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return A struct contaning two strings: the proxy certificate and the delegationID (generated on the server side)
* This proxy certificate has to be used as input while performing a putProxy for the created delegation Id
* @throws BaseException::DelegationException If the request failed
* @throws BaseException Any other error occurred
* @see #getVersion
* @see #BaseException
*/
ProxyReqStruct getNewProxyReq(ConfigContext *cfs);
/**
* Returns the version number of the delegation services
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the  object is created with the default parameters
* @throws BaseException::DelegationException If the request failed
* @throws BaseException Any other error occurred
* @see #getVersion
* @see #BaseException
*/
std::string getDelegationVersion(ConfigContext *cfs);
/**
* Returns the version number of the delegation interface
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the  object is created with the default parameters
* @throws BaseException::DelegationException If the request failed
* @throws BaseException Any other error occurred
* @see #getVersion
* @see #BaseException
*/
std::string getDelegationInterfaceVersion(ConfigContext *cfs);

/**
* Returns the time left of the delegated proxy that is identified by the "delegatioId" string.
* If an empty string is provided as input for delegationID, proxy with default identifier is ocnsidered.
* @param delegationID the identifier string of the delegated proxy (empty string for the default proxy)
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return the time left of the proxy
* @throws BaseException::DelegationException If the request failed
* @throws BaseException Any other error occurred
*/
int getProxyTerminationTime (const std::string &delegationId, ConfigContext *cfs);
/**
* Sends a request for removing of delegated proxy that is identified by the "delegatioId" string.
* If an empty string is provided as input for delegationID, proxy with default identifier is ocnsidered.
* @param delegationID the identifier string of the delegated proxy (empty string for the default proxy)
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @throws BaseException::DelegationException If the request failed
* @throws BaseException Any other error occurred
*/
void proxyDestroy (const std::string &delegationId, ConfigContext *cfs);
/**
* Sends a request for renewal of delegated proxy that is identified by the "delegatioId" string.
* If an empty string is provided as input for delegationID, proxy with default identifier is ocnsidered.
* @param delegationID the identifier string of the delegated proxy (empty string for the default proxy)
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @throws BaseException::DelegationException If the request failed
* @throws BaseException Any other error occurred
*/
std::string renewProxyReq (const std::string &delegationId, ConfigContext *cfs);
/**
* Associates the current proxy certificate file with a previously created delegation id.This method must be called after a getProxyReq call
*  This method remains to keep compatibility with the version 1.0.0 of WMProxy servers,
*  but it will be soon deprecated. The version of the server can be retrieved by calling the getVersion service
* @param delegationId The id of the delegation created previously (by a getProxyReq call)
* @param request The string request got by a previous call of a getProxyReq
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @throws AuthenticationException An authentication problem occurred
* @throws AuthorizationException The user is not authorized to perform this operation
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see #getProxyReq
* @see #getVersion
* @see BaseException
*/
void putProxy(const std::string &delegationId, const std::string &request, ConfigContext *cfs=NULL);
/**
* Associates the current proxy certificate file with a previously created delegation id.This method must be called after a getProxyReq call
* This method can be only used invoking WMProxy servers with version greater than or equal to 2.0.0;
*  the version of the server can be retrieved by calling the getVersion service.
* @param delegationId The id of the delegation created previously (by a getProxyReq call)
* @param request The string request got by a previous call of a getProxyReq
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @throws DelegationException If the request failed
* @throws GenericException A generic problem occurred
* @throws BaseException Any other error occurred
* @see #getProxyReq
* @see #getVersion
* @see BaseException
*/
void grst1PutProxy(const std::string &delegationId, const std::string &request, ConfigContext *cfs=NULL);
void grstPutProxy(const std::string &delegationId, const std::string &request, ConfigContext *cfs=NULL);
/**
* Returns the Delegated Proxy information identified by the delegationId string
* @param delegationId The id of the delegation created previously (by a getProxyReq call)
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return a struct with the information on the input proxy
* @throws DelegationException If the request failed
* @throws AuthenticationFaultException : 	a generic authentication problem occured.
* @throws AuthorizationFaultException : 	the client is not authorized to perform this operation.
* @throws InvalidArgumentFaultException : 	the given delegation id is not valid.
* @throws GenericFaultException : 		another problem occured.
* @throws BaseException Any other error occurred
* @see #getProxyReq
* @see #putProxy
* @see BaseException
*/
ProxyInfoStructType* getDelegatedProxyInfo(const std::string &delegationId, ConfigContext *cfs=NULL);
/**
* Returns the information related to the proxy used to submit a job that identified by its JobId.
* This operation needs that a valid proxy (identified by an id string -delegationId string-) has been previously delegated to the endpoint.
* @param jobId the identifier of the job
* @param cfs Non-default configuration context (proxy file, endpoint URL and trusted cert location) ;  if NULL, the object is created with the default parameters
* @return a struct with the information on the input proxy
* @throws DelegationException If the request failed
* @throws AuthenticationFaultException : 	a generic authentication problem occured.
* @throws AuthorizationFaultException : 	the client is not authorized to perform this operation.
* @throws InvalidArgumentFaultException : 	the given delegation id is not valid.
* @throws JobUnknownException The provided jobId has not been registered to the system
* @throws GenericFaultException : 		another problem occured.
* @throws BaseException Any other error occurred
* @see #getProxyReq
* @see #putProxy
* @see BaseException
*/
ProxyInfoStructType* getJobProxyInfo(const std::string &jobId, ConfigContext *cfs=NULL);


} // wmproxy namespace
} // wms namespace
} // glite namespace
#endif
//EOF












