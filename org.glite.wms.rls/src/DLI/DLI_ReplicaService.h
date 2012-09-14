//
//  File :     DLI_ReplicaService.h
//
//
//  Author :   Enzo Martelli ($Author$)
//  e-mail :   "enzo.martelli@ct.infn.it"
//
//
//  Description:
//  Wraps the DLI soap client API
//
//
//  Copyright (c) 2004 Istituto Nazionale di Fisica Nucleare (INFN).
//  All rights reserved.
//  See http://grid.infn.it/grid/license.html for license details.
//

#ifndef GLITE_WMS_RLS_DLI_REPLICASERVICE_H
#define GLITE_WMS_RLS_DLI_REPLICASERVICE_H

#include <string>
#include <vector>

#include <stdsoap2.h>
#include "ReplicaService.h"

extern "C" {
   #include "glite/security/glite_gsplugin.h"
}



namespace glite {
namespace wms {
namespace rls {
namespace DLI {


class DLI_ReplicaService : public ReplicaService 
{
public:

  DLI_ReplicaService( 
     const std::string& endpoint, 
     const std::string& proxy, 
     int timeout
  );

  void listReplica(
             const std::string& inputData,
             std::vector<std::string>& sfns
       );

  ~DLI_ReplicaService();

private:

  struct soap m_soap;   

  glite_gsplugin_Context m_ctx; 

  std::string m_proxy;

  std::string m_endpoint;
};

typedef DLI_ReplicaService* create_DLI_t(
                     const std::string& endpoint, 
                     const std::string& proxy,
                     int timeout
        );

typedef void destroy_DLI_t(DLI_ReplicaService*);


} // namespace DLI
} // namespace rls
} // namespace wms
} // namespace glite

#endif
