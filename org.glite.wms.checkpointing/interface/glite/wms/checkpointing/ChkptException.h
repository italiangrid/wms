// File: ChkptException.h
// Author: Alessio Gianelle <gianelle@pd.infn.it>
// Copyright (c) 2002 The European Datagrid Project - IST programme, all rights reserved.
// For license conditions see http://www.eu-datagrid.org/license.html
// Contributors are mentioned in the code where appropriate.

// $Id$

/**
 * \file ChkptException.h
 * \brief Provides an exception class for the checkpointing API. 
 * 
 * \version 0.1
 * \date 16 September 2002
 * \author Alessio Gianelle <gianelle@pd.infn.it>
*/

#ifndef GLITE_WMS_CHECKPOINTING_EXCEPTIONS_H
#define GLITE_WMS_CHECKPOINTING_EXCEPTIONS_H

#include "glite/wmsutils/exception/Exception.h"
#include "glite/wms/checkpointing/error_code.h"

namespace glite {
namespace wms {
namespace checkpointing {

  /**
   * \brief This ChkptException is thrown when something goes wrong in the checkpointing API.
   *
   * All the checkpointing exceptions derive from this class. 
   * \see glite::wms::common::Exception base class.
   */  

  class ChkptException : public glite::wmsutils::exception::Exception {
    
  protected: 
    /** 
     * Empty constructor
     */
    
    ChkptException() {};
    
  public:

    /** 
     * Constructor
     */

    ChkptException ( std::string file,   /**< the file name */
		     int line,           /**< the line number */
		     std::string method, /**< the name of the method */
		     int code,           /**< the code error according with the error_code.h file */
		     std::string exception_name /**< the name of the exception */ 
		     );
  };
  
  /** 
   * \brief Raised when the iterator arrives at the end of the list.
   *
   * \param file - the file name
   * \param line - the line number
   * \param method - the name of the method
   */
  class EoSException : public ChkptException {
    
  public:
    
    EoSException ( std::string file,    /**< the file name */
		   int line,            /**< the line number */
		   std::string method   /**< the name of the method */
		   );  
  };
  
  /**
   * \brief Raised when the attribute is not set.
   *
   * \param file - the file name
   * \param line - the line number
   * \param method - the name of the method
   * \param attr - the name of the "wrong" attribute
   */
  class ULException : public ChkptException {

  public:

    ULException ( std::string file,   /**< the file name */
		  int line,           /**< the line number */
		  std::string method, /**< the name of the method */
		  std::string attr    /**< the name of the "wrong" attribute */
		  );
  };

  /** 
   * \brief Raised when the type of the attribute mismatches with the type of the function.
   * 
   * \param file - the file name
   * \param line - the line number
   * \param method - the name of the method
   * \param attr - the name (or if possible the type) of the attribute
   * \param func - the type request by the function
   */
  class WTException : public ChkptException {

  public:

    WTException ( std::string file,  /**< the file name */
		  int line,          /**< the line number */
		  std::string method,/**< the name of the method */
		  std::string attr,  /**< the name (or if possible the type) of the attribute */
		  std::string func   /**< the type request by the function */
		  );
  };
    
  /** 
   * \brief Raised when a call to an external library (LB or JobID) gives an error.
   * \warning This is a \b FATAL exception. When it is raised the State could not be stored or retrieved.
   *
   * \param file - the file name
   * \param line - the line number
   * \param method - the name of the method
   * \param func - the name of the called funtion which raised the error
   * \param error_func - the error number raised by the called function
   */
  class LFException : public ChkptException {

  public:

    LFException ( std::string file,  /**< the file name */
		  int line,          /**< the line number */ 
		  std::string method,/**< the name of the method */
		  std::string func,  /**< the name of the called funtion which raised the error */
		  int error_func     /**< the error number raised by the called function */
		  );
  };

  /**
   * \brief Raised when syntax error is found during State Object initialization.
   * \warning This is a \b FATAL exception. When it is raised the State could not be stored or retrieved.
   *
   * \param file - the file name
   * \param line - the line number
   * \param method - the name of the method
   * \param attr - the name of the "wrong" attribute
   */
  class SEException : public ChkptException {
    
  public:
    
    SEException ( std::string file,  /**< the file name */
		  int line,          /**< the line number */ 
		  std::string method,/**< the name of the method */
		  std::string attr   /**< the name of the "wrong" attribute */
		  );
  };
  
  /**
   * \brief Raised when it has been required a method on an empty (not initialized) State.
   *
   * \param file - the file name
   * \param line - the line number
   * \param method - the name of the method
   * \param code - the error code 
   * \warning Check the error code inside the exception to know if it is the StateId 
   *  (-> CHKPT_NoStateId) or the UserData (-> Chkpt_EmptyState) which has not been set. 
   */
  class ESException : public ChkptException {

  public:
    
    ESException ( std::string file,  /**< the file name */
		  int line,          /**< the line number */ 
		  std::string method,/**< the name of the method */
		  int code           /**< the error code */
		  );
  };

} // checkpointing
} // wms
} // glite  

#endif

// Local Variables:
// mode: c++
// End:
