/***************************************************************************
 *  filename  : ReplicaServiceException.cpp
 *  authors   : Enzo Martelli <enzo.martelli@ct.infn.it>
 *  Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
 *  For license conditions see LICENSE file or
 *  http://www.edg.org/license.html
 ***************************************************************************/

#include "ReplicaServiceException.h"

using namespace std;

namespace glite {
namespace wms {
namespace rls {


ReplicaServiceException::ReplicaServiceException(const string& str) 
            : exception(), m_str(str)
{
}

const char* 
ReplicaServiceException::what() const throw()
{ 
   return m_str.c_str(); 
}

ReplicaServiceException::~ReplicaServiceException() throw()
{
}


} // namespace rls
} // namespace wms
} // namespace glite
