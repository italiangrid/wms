//
//  File :     dli_access_test.h
//
//
//  Author :   Enzo Martelli ($Author$)
//  e-mail :   "enzo.martelli@ct.infn.it"
//
//  Copyright (c) 2004 Istituto Nazionale di Fisica Nucleare (INFN).
//  All rights reserved.
//  See http://grid.infn.it/grid/license.html for license details.
//

#include <dlfcn.h>
#include <iostream>
#include <string>
#include <vector>
#include "DLI_ReplicaService.h"
#include "ReplicaServiceException.h"

using namespace glite::wms::rls::DLI;
using namespace glite::wms::rls;

int
main() {
/*
   void* dliLibHandle = NULL;
   DLI::create_DLI_t* createDli;
   DLI::create_DLI_with_timeout_t* createDli_with_timeout;
   DLI::destroy_DLI_t* destroyDli;
   DLI::DLI_ReplicaService* the_dli;
   string dliLib = "libglite_wms_rls_dli.so";

   dliLibHandle = dlopen (dliLib.c_str(), RTLD_NOW);
   if (!dliLibHandle) {
      std::cout<<"cannot load DLI helper lib " << dliLib<< std::endl;
      std::cout<<"dlerror returns: " << dlerror()<<std::endl;
      return 0;
   }
   else {
      createDli = (DLI::create_DLI_t*)dlsym(dliLibHandle,"create_DLI");
      createDli_with_timeout =
         (DLI::create_DLI_with_timeout_t*)dlsym(
            dliLibHandle,
            "create_DLI_with_timeout"
         );
      destroyDli = (DLI::destroy_DLI_t*)dlsym(dliLibHandle,"destroy_DLI");
      if (!createDli ||
          !createDli_with_timeout ||
          !destroyDli) {
         std::cout<<"cannot load DLI helper symbols"<<std::endl;
         std::cout <<"dlerror returns: " << dlerror()<<std::endl;
         dlclose(dliLibHandle);
         return 0;
      }
   }

   std::vector<std::string> sfns;
   std::string endpoint = "http://lfc-gilda.ct.infn.it:8085";
   std::string lfn = "lfn:/grid/gilda/tony/hostname.jdl";
   std::string proxy = "";
   int timeout = 0;
   if(timeout)
      the_dli = createDli_with_timeout(
                   endpoint,
                   proxy,
                   timeout
                );
   else
      the_dli = createDli(endpoint, proxy);

   try{
      the_dli->listReplica(lfn, sfns);
   }
   catch(const ReplicaServiceException&  ex){
      std::cout<< ex.reason() << std::endl;
   }
   destroyDli(the_dli);
   dlclose(dliLibHandle);

  for(int i = 0; i< sfns.size(); i++){
    std::cout << sfns[i] << std::endl;
  }

*/

  DLI_ReplicaService dli(
     "http://lfc-gilda.ct.infn.it:8085", 
     "",
     15
  );

  std::vector<std::string> sfns;
  try {
     dli.listReplica(
                 "lfn:/grid/gilda/tony/hostname.jdl",
                 sfns
     );
  }
  catch( const ReplicaServiceException& ex){
    std::cout << ex.what() << std:: endl;
    return -1;
  }

  for(int i = 0; i< sfns.size(); i++){
    std::cout << sfns[i] << std::endl;
  }
  return 0;
}

  

