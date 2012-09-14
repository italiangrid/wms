/**
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
*       Authors:        Alessandro Maraschini <alessandro.maraschini@datamat.it>
*                       Marco Sottilaro <marco.sottilaro@datamat.it>
*/

//      $Id$

#include "lbapi.h"
#include "utilities/excman.h"
// wmproxy-api
#include "glite/wms/wmproxyapi/wmproxy_api_utilities.h"
// lb client
#include "glite/lb/producer.h"
#include "glite/lb/JobStatus.h"  // Lb Job (Status & Loginfo)
#include "glite/lb/LoggingExceptions.h"

using namespace std ;
using namespace glite::lb ;
using namespace glite::wms::wmproxyapi;
using namespace glite::wms::wmproxyapiutils;
using namespace glite::wms::client::utilities;

namespace glite {
namespace wms{
namespace client {
namespace services {
/********************************
* Status Class Implementation
********************************/
Status::Status(JobStatus status){
	this->status=status;
}
pair<int,int> Status::getCodes(){
	pair<int,int> result;
	result.first=status.status;
	result.second=status.getValInt(JobStatus::DONE_CODE);
	return result;
}
std::string Status::getEndpoint(){
	return status.getValString(JobStatus::NETWORK_SERVER);
}
int Status::checkCodes(OpCheck op, std::string& warn, bool child){
	int code = 0;
	switch (op) {
		case OP_CANCEL:   // CANCEL CHECK
		{
			switch (status.status){
				case JobStatus::SUBMITTED:
				case JobStatus::WAITING:
				case JobStatus::READY:
				case JobStatus::SCHEDULED:
				case JobStatus::RUNNING:
				case JobStatus::UNKNOWN:
				break;
				case JobStatus::DONE:
					if (status.getValInt(JobStatus::DONE_CODE) == JobStatus::DONE_CODE_FAILED){
						break;
					}
				default:
					throw WmsClientException(__FILE__,__LINE__,
					"checkCodes", DEFAULT_ERR_CODE,
					"Cancel not allowed",
					"Current Job Status is "+status.name());
			}
		}// END CANCEL CHECK
		break;
		case OP_OUTPUT: // OUTPUT CHECK
		{
			bool hasChildren= (status.getValInt(JobStatus::CHILDREN_NUM) !=0);
			if (hasChildren){
				// IT is a DAG: can continue unless:
				// it is SUBMITTED,WAITING or CLEARED
				if (status.status==JobStatus::CLEARED){
					throw WmsClientException(__FILE__,__LINE__,
					"checkCodes", DEFAULT_ERR_CODE,
					"Output not Allowed",
					"Output files already retrieved");
				}else if (status.status==JobStatus::SUBMITTED||status.status==JobStatus::WAITING){
					throw WmsClientException(__FILE__,__LINE__,
					"checkCodes", DEFAULT_ERR_CODE,
					"Output not yet ready",
					"Output files have not been generated");
				} else{
					// OUTPUT allowed: exit switch
					break;
				}
			}
			// If this point is reached the status is not of a DAG
			switch (status.status){
				case JobStatus::DONE:
					if (status.getValInt(JobStatus::EXIT_CODE) !=0){
						warn = "the status ";
						if (child){ warn += "for this child node ";}
						warn += "is DONE (ExitCode != 0)";
					} else if (status.getValInt(JobStatus::DONE_CODE) == JobStatus::DONE_CODE_FAILED){
						warn = "the status ";
						if (child){ warn += "for this child node ";}
						warn += "is DONE FAILED";
						code = -1;
					} else if (status.getValInt(JobStatus::DONE_CODE) == JobStatus::DONE_CODE_CANCELLED){
						warn = "the status ";
						if (child){ warn += "for this child node ";}
						warn += "is DONE CANCELLED";
					}  // ELSE is DONE SUCCESS: OK
					break;
				case JobStatus::ABORTED:{
						if(hasChildren){
							code = -1;
							// An aborted dag cannot continue
							if (child){
								warn = "unable to retrieve the output "
									"for this node (the status is ABORTED)";
							}else{
								throw WmsClientException(__FILE__,__LINE__,
								"checkCodes", DEFAULT_ERR_CODE,
								"Output not Allowed",
								"Unable to retrieve ourput files from ABORTED Dag");
							}
						}else{
							warn = "the status ";
							if (child){ warn += "for this child node ";}
							warn += "is ABORTED";
						}
					break;
				}
				case JobStatus::CLEARED:
					if (child){
						warn = "unable to retrieve the output for this node (the status is CLEARED)";
						code = -1;
					} else {
						throw WmsClientException(__FILE__,__LINE__,
						"checkCodes", DEFAULT_ERR_CODE,
						"Output not Allowed",
						"Output files already retrieved");
					}
				break;
				case JobStatus::CANCELLED:
					if (child){
						warn = "unable to retrieve the output  for this node (the status is CANCELLED)";
						code = -1;
					} else {
						throw WmsClientException(__FILE__,__LINE__,
						"checkCodes", DEFAULT_ERR_CODE,
						"Output not Allowed",
						"The Job has been cancelled");
					}
				break;
				default:
					if (child){
						warn = "unable to retrieve the output for this node (the status is " + status.name()+")" ;
						code = -1;
					} else {
						throw WmsClientException(__FILE__,__LINE__,
						"checkCodes", DEFAULT_ERR_CODE,
						"Output not yet Ready",
						"Current Job Status is: "+status.name() );
					}
					break;
			}
		} // END OUTPUT CHECK
		break;
		case OP_PERUSAL_GET:
		{
			if (status.getValInt(JobStatus::CHILDREN_NUM) !=0){
				throw WmsClientException(__FILE__,__LINE__,
				"checkCodes", DEFAULT_ERR_CODE,
				"Perusal not allowed",
				"Operation supported only by jobs");
			}
			switch (status.status){
				case JobStatus::DONE:
				case JobStatus::SCHEDULED:
				case JobStatus::ABORTED:
				case JobStatus::RUNNING:
				case JobStatus::WAITING:
				case JobStatus::SUBMITTED:
				case JobStatus::READY:
				case JobStatus::CANCELLED:
					// No problems with Perusal
					break;
				case JobStatus::CLEARED:
				default:
					// CLEARED or other unexpected status codes
					throw WmsClientException(__FILE__,__LINE__,
					"checkCodes", DEFAULT_ERR_CODE,
					"Retrieving Job file perusal not allowed",
					"Current Job Status is: "+status.name() );
			}
		} // END PERUSAL_GET CHECK
		break;
		case OP_PERUSAL_UNSET:
		case OP_PERUSAL_SET:
		{
			if (status.getValInt(JobStatus::CHILDREN_NUM) !=0){
				throw WmsClientException(__FILE__,__LINE__,
				"checkCodes", DEFAULT_ERR_CODE,
				"Perusal not allowed",
				"Operation supported only by jobs");
			}
			switch (status.status){
				case JobStatus::DONE:{

					if (status.getValInt(JobStatus::EXIT_CODE) !=0){
						// Exit code !=0, job might be resubmitted
						// perusal is still allowed
						break;
					}else if (status.getValInt(JobStatus::DONE_CODE) == JobStatus::DONE_CODE_FAILED){
						throw WmsClientException(__FILE__,__LINE__,
						"checkCodes", DEFAULT_ERR_CODE,
						"(un)setting Job file perusal not allowed",
						"Current Job Status is: Done (failed)" );
					} else if (status.getValInt(JobStatus::DONE_CODE) == JobStatus::DONE_CODE_CANCELLED){
						throw WmsClientException(__FILE__,__LINE__,
						"checkCodes", DEFAULT_ERR_CODE,
						"(un)setting Job file perusal not allowed",
						"Current Job Status is: Done (cancelled)" );
					}
					else{
						// Only exit code!=0 accepted
						// (otherwise the job is finished)
						break;
					}

				}
				break;
				case JobStatus::SCHEDULED:
				case JobStatus::RUNNING:
				case JobStatus::WAITING:
				case JobStatus::SUBMITTED:
				case JobStatus::READY:
					// No problems with Perusal
					break;
				case JobStatus::CLEARED:
				case JobStatus::ABORTED:
				default:
					// ABORTED CLEARED or other unexpected status codes
					throw WmsClientException(__FILE__,__LINE__,
					"checkCodes", DEFAULT_ERR_CODE,
					"(un)setting Job file perusal not allowed",
					"Current Job Status is: "+status.name() );
			}
		} // END PERUSAL_SET/UNSET CHECK
		break;
		case OP_ATTACH:
		default:
			throw WmsClientException(__FILE__,__LINE__,
			"checkCodes", DEFAULT_ERR_CODE,
			"Unable to perform",
			"Specified code not available");
		break;
	}
	return code;
}
std::vector<std::string> Status::getChildren(){
	return status.getValStringList(JobStatus::CHILDREN);
}
std::vector<Status> Status::getChildrenStates(){
	std::vector<Status> result;
	std::vector<JobStatus> v = status.getValJobStatusList(JobStatus::CHILDREN_STATES);
	unsigned int size = v.size();
	for(unsigned int i=0; i < size; i++){
		result.push_back(Status(v[i]));
	}
	return result;
}
std::string Status::getJdl()  {
	return status.getValString(JobStatus::JDL);
}


glite::wmsutils::jobid::JobId Status::getJobId(){
	return status.getValJobId(JobStatus::JOB_ID).toString();
}
glite::wmsutils::jobid::JobId Status::getParent(){
	glite::wmsutils::jobid::JobId jid ;
	try{
		jid = ((glite::wmsutils::jobid::JobId) status.getValJobId(JobStatus::PARENT_JOB));
	}catch (std::exception &exc){ /**Do nothing: parent not present*/}
	return jid ;
}

bool Status::hasParent ( ){
	glite::wmsutils::jobid::JobId pj ;
	pj = getParent() ;
	return (pj.isSet( ));
}

std::string Status::toString(Verbosity verb){
	string result;
	return result;
}
/********************************
* LbApi Class Implementation
********************************/
LbApi::LbApi(){}

void LbApi::setJobId(const std::string& jobid){
	lbJob = glite::wmsutils::jobid::JobId( jobid  ) ;
}
void LbApi::setJobId(const glite::wmsutils::jobid::JobId& jobid){
	lbJob = jobid;
}
Status LbApi::getStatus(bool classads, bool subjobs){
	int level=classads?(Job::STAT_CLASSADS):0;
	if (subjobs ){level |= Job::STAT_CHILDSTAT;}
	try{
		return Status (lbJob.status(level));
	} catch (glite::lb::Exception &exc){
		throw WmsClientException(__FILE__,__LINE__,
			"getStatus", DEFAULT_ERR_CODE,
			"Unable to retrieve the job status",
			exc.what());
	}
}

}}}} // ending namespaces
