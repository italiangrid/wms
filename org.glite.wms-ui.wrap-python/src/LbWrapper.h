#ifndef  ORG_GLITE_WMSUI_WRAPY_LBWRAPPER_H
#define ORG_GLITE_WMSUI_WRAPY_LBWRAPPER_H
/*
 * LbWrapper.h
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 *
 */
#include "glite/lb/JobStatus.h"
#include "glite/lb/Event.h"
#include "glite/lb/Job.h"

/**
 * Provide a wrapper for the glite::lb::JobStatus class
 * it allows the user to retrieve bookkeeping information over a sequence of jobs as well as to
 * perform a query on the LB server and get back all the jobs that match
 *
 * @brief LB JobStatus wrapper class
 * @version 0.1
 * @date 15 April 2002
 * @author Alessandro Maraschini <alessandro.maraschini@datamat.it>
*/

class Status {
  public:
  	
	/*
        * Status Wrapper class Constructor
	*@param jobid: the id of the job whose info are to be retrieved
	*@param level a number in the interval {0,1,2,3} specifying the verbosity of the LB info to be downloaded
        */
        Status (const std::string& jobid, int level);
	
	/**
	* Perform a query to the Lb Server allowing the user to specify several parameters
	*@param host the LB server host name
	*@param port the LB server listening port
	*@param tagNames a vector containing all the user tag names to be searched
	*@param tagValues a vector containing all the user tag values for the tagNames names. the size of tagNames must be the same as tagValues
	*@param excludes an integer representing all the states that do not have to be retrieved (used togheter with includes returns empty set)
	*@param includes an integer representing only the states that have to be retrieved (used togheter with excludes returns empty set)
	*@param issuer retrieves only job belonging to the specified proxy certificate issuer
	*@param from retrieves only job submitted after specified time ( in seconds after epoch)
	*@param to retrieves only job submitted before specified time ( in seconds after epoch)
	*@param ad if different from 0, retrieves Ad Status information as well
	*/
	Status (const std::vector<std::string>& jobids, 
		const std::string& host , 
		int port ,
		const std::vector<std::string>& tagNames,
		const std::vector<std::string>& tagValues,
		const std::vector<int>& excludes,
		const std::vector<int>& includes,
		std::string issuer,
		int from , 
		int to ,
		int ad );

	/**
	* Retrieve the Names of all available States
	*/
	static std::vector<std::string> getStatesNames() ;

	/**
	* Retrieve the Codes of all available States
	*/
	static std::vector<std::string> getStatesCodes() ;
	
	/**
	* Retrieve the name for a specific status
	*/
	std::string getStatusName(int statusNumber);

	/**
	* Retrieve the event attributes for a specific event
	*/
	std::vector< std::string > getStatusAttributes(int statusNumber);
	
	/**
	* Retrieve the status number for the current Job ID
	*/
	int getStatusNumber() ;

	/**Set an attribute value (of string type)
	* @param err the string description passed by reference. Python will consider it as a returning parameter
	* @return a couple of [ int , string ] representing the error code (0 for success) and the error string representation
	*/
	
	std::vector<std::string> get_error () ;
		
  private:
	std::string jobid;
	std::vector<glite::lb::JobStatus> states ;
	std::string error ;
	bool error_code ;
	void log_error ( const std::string& err) ;
};
/**
 * Provide a wrapper for the glite::lb::Event class
 * it allows the user to retrieve logging information over one or more jobs
 *
 * @brief LB Event wrapper class
 * @version 0.1
 * @date 15 April 2002
 * @author Alessandro Maraschini <alessandro.maraschini@datamat.it>
*/
class Eve{
  public:
  	/*
        * Event Wrapper class Constructor
	*@param jobid: the id of the job whose info are to be retrieved
        */
        Eve (const std::string& jobid);

     	/*
	* Event Wrapper class Constructor that performs a query to the Lb Server 
	* allowing the user to specify several parameters
	*
	*@param jobids The list of jobids on which to perform the QUERY
	*@param host the LB server host name
	*@param port the LB server listening port
	*@param tagNames a vector containing all the user tag names to be searched
	*@param tagValues a vector containing all the user tag values for the tagNames names. the size of tagNames must be the same as tagValues
	*@param excludes an integer representing all the states that do not have to be retrieved (used togheter with includes returns empty set)
	*@param includes an integer representing only the states that have to be retrieved (used togheter with excludes returns empty set)
	*@param issuer retrieves only job belonging to the specified proxy certificate issuer
	*@param from retrieves only job submitted after specified time ( in seconds after epoch)
	*@param to retrieves only job submitted before specified time ( in seconds after epoch)
	*@param ad if different from 0, retrieves Ad Status information as well
	*/
	Eve (const std::vector<std::string>& jobids,
	     const std::string& lbHost, 
	     int lbPort,
	     const std::vector<std::string>& tagNames,
	     const std::vector<std::string>& tagValues,
	     const std::vector<int>& excludes,
	     const std::vector<int>& includes,
	     const std::string& issuer,
	     int from,
	     int to);

	/**
	* Static method to retrieve the Names of all available Events
	*/
	static std::vector<std::string> getEventsNames() ;

	/**
	* Static method to retrieve the Codes of all available Events
	*/
	static std::vector<std::string> getEventsCodes() ;
	
	/**
	* Retrieve the name for a specific event
	*/
	std::string getEventName(int eventNumber);

	/**
	* Retrieve the event attributes for a specific event
	*/
	std::vector< std::string > getEventAttributes(int eventNumber);

	/**
	* Retrieve the events number for the current Job ID
	*/
	int getEventsNumber() ;
	
	std::vector<std::string> get_error () ;
	
  private:
	std::string jobid;
  	std::vector<glite::lb::Event> events;
	std::string error;
	bool error_code ;
	void log_error ( const std::string& err) ;
};

#endif
