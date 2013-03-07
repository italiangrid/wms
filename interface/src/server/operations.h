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
// File: operations.h
// Author: Giuseppe Avellino <egee@datamat.it>
//

#ifndef GLITE_WMS_WMPROXY_WMPOPERATIONS_H
#define GLITE_WMS_WMPROXY_WMPOPERATIONS_H


#include "responsestruct.h"


// Web service available operations (see coreoperations.h for core operations).
// All methods are void, response values are inserted in the corresponding
// response structure.
// For more information about arguments see Web Service Description Language
// file (wsdl).

void getVersion(getVersionResponse& getVersion_response);

void getJDL(const std::string& job_id, JdlType jdltype,
            getJDLResponse& getJDL_response);

void getMaxInputSandboxSize(getMaxInputSandboxSizeResponse
                            &getMaxInputSandboxSize_response);

void getSandboxDestURI(getSandboxDestURIResponse& getSandboxDestURI_response,
                       const std::string& job_id, const std::string& protocol);

void getSandboxBulkDestURI(getSandboxBulkDestURIResponse
                           &getSandboxBulkDestURI_response, const std::string& job_id,
                           const std::string& protocol);

void getQuota(getQuotaResponse& getQuota_response);

void getFreeQuota(getFreeQuotaResponse& getFreeQuota_response);

void getOutputFileList(getOutputFileListResponse& getOutputFileList_response,
                       const std::string& job_id, const std::string& protocol);

void getJobTemplate(getJobTemplateResponse& getJobTemplate_response,
                    JobTypeList job_type, const std::string& executable,
                    const std::string& arguments, const std::string& requirements,
                    const std::string& rank);

void getDAGTemplate(getDAGTemplateResponse& getDAGTemplate_response,
                    GraphStructType dependencies, const std::string& requirements,
                    const std::string& rank);

void getCollectionTemplate(getCollectionTemplateResponse
                           &getCollectionTemplate_response, int job_number,
                           const std::string& requirements,
                           const std::string& rank);

void getIntParametricJobTemplate(getIntParametricJobTemplateResponse
                                 &getIntParametricJobTemplate_response, StringList *attributes, int param,
                                 int parameterStart, int parameterStep, const std::string& requirements,
                                 const std::string& rank);

void getStringParametricJobTemplate(getStringParametricJobTemplateResponse
                                    &getStringParametricJobTemplate_response, StringList *attributes,
                                    StringList *param, const std::string& requirements, const std::string& rank);
// DELEGATION OPERATION INTERFACE

void
getDelegationVersion(getVersionResponse& getVersion_response);

void
getDelegationIntefaceVersion(getVersionResponse& getVersion_response);

void getProxyReq(getProxyReqResponse& getProxyReq_response,
                 const std::string& delegation_id);

void putProxy(putProxyResponse& putProxyReq_response,
              const std::string& delegation_id, const std::string& proxy);

void renewProxyReq(std::string& renewProxyReq_response,const std::string& delegation_id);

void getNewProxyReq(std::pair<std::string, std::string> &retpair);

void destroyProxy(const std::string& delegation_id);

void getProxyTerminationTime(time_t& getProxyTerminationTime_response, const std::string& delegation_id);

void getACLItems(getACLItemsResponse& getACLItems_response,
                 const std::string& job_id);

void addACLItems(addACLItemsResponse& addACLItems_response,
                 const std::string& job_id, StringList *dnlist);

void removeACLItem(removeACLItemResponse& removeACLItem_response,
                   const std::string& job_id, const std::string& item);

void getProxyInfo(getProxyInfoResponse& getProxyInfo_response,
                  const std::string& id, bool isjobid = false);

void enableFilePerusal(enableFilePerusalResponse& enableFilePerusal_response,
                       const std::string& job_id, StringList *fileList);

void getPerusalFiles(getPerusalFilesResponse
                     &getPerusalFiles_response, const std::string& job_id, const std::string
                     &fileName, bool allChunks, const std::string& protocol);

void getTransferProtocols(getTransferProtocolsResponse
                          &getTransferProtocols_response);

void getJobStatusOp(getJobStatusResponse& getJobStatus_response,
                    const std::string& job_id);


#endif // GLITE_WMS_WMPROXY_WMPOPERATIONS_H

