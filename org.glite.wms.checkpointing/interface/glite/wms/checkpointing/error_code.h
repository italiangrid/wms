// File: error_code.h
// Author: Alessio Gianelle <gianelle@pd.infn.it>
// Copyright (c) 2002 The European Datagrid Project - IST programme, all rights reserved.
// For license conditions see http://www.eu-datagrid.org/license.html
// Contributors are mentioned in the code where appropriate.

// $Id$

/**
 * \file error_code.h
 * \brief Provides a list of errors return by the functions of the checkpointing API's. 
 * 
 * \version 0.1
 * \date 16 September 2002
 * \author Alessio Gianelle <gianelle@pd.infn.it>
*/

#ifndef GLITE_WMS_CHECKPOINTING_ERROR_CODE_H
#define GLITE_WMS_CHECKPOINTING_ERROR_CODE_H

#include "glite/wmsutils/exception/exception_codes.h"

namespace glite {
namespace wms {
namespace checkpointing {
  
  /**
   * \brief A list of errors return by some methods of the JobState class.
   */

  enum {
    BASE = glite::wmsutils::exception::WMS_CHKPT_ERROR_BASE, /**< Base value. */
    CHKPT_OutOfSet,        /**< \b 01 - The end of the iterator. */
    CHKPT_UndefinedLabel,  /**< \b 02 - You have request an attribute which is not defined. */
    CHKPT_WrongType,       /**< \b 03 - The type of the attribute mismatched with the type of the function. */
    CHKPT_LoadFailed,      /**< \b 04 - The query to the LB service raised an error. */
    CHKPT_SyntaxError,     /**< \b 05 - There has been a syntax error. */
    CHKPT_EmptyState,      /**< \b 06 - You required a method on an empty (not initialized) State. */
    CHKPT_NoStateId,       /**< \b 07 - The StateId is not set. */
    CHKPT_NoIterator,      /**< \b 08 - The iterator is not set. */
    CHKPT_SaveFailed,      /**< \b 09 - It is not possible to store the State in the LB. */
    CHKPT_ConnProb,        /**< \b 10 - There has been some problems connecting LB, you can try again. */	
    CHKPT_NotAuth,         /**< \b 11 - User are not authorized to save this State (only for partitionable)*/ 
  };

} // checkpointing
} // wms
} // glite 

#endif
