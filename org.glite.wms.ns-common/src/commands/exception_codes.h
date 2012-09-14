/*
 * exception_codes.h
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 */

#ifndef _GLITE_WMS_MANAGER_NS_CLIENT_EXCEPTION_CODES_H_
#define _GLITE_WMS_MANAGER_NS_CLIENT_EXCEPTION_CODES_H_

#include "glite/wmsutils/exception/exception_codes.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace client {

enum {
      NSE_FATAL = glite::wmsutils::exception::WMS_NS_ERROR_BASE +1,
      NSE_CONNECTION_ERROR,
      NSE_AUTHENTICATION_ERROR,
      NSE_JOB_NOT_FOUND,
      NSE_JOB_NOT_DONE,
      NSE_EMPTY_JOB_LIST,
      NSE_NO_SUITABLE_RESOURCE,
      NSE_NOT_AUTHORIZED_USER,
      NSE_SANDBOX_IO,
      NSE_NOT_ENOUGH_SPACE,
      NSE_JDL_PARSING,
      NSE_MATCHMAKING,
      NSE_WRONG_COMMAND,
      NSE_NOT_ENOUGH_QUOTA,
      NSE_JOB_SIZE,
      NSE_MKDIR,
      NSE_PROXY_RENEWAL_FAILURE,
      NSE_IS_FAILURE,
      NSE_MULTI_ATTRIBUTE_FAILURE,
      NSE_NO_ERROR
};

} // namespace client
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif

