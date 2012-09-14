// File: jobstate.h
// Author: Alessio Gianelle <gianelle@pd.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

/**
 * \file
 * \brief Provides the JobState object. 
 *
 * \version 0.1
 * \date 18 December 2002
 * \author Alessio Gianelle <gianelle@pd.infn.it>
*/

#ifndef GLITE_WMS_CHECKPOINTING_JOBSTATE_H
#define GLITE_WMS_CHECKPOINTING_JOBSTATE_H

#include <string>
#include <vector>

#include <glite/wms/checkpointing/step.h>
#include <glite/lb/context.h>

#include <boost/shared_ptr.hpp>

// classAd include files
#include "classad_distribution.h"


namespace glite {
namespace wms {
namespace checkpointing {
 
class StepsSet;

  /** 
   * \brief Provides the Job State.  
   *
   * From an internal point of view a JobState is described through a
   * ClassAd.  This ClassAd is composed by some privates attributes
   * used inside the EuDataGrid architecture; and by a ClassAds (which
   * contains all the pairs <em> <var, value> </em> define by the
   * user. This is the schema with the right names:
   * 
   * JobState = <br>
   * [ <br>
   *   StateId = "a string with the stateID"; <br>
   *   JobSteps = "the last step (an int) or a list of strings representing the iterator"; <br>
   *   CurrentStep = "an integer which is the \b index of the current step"; <br>
   *   UserData = <br>
   *   [ <br>
   *     "the <var, value> pairs set by the user" <br>
   *   ] <br>
   * ] <br>
   * */
  
  class JobState {
  
  public:

    /** This is an optional argument for the JobState class constructor. */
    typedef enum state_type { 
      empty,  /**< To construct an empty state object. */
      job     /**< To construct and initialize (through an LB query) a state object. */
    };

    /** \name Constructor/Destructor */
    /// @{
    /** Default Constructor. 
	\param type - If not set it create an empty object, otherwise if it is set 
	to \a job it retrieves <b>from the LB</b> the last saved state for that job 
	and creates a new JobState object. 
	\throw SEException If the string defining the classAd is not syntactically correct.
	\throw LFException If some errors occur querying the LB to retrieve the job state.
	\warning Both the exceptions are \b fatal exceptions!  */
    JobState( state_type type = empty );
    /** Construct the object from a string.
	\param state_str a string representig the classAd
	\throw SEException. If the string defining the classAd is not syntactically correct.
        \warning The LB context is not initialized.*/
    JobState( const std::string state_str );
    /** Construct the object from a classAd.
	\param state_ad The classAd
	\throw SEException. If the classAd is not syntactically correct.
        \warning The LB context is not initialized.*/
    JobState( classad::ClassAd *state_ad );
    /** Construct the object from a file.
	\param filename - the file with the classAd
	\throw SEException. If the classAd is not syntactically correct or if the file 
	could not be opened.
        \warning The LB context is not initialized.*/
    JobState( std::istream* file );
    /** Copy Constructors */
    JobState( const JobState &js );
    /**  Copy operator.
	 \param that - The JobState object to copy.
	 \return The current object. */
    JobState &operator=( const JobState &that );
    /** Destructor */
    ~JobState();
    /// @}
  
    /** \name Save value methods. 
	\note A valid identifier for a <var, value> pair begin with an alphabetic character 
	or an underscore, and are followed with any number of alphabetic, numeric, or underscore 
	characters. Identifiers are case-insensitive, but case-preserving.
	\warning If a similarly named expression (case-insensitive) already exists in the 
	UserData set, the old expression is deleted, and the new expression takes its place. */
    /// @{
    /** Saves an int value.
	\param name - A string to identify the name of the attribute to store.
	\param value - A int with the value of the attribute to store.
	\retval CHKPT_SyntaxError if name is not a valid identifier or if value is not set.
	\throw ESException If it is called on an empty JobState object. */
    int saveValue (const std::string& name, int value);
    /** Saves a boolean value.
	\param name - A string to identify the name of the attribute to store.
	\param value - A boolean with the value of the attribute to store.
	\retval CHKPT_SyntaxError if name is not a valid identifier or if value is not set.
	\throw ESException If it is called on an empty JobState object. */
    int saveValue (const std::string& name, bool value);
    /** Saves a double value.
	\param name - A string to identify the name of the attribute to store.
	\param value - A double with the value of the attribute to store.
	\retval CHKPT_SyntaxError if name is not a valid identifier or if value is not set.
	\throw ESException If it is called on an empty JobState object. */
    int saveValue (const std::string& name, double value);
    /** Saves a string value.
	\param name - A string to identify the name of the attribute to store.
	\param value - A char* with the value of the attribute to store.
	\retval CHKPT_SyntaxError if name is not a valid identifier or if value is not set.
	\throw ESException If it is called on an empty JobState object. */
    int saveValue (const std::string& name, const char* value);
    /** Saves a string value.
	\param name - A string to identify the name of the attribute to store.
	\param value - A string with the value of the attribute to store.
	\retval CHKPT_SyntaxError if name is not a valid identifier or if value is not set.
	\throw ESException If it is called on an empty JobState object. */
    int saveValue (const std::string& name, const std::string& value);
    /// @}

    /** \name Append value methods.
	Using these methods you can create attributes with list value.
	\warning The type of all values inside the same attribute list must be the same */
    /// @{
    /** Appends an int value to an already set attribute, or creates a new one.
	\param name - A string to identify the name of the attribute.
	\param value - A int with the value of the attribute to append.
	\retval CHKPT_SyntaxError if name is not a valid identifier or if value is not set.
	\retval CHKPT_WrongType if the type mismatched with the type of the already set attribute name.
	\throw ESException If it is called on an empty JobState object. */
    int appendValue (const std::string& name, int value);
    /** Appends a boolean value to an already set attribute, or creates a new one.
	\param name - A string to identify the name of the attribute.
	\param value - A boolean with the value of the attribute to append.
	\retval CHKPT_SyntaxError if name is not a valid identifier or if value is not set.
	\retval CHKPT_WrongType if the type mismatched with the type of the already set attribute name.
	\throw ESException If it is called on an empty JobState object. */
    int appendValue (const std::string& name, bool value);
    /** Appends a double value to an already set attribute, or creates a new one.
	\param name - A string to identify the name of the attribute.
	\param value - A double with the value of the attribute to append.
	\retval CHKPT_SyntaxError if name is not a valid identifier or if value is not set.
	\retval CHKPT_WrongType if the type mismatched with the type of the already set attribute name.
	\throw ESException If it is called on an empty JobState object. */
    int appendValue (const std::string& name, double value);
    /** Appends a string value to an already set attribute, or creates a new one.
	\param name - A string to identify the name of the attribute.
	\param value - A char* with the value of the attribute to append.
	\retval CHKPT_SyntaxError if name is not a valid identifier or if value is not set.
	\retval CHKPT_WrongType if the type mismatched with the type of the already set attribute name.
	\throw ESException If it is called on an empty JobState object. */
    int appendValue (const std::string& name, const char* value); 
    /** Appends a string value to an already set attribute, or creates a new one.
	\param name - A string to identify the name of the attribute.
	\param value - A string with the value of the attribute to append.
	\retval CHKPT_SyntaxError if name is not a valid identifier or if value is not set.
	\retval CHKPT_WrongType if the type mismatched with the type of the already set attribute name.
	\throw ESException If it is called on an empty JobState object. */
    int appendValue (const std::string& name, const std::string& value); 
    /// @}


    /** \name Get value methods. 
	Returns 1-size vector if the attribute has a single value. */
    /// @{
    /** Retrieves the integer value(s) of the specified attribute.
	\param name - A string to identify the name of the attribute.
	\return A vector of int with the values of the attribute.
	\throw ULException If the attribute doesn't exist.
	\throw WTException If you call the wrong typed function.
	\throw ESException If it is called on an empty JobState object. */
    std::vector<int> getIntValue (const std::string& name);
    /** Retrieves the boolean value(s) of the specified attribute.
	\param name - A string to identify the name of the attribute.
	\return A vector of bool with the values of the attribute.
	\throw ULException If the attribute doesn't exist.
	\throw WTException If you call the wrong typed function.
	\throw ESException If it is called on an empty JobState object. */
    std::vector<bool> getBoolValue (const std::string& name);
    /** Retrieves the double value(s) of the specified attribute.
	\param name - A string to identify the name of the attribute.
	\return A vector of double with the values of the attribute.
	\throw ULException If the attribute doesn't exist.
	\throw WTException If you call the wrong typed function.
	\throw ESException If it is called on an empty JobState object. */
    std::vector<double> getDoubleValue (const std::string& name);
    /** Retrieves the string value(s) of the specified attribute.
	\param name - A string to identify the name of the attribute.
	\return A vector of string with the values of the attribute.
	\throw ULException If the attribute doesn't exist.
	\throw WTException If you call the wrong typed function.
	\throw ESException If it is called on an empty JobState object. */
    std::vector<std::string> getStringValue (const std::string& name);
    /// @}

    /** \name Check-Type methods. 
        If the attribute is a list, the method check the type of the elements of the list.*/
    /// @{
    /** Checks if the specified attribute is of int type.
	\param name - A string to identify the name of the attribute.
	\return \a true if the specified attribute is of the requested type, \a false otherwise.
	\throw ULException If the attribute doesn't exist.
	\throw ESException If it is called on an empty JobState object. */
    bool isIntValue (const std::string& name);
    /** Checks if the specified attribute is of boolean type.
	\param name - A string to identify the name of the attribute.
	\return \a true if the specified attribute is of the requested type, \a false otherwise.
	\throw ULException If the attribute doesn't exist.
	\throw ESException If it is called on an empty JobState object. */
    bool isBoolValue (const std::string& name);
    /** Checks if the specified attribute is of double type.
	\param name - A string to identify the name of the attribute.
	\return \a true if the specified attribute is of the requested type, \a false otherwise.
	\throw ULException If the attribute doesn't exist.
	\throw ESException If it is called on an empty JobState object. */
    bool isDoubleValue (const std::string& name);
    /** Checks if the specified attribute is of string type.
	\param name - A string to identify the name of the attribute.
	\return \a true if the specified attribute is of the requested type, \a false otherwise.
	\throw ULException If the attribute doesn't exist.
	\throw ESException If it is called on an empty JobState object. */
    bool isStringValue (const std::string& name);
    /** Checks if the specified attribute is a list value.
	\param name - A string to identify the name of the attribute.
	\return \a true if the specified attribute is of the requested type, \a false otherwise.
	\throw ULException If the attribute doesn't exist.
	\throw ESException If it is called on an empty JobState object. */
    bool isListValue (const std::string& name);
    /// @}


    /** \name Miscellaneous methods. */
    /// @{
    /** Resets the ClassAd with the pairs set by the user (all the pairs are deleted).
	\throw ESException If it is called on an empty JobState object. */
    void clearPairs ( void );
    /** Set the stateId
	\param stateid - the ID of the State object. */
    void setId( const std::string& stateid );
    /** check the validity of a State Object.
	\return an error code.
	\retval CHKPT_NoStateId if the StateId has not been set.
	\retval CHKPT_EmptyState if the Object is empty (the UserData has not be set).
	\retval CHKPT_NoIterator if the stepper has not been set.
	\retval CHKPT_OutOfSet if the CurrentStep is out of the range defines by the JobSteps. */
    int checkState( void );
    /** Gets the next step of the associated iterator.
	\return A State object with the label of the next step in the iterator.
	\throw EoSException If we are at the end of the iterator.
	\throw ESException If the iterator has not be set. */
    Step getNextStep ( void );
    /** Gets the current step of the associated iterator.
	\return A Step object with the label of the current step in the iterator.
	\throw EoSException If we are at the end of the iterator.
	\throw ESException If the iterator has not been set. */
    Step getCurrentStep ( void );
    /** Convert the JobState object into a ClassAd.
	\return The ClassAd created. 
	\throw ESException If it is called on an empty JobState object. */
    classad::ClassAd toClassAd( void );
    /** Convert the JobState object into a string.
	\return The string obtained. 
	\throw ESException If it is called on an empty JobState object. */
    std::string toString( void );
    /** Store the JobState object into a file.
	\param filename - The name of the file.
	\throw ESException If it is called on an empty JobState object. */
    void toFile( const std::string& filename );
    /// @}
		
    /** \name Save and Load state methods. */
    /// @{
    /** Saves persistently the JobState object in a LB service.
	\retval CHKPT_SyntaxError if some "private" attributes are not correctly setted \b FATAL.
	\retval CHKPT_SaveFailed if some problems occur logging to LB 
                (EPERM || EINVAL || EDG_WLL_ERROR_NOJOBID) \b FATAL.
	\retval CHKPT_NotAuth if you are saving a state of another job (only for partitionable).
	\retval ENOSPC if LB infrastructure failed to accept the event due to lack of disk space.
	\retval ENOMEM if LB infrastructure failed to allocate memory.
	\retval CHKPT_ConnProb if some problems occur connecting LB 
                (EAGAIN || ECONNREFUSED) (you can retry).
	\throw SEException If an environment variable has not been set 
	        (only when we need to initialize the ctx).
	\throw LFException If some errors occur querying the LB to save the JobState.
	\throw ESException If it is called on an empty JobState object. */
    int saveState ( void );
    /** Retrieves the "last - num" JobState object for a job whose identifier is the one specified as argument.
	\param stateID - A string with the identifier for the state.
	\param num - The number of the event to retrieve: default is 0 == last!
	\return A JobState object initialized with the data retrieve from the LB service.
	\throw SEException If the string defining the classAd is not syntactically correct.
	\throw LFException If something goes wrong with the LB communication. 
        \warning Both the exceptions are \b fatal exceptions! 
	\warning If no state is found in the LB an empty object is returned.*/
    JobState loadState ( const std::string& stateID, int num = 0 );
    /// @}
	 	
  private:
    /** \name Auxiliary generic methods. */
    /// @{
    /** Create an LB context, useful to query the LB database.
	\return The edg-jobId as it is set in the GLITE_WMS_JOBID env variable.
	\throw SEException 
	\throw LFException  */
    const char *createContext( void );
    /** Clean the JobState object. */
    void removeall( void );
    /** Check if the object is empty.
	\param line - the line number which generate the error (___LINE__).
	\param func - the name of the function calling this method 
	              (to store it in the exception).
 	\throw ESException */
    void isEmpty(int line, const char *func);
    /** Adds a value to an already set attribute.
	This method is used by all the appendValue public methods. 
        \return CHKPT_SyntaxError.*/
    int addValue( classad::ExprTree* trees, const classad::Value& val , const std::string& name);
    /** Gets the untyped generic value of the given attribute.
	This method is used by all the getTypedValue public methods. 
        \throw ULException 
	\throw ESException. */
    classad::Value getUnTypedValue(const std::string& name); 
    /** Checks the type of the generic attribute value given.
	If the value is a list value, it returns the type of the list's elements. 
	This method is used by all the isTyped public methods. 
        \return "Int" || "Boolean" || "Double" || "String" || "Undefined" */
    const std::string getType( classad::ExprTree *tree ); 
    /** Initializes a JobState object.
	The parameter is the classAd defining the job state.
 	This method is used by all the constructors and by the load_state funcion.
        \throw SEException. */ 
    void initialize ( classad::ClassAd *cstate );
    /** Query the LB to obtain the string representing the Job State.
	This function is used by the default object constructor and by the loadState. 
        \param num the number of the event to retrieve: 0 == last!
	\throw LFException. */
    std::string getStateFromLB( const char *jobidi, int num = 0);
    /// @}
    
    std::string         js_stateId;            /**< The identifier, at the moment it is the EdgJobID. */
    boost::shared_ptr<edg_wll_Context> js_ctx; /**< Context opaque object used to log to LB. */
    StepsSet           *js_stepper;            /**< The stepper. */
    classad::ClassAd   *js_pairs;              /**< The set of <var, value> pairs defined by the user. */  
  };

} // checkpointing
} // wms
} // glite

#endif // GLITE_WMS_CHECKPOINTING_JOBSTATE_H

//  Local Variables:
//  mode: c++
//  End:
