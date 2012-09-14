// File: ChkptException.cpp
// Author: Alessio Gianelle <gianelle@pd.infn.it>
// Copyright (c) 2002 The European Datagrid Project - IST programme, all rights reserved.
// For license conditions see http://www.eu-datagrid.org/license.html
// Contributors are mentioned in the code where appropriate.

// $Id$

#include <boost/lexical_cast.hpp>
#include <stdio.h>
#include "glite/wms/checkpointing/ChkptException.h"

namespace glite {
namespace wms {
namespace checkpointing {
 
  ChkptException::ChkptException ( std::string file,
				   int line,
				   std::string method,
				   int code,
				   std::string exception_name) :
   glite::wmsutils::exception::Exception(file, line, method, code, exception_name ) {};
      
  EoSException::EoSException ( std::string file,
			       int line, 
			       std::string method ) :
    ChkptException(file, line, method, CHKPT_OutOfSet, "EndOfSet") {
    this->error_message = "We are at the end of the iterator.";
  }
  
  ULException::ULException ( std::string file,
			     int line,
			     std::string method,
			     std::string attr ) :
    ChkptException(file, line, method, CHKPT_UndefinedLabel, "UndefinedLabel") {
    this->error_message = "The attribute '" + attr + "' has not been set yet!";
  }
 
  WTException::WTException ( std::string file,
			     int line,
			     std::string method,
			     std::string attr,
			     std::string func ) :
    ChkptException(file, line, method, CHKPT_WrongType, "WrongType") {
    this->error_message = "The type of the parameter: '" + 
      attr + "' does not matched with the type of the function: '" + func + "'!";
  } 
  
  LFException::LFException ( std::string file,
			     int line,
			     std::string method,
			     std::string func,
			     int error_func ) :
    ChkptException(file, line, method, error_func, "LoadFailed") {
    this->error_message = "The load of the State failed when the function: '" + func + 
							"' is called! \n The called function returns with error's code: ";
    this->error_message.append( boost::lexical_cast<std::string>(error_func) );
  }
 
  SEException::SEException ( std::string file,
			     int line,
			     std::string method,
			     std::string attr ) :
    ChkptException(file, line, method, CHKPT_SyntaxError,"SyntaxError") {
    this->error_message = "Syntax Error catched for: '" + attr + "'";
  }
  
  ESException::ESException ( std::string file,
			     int line,
			     std::string method,
			     int code) :
    ChkptException(file, line, method, code, "EmptyState") {
    this->error_message = "It has been required a method on an empty State. Error code: ";
    this->error_message.append( boost::lexical_cast<std::string>(code) );
  }

} // checkpointing
} // wms
} // glite

// Local Variables:
// mode: c++
// End:
