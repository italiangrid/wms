/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

//
// File: structconverter.h
// Author: Giuseppe Avellino <egee@datamat.it>
//

#ifndef GLITE_WMS_WMPROXY_WMPSTRUCTCONVERTER_H
#define GLITE_WMS_WMPROXY_WMPSTRUCTCONVERTER_H

#include "soapH.h"

#include "responsestruct.h"
#include "glite/jdl/ExpDagAd.h" // JobIdStruct
#include "glite/jdl/adconverter.h" // NodeStruct

JobTypeList * convertFromGSOAPJobTypeList(ns1__JobTypeList *job_type_list);

std::vector<ns1__JobIdStructType*> * convertToGSOAPJobIdStructTypeVector(
   std::vector<JobIdStructType*> *graph_struct_type_vector);

std::vector<GraphStructType*> * convertFromGSOAPGraphStructTypeVector(
   std::vector<ns1__GraphStructType*> *graph_struct_type_vector);

GraphStructType* convertFromGSOAPGraphStructType(
   ns1__GraphStructType *graph_struct_type);

StringList * convertToStringList(ns1__StringList *ns1_string_list);

ns1__StringList * convertToGSOAPStringList(StringList * items);

ns1__StringList * convertVectorToGSOAPStringList(std::vector<std::string> items);

ns1__VOProxyInfoStructType * convertToGSOAPVOProxyInfoStructType(
   VOProxyInfoStructType *voproxyinfo);

std::vector<ns1__VOProxyInfoStructType*> convertToGSOAPVOProxyInfoStructTypeVector(
   std::vector<VOProxyInfoStructType*> voproxyinfovector);

ns1__ProxyInfoStructType * convertToGSOAPProxyInfoStructType(ProxyInfoStructType
      *proxyinfo);

std::vector<JobIdStructType*> * convertJobIdStruct(
   std::vector<glite::jdl::JobIdStruct*> &job_struct);

std::vector<glite::jdl::NodeStruct*> convertGraphStructTypeToNodeStructVector(
   std::vector<GraphStructType*> *graph_struct);

glite::jdl::NodeStruct convertGraphStructTypeToNodeStruct(
   GraphStructType graph_struct);

int convertJobTypeListToInt(JobTypeList job_type_list);

ns1__JobStatusStructType * convertToGSOAPJobStatusStructType ( JobStatusStructType *);


#endif // GLITE_WMS_WMPROXY_WMPSTRUCTCONVERTER_H
