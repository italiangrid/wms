/**
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
* 	Authors:	Alessandro Maraschini <alessandro.maraschini@datamat.it>
* 			Marco Sottilaro <marco.sottilaro@datamat.it>
*/

// 	$Id$


#ifndef GLITE_WMS_CLIENT_SERVICES_LBAPI_H
#define GLITE_WMS_CLIENT_SERVICES_LBAPI_H

#include "glite/wmsutils/jobid/JobId.h" // JobId
#include "glite/wms/wmproxyapi/wmproxy_api.h" // wmproxy API
#include "glite/lb/Job.h"  // Lb Job (Status & Loginfo)
namespace glite {
namespace wms{
namespace client {
namespace services {

/**
* Verbosity level
*/
enum Verbosity {
	VERB_MIN,
	VERB_BAS,
	VERB_NOR,
	VERB_JDL,
	VERB_MAX
};
/********************************
* Status Class
********************************/
class Status{
	public:
		enum OpCheck {
			OP_CANCEL,
			OP_OUTPUT,
			OP_ATTACH,
			OP_PERUSAL_SET,
			OP_PERUSAL_UNSET,
			OP_PERUSAL_GET
		};
		/** Default Constructor
		* @param status an instance of lb status
		 */
		Status(glite::lb::JobStatus status);
		/** Default Destructor */
		virtual ~Status() throw(){};
		/** Convert the Status info into a string
		* @param verb the level of verbosity needed
		* @return the string representation with the requested verbosity
		*/
		std::string toString(Verbosity verb);
		/** Retrieve the list of children for the jobid linked to the retrieved status
		* @return a vector of strings, each string a son. Empty vector for jobs
		*/
		std::vector<std::string> getChildren();
		/** Retrieve the list of states for children owned by retrieved jobid
		* @return a vector of Status, each Status beeing a son. Empty vector for jobs
		*/
		std::vector<Status> getChildrenStates();
		/** Retrieve the endpoint where the job has been submitted
		* @return the string representation of the wmproxy endpoint
		*/
		std::string getEndpoint();
		/** Retrieve current status
		* @param retrieve the string representation of the status name
		*/
		glite::wmsutils::jobid::JobId getParent();
		/** Check whether the jobid associated with the status
		* belong to a dag/collection/parametric job
		*/
		bool hasParent ( ) ;
		/** Retrieve the jobid related to retrieved Status
		* @return the string representation of the jobid
		*/
		glite::wmsutils::jobid::JobId getJobId();
		/** Retrieve relevant codes
		*@return a pair containing STATUS_CODE, DONE_CODE
		*/
		std::pair<int,int> getCodes();
		/**
		* Automatically determine if status allows requested operation
		*/
		int checkCodes(OpCheck, std::string &warn, bool child = false);
		/** Retrieve the JDL
		*@return the string representation of the JDL related to the job
		*/
		std::string getJdl();
		/**
		* Retrieve the string representation of the current status
		*/
		std::string getStatusName(){return status.name();}
	private:
		glite::lb::JobStatus status;
};
/********************************
* LbApi Class
********************************/
class LbApi{
	public :
		/********************************
		* Constructors/init parameters
		********************************/
		/** Constructor with String*/
		LbApi();
		/** Default Destructor */
		virtual ~LbApi() throw(){};
		/** switch to a JobId
		* Override (if present) previous JobId
		* param jobid the string representation of the working jobid
		*/
		void setJobId(const std::string& jobid);
		/** switch to a JobId
		* Override (if present) previous JobId
		* param jobid the working jobid
		*/
		void setJobId(const glite::wmsutils::jobid::JobId& jobid);
		/** Retrieve the status information
		* @param classads determine whether to download (true) or not(false) jdl information
		* @param subjobs determine whether to download (true) or not(false) children information
		* @return an instance of Status class
		*/
		Status getStatus(bool classads=false, bool subjobs=false);
	private:
		glite::lb::Job lbJob;

};
}}}} // ending namespaces
#endif //GLITE_WMS_CLIENT_SERVICES_LBAPI_H

