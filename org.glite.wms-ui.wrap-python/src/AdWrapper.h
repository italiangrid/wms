#ifndef  ORG_GLITE_WMSUI_WRAPY_ADWRAPPER_H
#define ORG_GLITE_WMSUI_WRAPY_ADWRAPPER_H
/*
 * AdWrapper.h
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 *
 */

#include "glite/jdl/JobAd.h"
#include "glite/jdl/ExpDagAd.h"
#include <list>
#include <string>

/**
  * Provides a wrapping class implementation of the Userinterface JobAd library distribution
  * it can manipulate simple Ad intancis as well as JobAd ruled instances. Loading is allowed both from file and from string
 *
 * @brief Provides a wrapping class implementation of the condor classad library distribution
  * @version 0.1
 * @date 15 April 2002
 * @author Alessandro Maraschini <alessandro.maraschini@datamat.it>
*/
class AdWrapper {
   public:
	/**Ad wrapper constructor
	* @param ad if set to 0 the wrapper is generated for a simple classad instance, otherwise for a JobAd instance
	*/
	AdWrapper( int ad=0);
	/**Ad wrapper destructor */
	~AdWrapper();
	/**Wrapper used to print a single string without newline char
	* @param ch the string to be printed to the standard output
	*/
	void printChar( const std::string &ch ) ;
	/**load a JobAd from String
	* @param jdl  the JobAd string representation
	*/
	bool fromString(const std::string &jdl) ;
	/**load a JobAd from File
	* @param jdl the file where the jdl has been stored
	*/
	bool fromFile(const std::string &jdl) ;
	/**convert the current Ad instance into a JobAd intace
	*/
	bool toJobAd () ;
	/**
	* convert the current Ad intance into a DagAd instance
	*/
	bool toDagAd () ;
	/**
	*convert the current partitionable job into a DagAd
	*@param jobids the jobids of the sub jobs of the dag to be created
	*/
	bool  toDagAd( const std::vector <std::string>&  jobids);
	/**Check the JobAd attributes and  values
	*/
	bool check();
	/**Convert the current the Ad/JobAd into its jdl string representation
	*/
	std::string toString();
	/**Convert the current the Ad-JobAd into its jdl string representation creating one line per attribute
	*/
	std::string toLines();
	/**Check rank and requirements expression for member-isMember statement syntax
	*@param multi a vector of all attributes that are multi valued and could belong to a member-ismember statement
	*/
	bool checkMulti ( const std::vector<std::string>& multi );
	/**Permanetly remove an attribute from the Ad instance
	*@param attr_name the name of the attribute to be removed
	*/
	bool removeAttribute ( const std::string& attr_name ) ;
	/**Set an attribute value (of string type)
	*@param attr_name the name of the attribute to be set
	*@param attr_value the string value to be set
	*/
	bool  setAttributeStr (const std::string& attr_name, const std::string& attr_value) ;
	/**Ad an attribute value (of string type) to a (possibly already existing value) in order to create a list of values
	*@param attr_name the name of the attribute to be set
	*@param attr_value the string value to be set
	*/
	bool  addAttributeStr (const std::string& attr_name, const std::string& attr_value) ;
	/**Set an attribute value (of integer type)
	*@param attr_name the name of the attribute to be set
	*@param attr_value the integer value to be set
	*/
	bool  setAttributeInt (const std::string& attr_name, int attr_value) ;
	/**Set an attribute value (of real type)
	*@param attr_name the name of the attribute to be set
	*@param attr_value the real value to be set
	*/
	bool  setAttributeReal (const std::string& attr_name, double attr_value) ;
	/**Set an attribute value (of bool type)
	*@param attr_name the name of the attribute to be set
	*@param attr_value the bool value to be set
	*/
	bool  setAttributeBool (const std::string& attr_name, bool attr_value) ;
	/**Set an attribute value (of expression type)
	*@param attr_name the name of the attribute to be set
	*@param attr_value the expression value to be set
	*/
	bool  setAttributeExpr (const std::string& attr_name, const std::string& attr_value) ;
	/**Check Wheter the instance has the specified attribute set
	*@param attr_name the name of the attribute to be set
	*/
	bool hasKey (const std::string& attr_name) ;
	/**Set an attribute value (of string type)
	* @param err the string description passed by reference. Python will consider it as a returning parameter
	* @return a couple of [ int , string ] representing the error code (0 for success) and the error string representation
	*/
	std::vector<std::string> get_error () ;
	/**Retrieve the list of all inserted attributes
	*/
	std::vector <std::string> attributes ( )  ;
	/**Retrieve the inserted value(s) for the specified attribute
	* @param attr_name the name of the attribute to be retrieved
	* @return a vector cantaining the values listed in the specified attribute ,  (1-size vector if the attribute has a single value)
	*/
	std::vector <std::string> getStringValue (const std::string& attr_name)  ;
	/**Retrieve the inserted list of strings for the specified attribute.
	* A list of this kind: {  {"one" , "two" }   , {"three" , "for", five" }   }
	* is converted into : [   "one" , "two" , ""   , "three" , "for", five" ]
	* @param attr_name the name of the attribute to be retrieved
	*@return a vector representing several list of strings: each block is separated by an empty-string value
	*/
	std::vector < std::string>  getStringList (const std::string& attr_name)  ;
	/**Retrieve the inserted value(s) for the specified attribute
	* @param attr_name the name of the attribute to be retrieved
	* @return a vector cantaining the values listed in the specified attribute ,  (1-size vector if the attribute has a single value)
	*/
	std::vector <int> getIntValue (const std::string& attr_name)  ;
	/**Retrieve the inserted value(s) for the specified attribute
	* @param attr_name the name of the attribute to be retrieved
	* @return a vector cantaining the values listed in the specified attribute ,  (1-size vector if the attribute has a single value)
	*/
	std::vector <bool> getBoolValue (const std::string& attr_name)  ;
	/**Retrieve the inserted value(s) for the specified attribute
	* @param attr_name the name of the attribute to be retrieved
	* @return a vector cantaining the values listed in the specified attribute ,  (1-size vector if the attribute has a single value)
	*/
	std::vector <double> getDoubleValue (const std::string& attr_name)  ;
	/**Retrieve an attribute of Expression type (such as rank, requirements...)
	* @param attr_name the name of the attribute to be retrieved
	* @return the attribute value into its string representation
	*/
	std::string getValueExpr (const std::string& attr_name)  ;
	/**Retrieve an Ad value into its string representation
	* @param attr_name the name of the attribute to be retrieved
	* @return the Ad value into its string representation
	*/
	std::string getAd( const std::string& name  ) ;
    private:
	glite::jdl::Ad* jad ;
	std::string error ;
	bool error_code ;
 };
 
/**
  * Provides a wrapping class implementation of the DagAd library distribution.
  * proveides all the needed methods to read, create, manipulate , fill and check a DagAd instance
 *
 * @brief Provides a wrapping class implementation of the DagAd library distribution
  * @version 0.1
 * @date 15 April 2003
 * @author Alessandro Maraschini <alessandro.maraschini@datamat.it>
*/
class DagWrapper {
   public:
	/**DagAd wrapper constructor
	* @param ad if set to 0 the wrapper is generated for a simple classad instance, otherwise for a JobAd instance
	*/
	DagWrapper();
	/**DagAd wrapper constructor from file
	* @param file the file where the dag jdl is stored
	*/
	DagWrapper( const std::string& file );
	/**DagAd wrapper destructor
	*/
	~DagWrapper();
	/**load a DagAd wrapper instance from file
	* @param file the file where the dag jdl is stored
	*/
	bool fromFile ( const std::string& file  ) ;
	/**load a DagAd wrapper instance from stirng
	* @param jdl the dagad string representation 
	*/
	bool fromString( const std::string& jdl) ;
	/**Check whether the instance has the specified attribute set
	*@param attr_name the name of the attribute to be set
	*/
	bool hasKey (const std::string& attr_name) ;
	/**Convert the current DagAd instance into its jdl string representation
	*@param level defines which kind of DagAd is beeing represented:  1 = CURRENT , 2 =SUBMISSION, 3= NO_NODES, 4=  MULTI_LINES, 5= RESTORED
	*/
	std::string toString ( int level = 0) ;
	/** Retrieve the sub-nodes submission strings
	*@return a vector containing the submission strings of the subjobs
	*/
	std::vector<std::string> getSubmissionStrings () ;
	/** Retrieve the DagAd nodes values of a specified attribute
	* @param attr_name the name of the attribute to be retrieved
	* @return a vector of string in pairs. Each pair contain the string representation of the jobid and a deep copy of the strin representation of the value of attr_name for this sub-job (if present) */
	std::vector<std::string> getSubAttributes ( const std::string& attr_name ) ;
	/** Set the specified jobid(s) into the current DagAd instance
	* @param jobids a vector containing N job id (S) where N is the number of DagAd sub jobs
	*/
	void setJobIds ( const std::vector <std::string>&  jobids ) ;
	/**Set an attribute value (of string type)
	*@param attr_name the name of the attribute to be set
	*@param attr_value the string value to be set
	*/
	bool setAttributeStr ( int attr_name , const std::string& attr_value ) ;
	/**Permanetly remove an attribute from the Ad instance
	*@param attr_name the name of the attribute to be removed
	*/
	bool removeAttribute   ( int attr_name ) ;
	/**Retrieve the inserted value(s) for the specified attribute
	* @param attr_name the name of the attribute to be retrieved
	* @return The string representation for the specified attribute value
	*/
	std::string getStringValue ( int attr_name ) ;
	/** Retrieve number of subjobs
	@return the number of nodes belonging to the dag*/
	int size();
	/** Retrieve DagAd Map
	* a vector of strings in pairs. Each two elements consist in a jobid-node couple
	*/
	std::vector<std::string> getMap ();
	std::vector<std::string> get_error () ;
   private:
	glite::jdl::ExpDagAd* dagad ;
	std::string error;
	bool error_code ;
	void log_error ( const std::string& err) ;
};
 #endif
