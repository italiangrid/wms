#include "LbWrapper.h"
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <vector>
/**  LB Class implementation:  */
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/exception/Exception.h"
#include "glite/lb/producer.h"
#include "glite/lb/Job.h"

using namespace std ;
using namespace glite::lb ;
using namespace glite::wmsutils::exception ;

const int VECT_DIM = JobStatus::ATTR_MAX + 3;
const int STATUS = VECT_DIM  - 3 ;
const int STATUS_CODE = VECT_DIM - 2  ;
const int HIERARCHY  =VECT_DIM - 1  ;

// Create a query. This method is used both for queryStatus and for queryEvents

void createQuery (
	vector<vector<QueryRecord> >&cond ,
	// User Tags parameters:
	const std::vector<std::string>& tagNames,
	const std::vector<std::string>& tagValues,
	// Include-Exclude States parameters
	const std::vector<int>& excludes,
	const std::vector<int>& includes,
	// Issuer value (if -all selected):
	std::string issuer,
	// --from and --to options:
	int from , int to,  
	bool event = false
	)
{
	// USER TAGS QUERY
	for (unsigned int i = 0 ; i < tagNames.size() ; i++ ){
		cond.push_back(  vector<QueryRecord> (1, QueryRecord(tagNames[i], QueryRecord::EQUAL, tagValues[i])));
	}

	// EXCLUDES
	for (unsigned int i = 0 ; i < excludes.size() ; i++ ){
		if (event){
			cond.push_back(vector<QueryRecord>(1, QueryRecord(QueryRecord::EVENT_TYPE, QueryRecord::UNEQUAL, excludes[i])));
		}
		else{
			cond.push_back(vector<QueryRecord>(1, QueryRecord(QueryRecord::STATUS, QueryRecord::UNEQUAL, excludes[i])));
		}
	}

	// INCLUDES
	if (  includes.size()  > 0 ){
		vector<QueryRecord>  inclVect ;
		for (unsigned int i = 0 ; i < includes.size() ; i++ ){
			if (event){
				inclVect.push_back(QueryRecord(QueryRecord::EVENT_TYPE, QueryRecord::EQUAL, includes[i]));
			}else{
				inclVect.push_back(QueryRecord(QueryRecord::STATUS, QueryRecord::EQUAL, includes[i]));
			}
		}
		cond.push_back(  inclVect ) ;
	}

	// --ALL OPTION
	if ( issuer !="" ) {
		cond.push_back( vector<QueryRecord> (1,  QueryRecord   ( QueryRecord::OWNER , QueryRecord::EQUAL,  issuer ) ) );
	}
	// FROM - TO OPTIONS
	if ( from!=0 ) {
		timeval   tvFrom=  { from   ,0 };
		cond.push_back(  vector<QueryRecord> (1, QueryRecord   ( QueryRecord::TIME , QueryRecord::GREATER, JobStatus::SUBMITTED, tvFrom ) ) );
	}
	if ( to!=0 ) {
		timeval   toStruct = {  to   ,0 };
		cond.push_back(   vector<QueryRecord> (1, QueryRecord   ( QueryRecord::TIME , QueryRecord::LESS, JobStatus::SUBMITTED, toStruct )  )  );
	}
}

/******************************************************************************/
/* Status CLASS                                                                  */
/******************************************************************************/

void push_status( JobStatus status_retrieved , std::vector<std::string>& result , int hierarchy ){
	int VECT_OFFSET = result.size() ;
	result.resize( VECT_DIM + VECT_OFFSET ) ;
	char  tmp [1024];
	// push back additional Status information: STATUS
	result[VECT_OFFSET + STATUS  ] = status_retrieved.name()  ;
	// push back additional Status information:STATUS_CODE
	sprintf (tmp , "%d" ,  status_retrieved.status ) ;
	result[VECT_OFFSET +STATUS_CODE ] =  string( tmp ) ;
	// push back additional Status information:HIERARCHY
	sprintf (tmp , "%d" , hierarchy  ) ;
	result[VECT_OFFSET + HIERARCHY ] =  string( tmp ) ;
	// Iterate over the attributes
	std::vector<pair<JobStatus::Attr, JobStatus::AttrType> > attrList = status_retrieved.getAttrs();
	for (unsigned i=0; i < attrList.size(); i++ ) {
		switch (attrList[i].second) {
			case JobStatus::INT_T :{
				sprintf (tmp , "%d" , status_retrieved.getValInt(attrList[i].first) );
				result[VECT_OFFSET + attrList[i].first ] =  string ( tmp) ;
			}
			break;
			case JobStatus::BOOL_T    :{
				sprintf (tmp , "%d" , status_retrieved.getValBool(attrList[i].first  ) );
				result[VECT_OFFSET +  attrList[i].first ] =  string( tmp );
			}
			break;
			case JobStatus::STRING_T  :{
				result[VECT_OFFSET +  attrList[i].first ] =
					status_retrieved.getValString(attrList[i].first)  ;
			}
			break;
			case JobStatus::JOBID_T    :{
				if(((glite::wmsutils::jobid::JobId)status_retrieved.getValJobId(attrList[i].first)).isSet())
					result[VECT_OFFSET +  attrList[i].first ] =
						status_retrieved.getValJobId(attrList[i].first).toString()  ;
			}
			break;
			case JobStatus::STSLIST_T    :{
				// Do Nothing, JobStatus sons have been already processed
			}
			break;
			case JobStatus::STRLIST_T    :{
				std::vector<std::string> v = status_retrieved.getValStringList(attrList[i].first);
				for(unsigned int j=0; j < v.size(); j++)
					result[VECT_OFFSET +  attrList[i].first ] =  v[j]  ;
			}
			break;
			case JobStatus::TAGLIST_T    :{
				std::vector<std::pair<std::string,std::string> >  v =
					status_retrieved.getValTagList(   attrList[i].first   ) ;
				string res ;
				for (unsigned int j = 0 ; j < v.size() ; j++ )
					res +=v[j].first + "=" + v[j].second +";" ;
				result[VECT_OFFSET +  attrList[i].first ] =  res ;
			break;
			}
			case JobStatus::INTLIST_T    :{
				std::vector<int> v = status_retrieved.getValIntList(attrList[i].first);
				for(unsigned int j=0; j < v.size(); j++)
					sprintf (tmp , "%s%s%s%d%s", tmp , edg_wll_StatToString((edg_wll_JobStatCode) j ), "=" , v[j] , " ") ;
				result[VECT_OFFSET +  attrList[i].first ] = string(tmp) ;
			}
			break;
			case JobStatus::TIMEVAL_T    :{
				timeval t = status_retrieved.getValTime(attrList[i].first);
				sprintf (tmp , "%d", (int)  t.tv_sec) ;
				result[VECT_OFFSET +  attrList[i].first ] = string(tmp) ;
			}
			break;
			default : /* something is wrong */
				cerr << "\n\nFATAL ERROR!!! 	 has gone bad for "
					<< status_retrieved.getAttrName(attrList[i].first) << flush;
			break;
		}// end switch
	}// end for	
	// Upload info from Sub Jobs ( if present )
	std::vector<JobStatus> v = status_retrieved.getValJobStatusList(  JobStatus::CHILDREN_STATES );
	for(unsigned int i=0; i < v.size(); i++){
		push_status( v[i] , result , hierarchy+1 );
	}	
}

/*
* Statuts Wrapper class Constructor
*@param jobid the id of the job whose info are to be retrieved
*@param level a number in the interval {0,1,2,3} specifying the verbosity of the LB info to be downloaded
*/
Status::Status (const std::string& jobid, int level = 0) {

	// Initialise
	error_code = false ;

	// Set the private Job Id
	this->jobid = jobid;
	
	// Retrieve all the Statuses associated to the current Job ID
	try{
		// Set the Job ID
		glite::lb::Job lbJob = glite::wmsutils::jobid::JobId(this->jobid);
		
		// Check if the level is set in order to retrieve Class ADs
		if(0 != level) {
			// Set the Class Ads level
			level = glite::lb::Job::STAT_CLASSADS ;
		}
		
		// Retrieve the status for the Job ID
		glite::lb::JobStatus status = lbJob.status(level | glite::lb::Job::STAT_CHILDSTAT) ;
		
		// Insert the status retrieved
		states.push_back(status);
		
	}catch (std::exception &exc){
		
		// Log the error
		log_error ("Unable to retrieve the status for: " + jobid +"\n" + string (exc.what() ) ) ;
		
	} catch (...){  
	
		// Log the error
		error_code= true; 
		error = "Fatal Error: Unpredictalbe exception thrown by swig wrapper"; 
	}

}

Status::Status (
	const std::vector<std::string>& jobids,
 	// Lb Server address
	const std::string& host , int port ,
	// User Tags parameters:
	const std::vector<std::string>& tagNames,
	const std::vector<std::string>& tagValues,
	// Include-Exclude States parameters:
	const std::vector<int>& excludes,
	const std::vector<int>& includes,
	// Issuer value (if -all selected):
	std::string issuer,
	// --from and --to options:
	int from , int to ,
	// Verbosity Level
	int ad )
{
	try{
		error_code = false ;

		// Create and set a server
		ServerConnection server ;
		server.setQueryServer( host , port );

		// Retrieve query attributes
		std::vector<std::vector<std::pair<QueryRecord::Attr,std::string> > > ia = server.getIndexedAttrs();
		// indexed is used to check whether the user has performed a valid query (i.e. a query on the indexed attributes)
		bool indexed = false ;
		string queryErrMsg = "" ;
		for (unsigned int i = 0 ; i< ia.size() ;  i++ ){
			if (indexed) break ;
			for (unsigned int j = 0 ;  j < ia[i].size() ; j++ ) {
				if (indexed) break ;
				switch (   ia[i][j].first ){
					case QueryRecord::OWNER:
						if ( issuer!="") indexed = true ;
						queryErrMsg += "\n'--all' option" ;
						break;
					case QueryRecord::TIME:
						if(     ( from!=0 )  || ( to!=0 )   ) indexed = true ;
						queryErrMsg += "\n'--from'/'--to' option" ;
						break;
					case QueryRecord::USERTAG:
						for ( unsigned int k = 0 ; k<  tagNames.size()  ; k++ )
							if (   tagNames[k] == ia[i][j].second  )  indexed = true ;
						queryErrMsg += "\n'--user-tag " + ia[i][j].second +"=<tag value>' condition"   ;
					default:
						break;
				}
			}
		}
		if ( !indexed){
			// query is useless, no indexed value found
			if ( queryErrMsg =="" )
				queryErrMsg = " No indexed key found for the server: " +host ;
			else
				queryErrMsg =  " Try to use the following option(s):"   + queryErrMsg ;
			log_error ("No indexed attribute queried." + queryErrMsg ) ;
		}
		int FLAG  = 0 ;
		if (ad!=0) FLAG =   EDG_WLL_STAT_CLASSADS  ;
		
		// Create a vector of conditions		
		vector <vector<QueryRecord> >cond ;
		
		if(jobids.size() > 0) {
		
			// Create the vector of Query Records for the Job ID's
			vector<QueryRecord> queryRecordCond;
	
			// Fill the vector of Query Records for the Job ID's
			for (unsigned int i = 0; i<jobids.size(); i++){
				// Create a Query Record 
				QueryRecord queryRecord(QueryRecord::JOBID, QueryRecord::EQUAL, glite::wmsutils::jobid::JobId(jobids[i]));
			
				// Add the Query Record to the vector
				queryRecordCond.push_back(queryRecord);
			}

			// Add the Query Records of Job ID's to the Job Conditions
			cond.push_back(queryRecordCond);
		}
		
		// Create the conditions
		//createQuery (cond ,tagNames , tagValues , excludes , includes, issuer , from , to );
		
		// the Server will fill the result vector anyway, even when exception is raised
		if ( ! getenv ( "GLITE_WMS_QUERY_RESULTS") ) {
			server.setParam (EDG_WLL_PARAM_QUERY_RESULTS , 3 ) ;
		}
		
		// Perform the actual LB query:
		server.queryJobStates (cond, FLAG | EDG_WLL_STAT_CHILDSTAT , states) ;
		
	}catch (Exception &exc){
			if (exc.getCode()  ==E2BIG )  log_error ("Unable to retrieve all status information from: " + host + ": " +string (exc.what() ) ) ;
			else  log_error ("Unable to retrieve any status information from: " + host + ": " +string (exc.what() ) ) ;
	}catch (exception &exc){
			log_error ("Unable to retrieve any status information from: " + host + ": " +string (exc.what() ) ) ;
	} catch (...){  error_code= true; error = "Fatal Error: Unpredictalbe exception thrown by swig wrapper"; }
}

std::vector<std::string> Status::getStatesNames() {

	std::vector<std::string> result ;
	
	// Fill the returned array with the States Names
	for (int lb_attr = 0; (JobStatus::Attr)lb_attr < JobStatus::ATTR_MAX; lb_attr++){
		// Insert a single State Name
		result.push_back(JobStatus::getAttrName((JobStatus::Attr)lb_attr));
	}
	
	// Return the vector of States Names
	return result;
}

std::vector<std::string> Status::getStatesCodes(){

	std::vector<std::string> result ;

	// Fill the returned array with the States Codes
	for (int lb_code = 0; (JobStatus::Code)lb_code < JobStatus::CODE_MAX; lb_code++){
		// Insert a single State Code
		result.push_back(JobStatus::getStateName((JobStatus::Code)lb_code));
	}
	
	// Return the vector of States Codes
	return result;
}

/**
* Retrieve the name for a specific status
*/
std::string Status::getStatusName(int statusNumber) {
	// Return the status name
	return states[statusNumber].name();
}

/**
* Retrieve the event attributes for a specific event
*/
std::vector< std::string > Status::getStatusAttributes(int statusNumber) {

	// Initialise the output
	std::vector< std::string > statusAttributes;

	// Check if the event_number is valid
	if(statusNumber < 0 || statusNumber >= states.size()) {
		// Log the error
		log_error ("Status number out of bounds") ;
		
		// Return the empty array
		return statusAttributes;
	}
	
	// Retrieve the status	
	JobStatus statusRequested = states[statusNumber];
	
	// Fill the vector
	push_status(statusRequested, statusAttributes, 0);
	
	// Return
	return statusAttributes;
}
	


/**
* Retrieve the status number for the current Job ID
*/
int Status::getStatusNumber() {
	// Return the number of status
	return states.size() ;	
}

void Status::log_error ( const std::string& err) { 
	error_code = true ; 
	error = err ;
};

std::vector<std::string> Status::get_error ()  {

  std::vector<std::string> result ;
  
  // BUG 25250: PATCH TO AVOID COMPATIBILITY PROBLEMS WITH PYTHN CLI 
  result.push_back(error);
  result.push_back(error);
  // BUG 25250: PATCH TO AVOID COMPATIBILITY PROBLEMS WITH PYTHN CLI 
  
  error = "" ;

  return result;
}

/******************************************************************************/
/* Eve CLASS                                                                  */
/******************************************************************************/

/*
* Event Wrapper class Constructor
*@param jobid the id of the job whose info are to be retrieved
*/
Eve::Eve (const std::string& jobid) {

	// Initialise
	error_code = false ;

	// Set the private Job Id
	this->jobid = jobid;
	
	// Retrieve all the Events associated to the current Job ID
	try{
		// Set the Job ID
		glite::lb::Job lbJob = glite::wmsutils::jobid::JobId(this->jobid);
		
		// Retrieve the Events
		lbJob.log(events);
		
	}catch (Exception &exc){
		
		// Check for specific error E2BIG
		if (exc.getCode() == E2BIG) {
			// Log the error
			log_error ("Unable to retrieve all events from: " +jobid ) ;
		}
		
		// Log the error
		log_error ("Unable to retrieve the Job Events for: " +jobid+"\n" + string (exc.what() ) ) ;
		
	}catch (std::exception &exc){
	
		// Log the error
		log_error ("Unable to retrieve the Job Events for: " +jobid+"\n" + string (exc.what() ) ) ;
		
	} catch (...){  
	
		// Log the error
		error_code= true; 
		error = "Fatal Error: Unpredictalbe exception thrown by swig wrapper"; 
	}

}

/*
* Event Wrapper class Constructor
*@param jobid the id of the job whose info are to be retrieved
*/
Eve::Eve (const std::vector<std::string>& jobids,
  	  const std::string& lbHost, 
	  int lbPort,
	  const std::vector<std::string>& tagNames,
	  const std::vector<std::string>& tagValues,
	  const std::vector<int>& excludes,
	  const std::vector<int>& includes,
	  const std::string& issuer,
	  int from,
	  int to) 
{

	try{
	
		// Create the vector of Query Records for the Job ID's
		vector<QueryRecord> queryRecordCond;
	
		// Fill the vector of Query Records for the Job ID's
		for (unsigned int i = 0; i<jobids.size(); i++){
			// Create a Query Record 
			QueryRecord queryRecord(QueryRecord::JOBID, QueryRecord::EQUAL, glite::wmsutils::jobid::JobId(jobids[i]));
			
			// Add the Query Record to the vector
			queryRecordCond.push_back(queryRecord);
		}

		// Create a vector of conditions		
		vector<vector<QueryRecord> > jobCond;

		// Add the Query Records of Job ID's to the Job Conditions
		jobCond.push_back(queryRecordCond);

		// FILL Other parameters QUERY:
		error_code = false ;

		// Create the vector of Query conditions
		vector <vector<QueryRecord> > eveCond ;

		// Create the conditions
		createQuery (eveCond, tagNames, tagValues, excludes, includes, issuer, from, to,
			// LAST boolean parameter specify it is an Event query
			true);
			
		// Create and set a server
		ServerConnection server ;
		server.setQueryServer(lbHost, lbPort);

		// the Server will fill the result vector anyway, even when exception is raised
		if (!getenv ( "GLITE_WMS_QUERY_RESULTS")) {
			server.setParam(EDG_WLL_PARAM_QUERY_RESULTS, 3);
		}
		
		// Perform the actual LB query:
		server.queryEvents (jobCond, eveCond, events);
		
	}catch (Exception &exc){
			if (exc.getCode()  ==E2BIG ){
				log_error ("Unable to retrieve all event information from: " + lbHost + ": " +string (exc.what() ) ) ;
			} else{
				log_error ("Unable to retrieve any event information from: " + lbHost + ": " +string (exc.what() ) ) ;
			}
	}catch (exception &exc){
			log_error ("Unable to retrieve any status information from: " + lbHost + ": " +string (exc.what() ) ) ;
	} catch (...){  error_code= true; error = "Fatal Error: Unpredictalbe exception thrown by swig wrapper"; }

}

/**
* Static method to retrieve the Names of all available Events
*/
std::vector<std::string> Eve::getEventsNames(){

	std::vector<std::string> result ;

	// Fill the returned array with the Events Names
	for (int lb_attr = 0; (Event::Attr)lb_attr < Event::ATTR_MAX; lb_attr++){
		// Insert a single Event Name
		result.push_back(Event::getAttrName((Event::Attr)lb_attr));
	}
	
	// Return the vector of Event Names
	return result;
}

/**
* Static method to retrieve the Codes of all available Events
*/
std::vector<std::string> Eve::getEventsCodes(){

	std::vector<std::string> result ;

	// Fill the returned array with the Events Codes
	for (int lb_code = 0; (Event::Type)lb_code < Event::TYPE_MAX; lb_code++){
		// Insert a single Event Code
		result.push_back(Event::getEventName((Event::Type)lb_code));
	}
	
	// Return the vector of Events Codes
	return result;
}

/**
* Retrieve the name for a specific event
*/
std::string Eve::getEventName(int eventNumber) {
	// Return the event name
	return events[eventNumber].name();
}

/**
* Retrieve the event attributes for a specific event
*/
std::vector< std::string > Eve::getEventAttributes( int eventNumber ) {

	// Initialise the result vector of Event Attributes strings
	std::vector< std::string > eventAttributes;

	// Check if the event_number is valid
	if(eventNumber < 0 || eventNumber >= events.size()) {
		// Log the error
		log_error ("Event number out of bounds") ;
		
		// Return the empty array
		return eventAttributes;
	}
	
	eventAttributes.resize(Event::ATTR_MAX);

	// Retrieve the Event and Event Attributes
	Event eventRequested = events[eventNumber];
	std::vector<pair<Event::Attr, Event::AttrType> > attrList = eventRequested.getAttrs();

	// Fill the result vector with all the attributes found
	for (unsigned int attrCounter = 0; attrCounter < attrList.size(); attrCounter++ ) {
	
		// Conversion stream
		std::stringstream convStr;

		// Set the Event Attribute
		Event::Attr attribute = attrList[attrCounter].first;

		// Set the Event Attribute Type
		Event::AttrType attributeType = attrList[attrCounter].second;

		// Check for special cases
		if(Event::LEVEL == attribute) {
			// Set the Level string
			eventAttributes[attribute] = string (  edg_wll_LevelToString((edg_wll_Level)eventRequested.getValInt(attribute)));
		}
		else if (Event::STATUS_CODE == attribute) {
		
			// Build the string according to the type of the event
			if(Event::CANCEL == eventRequested.type) {
				eventAttributes[attribute] = string
					(edg_wll_CancelStatus_codeToString (
					(edg_wll_CancelStatus_code)eventRequested.getValInt(attribute) ) ) ;
			
			}
			else {
				eventAttributes[attribute] = string
					(edg_wll_DoneStatus_codeToString (
					(edg_wll_DoneStatus_code)eventRequested.getValInt(attribute) ) ) ;
			}	
		}
		else if(Event::JOBTYPE == attribute ||
			Event::RESULT == attribute) {
			// Set the attribute
			eventAttributes[attribute] = eventRequested.getValString(attribute);
		}
		else
		{
			// Read the attributes according to its type
			switch (attributeType) {
				
				case Event::LOGSRC_T :
				case Event::PORT_T :{

					// Convert the INT value to string and set the attribute
					eventAttributes[attribute] =  string(edg_wll_SourceToString((edg_wll_Source)eventRequested.getValInt(attribute)));
				}
				break;
			
				case Event::NOTIFID_T:
				case Event::INT_T :
				{

					// Convert the INT value to string
					convStr << eventRequested.getValInt(attribute);

					// Set the attribute
					eventAttributes[attribute] = convStr.str();
				}
				break;
			
				case Event::FLOAT_T :{

					// Convert the FLOAT value to string
					convStr << eventRequested.getValFloat(attribute);

					// Set the attribute
					eventAttributes[attribute] = convStr.str();
				}
				break;
			
				case Event::DOUBLE_T :{

					// Convert the DOUBLE value to string
					convStr << eventRequested.getValDouble(attribute);

					// Set the attribute
					eventAttributes[attribute] = convStr.str();
				}
				break;
			
				case Event::STRING_T  :{

					// Set the attribute
					eventAttributes[attribute] = eventRequested.getValString(attribute);
				}
				break;
			
				case Event::JOBID_T :{

					if(((glite::wmsutils::jobid::JobId)eventRequested.getValJobId(attribute)).isSet()) {
						eventAttributes[attribute] = eventRequested.getValJobId(attribute).toString();
					}
				}
				break;
			
				case Event::TIMEVAL_T :{
		
					char tmp [1024] ;
				
					memset(tmp, 0, sizeof(tmp));
				
					/* Convert the TIMEVAL value to string */
					timeval t = eventRequested.getValTime(attribute);
					sprintf (tmp , "%d", (int)  t.tv_sec) ;
					
					// Set the attribute
					eventAttributes[attribute] = string(tmp) ;
				}
				break;
				
				default : /* something is wrong */ {
					cerr << "\n\nFATAL ERROR!!! 	 has gone bad for "
						<< eventRequested.getAttrName(attribute) << flush;
				}					
				break;
	
			} // end switch

		} // end if

	} // end for	

	// Return 
	return eventAttributes;
}

/**
* Retrieve the events number for the current Job ID
*/
int Eve::getEventsNumber() {
	// Return the number of events
	return events.size();
}

void Eve::log_error ( const std::string& err) {    
	error_code = true ; 
	error = err ;
};

std::vector<std::string> Eve::get_error ()  {

  std::vector<std::string> result ;
  
  // BUG 25250: PATCH TO AVOID COMPATIBILITY PROBLEMS WITH PYTHN CLI 
  result.push_back(error);
  result.push_back(error);
  // BUG 25250: PATCH TO AVOID COMPATIBILITY PROBLEMS WITH PYTHN CLI 
  
  error = "" ;

  return result;
}
