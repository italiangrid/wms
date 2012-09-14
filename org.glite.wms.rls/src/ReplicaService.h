/***************************************************************************
 **  filename  : ReplicaService.h
 **  authors   : Enzo Martelli <enzo.martelli@ct.infn.it>
 **  Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
 **  For license conditions see LICENSE file or
 **  http://www.edg.org/license.html
 ****************************************************************************/

#ifndef GLITE_WMS_REPLICASERVICE_H
#define GLITE_WMS_REPLICASERVICE_H

#include <string>
#include <vector>

namespace glite {
namespace wms {
namespace rls {
	

class ReplicaService
{
public:
  
  virtual ~ReplicaService() = 0;

public:

  virtual void listReplica(
             const std::string& lfn,
             std::vector<std::string>& sfns
          ) = 0;
    
};

} // namespace rls
} // namespace wms
} // namespace glite

#endif 


