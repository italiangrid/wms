#ifndef GLITE_WMS_COMMON_LDIF2CLASSAD_EXCEPTION_CODES_H
#define GLITE_WMS_COMMON_LDIF2CLASSAD_EXCEPTION_CODES_H

/*
 * exception_codes.h
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 */

#include "glite/wmsutils/exception/exception_codes.h"

namespace glite {
namespace wms {
namespace common {
namespace ldif2classad {

enum {
      LDAP_FATAL_ERROR = glite::wmsutils::exception::WMS_LDAP_ERROR_BASE +1 ,
      LDAP_CONNECTION_ERROR,
      LDAP_QUERY_ERROR,
      LDAP_UNDEFINED_VALUE_ERROR
};

} // namespace ldif2classad
} // namespace common
} // namespace wms
} // namespace glite

#endif

// EOF
