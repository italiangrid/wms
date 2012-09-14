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
// File: responsestruct.h
// Author: Giuseppe Avellino <egee@datamat.it>
//

#ifndef GLITE_WMS_WMPROXY_WMPRESPONSESTRUCT_H
#define GLITE_WMS_WMPROXY_WMPRESPONSESTRUCT_H

#include <string>
#include <vector>


// Possible values for "type" jdl attribute
enum ObjectType {
   WMS_JOB,
   WMS_DAG,
   WMS_COLLECTION,
};

// Possible values for "jobType" jdl attribute (and combination of some)
enum JobType {
   WMS_PARAMETRIC,
   WMS_NORMAL,
   WMS_INTERACTIVE,
   WMS_MPI,
};

enum JdlType {
   WMS_JDL_ORIGINAL,
   WMS_JDL_REGISTERED,
};

// Base fault type to hold all kinds of faults
struct BaseFaultType {
   std::string methodName;
   time_t timestamp;
   std::string *errorCode;
   std::string *description;
   std::vector<std::string> *faultCause;
};

// Struct used to communicate with wmp manager
struct wmp_fault_t {
   int code;
   std::string message;
   std::vector<std::string> stack;
};


// Struct used inside response structures (see below)

struct JobTypeList {
   std::vector<enum JobType> *jobType;
};

struct StringList {
   std::vector<std::string> *Item;
};

struct StringAndLongType {
   std::string name;
   long size;
};

struct StringAndLongList {
   std::vector<StringAndLongType*> *file;
};

struct JobIdStructType {
   std::string id;
   std::string *name;
   std::string *path;
   std::vector<JobIdStructType*> *childrenJob;
};


// TODO fill all needed attributes
struct JobStatusStructType {
   std::string jobid ;
   std::string status;
   std::vector <JobStatusStructType*> childrenJob;
};

struct GraphStructType {
   std::string *name;
   std::vector<GraphStructType*> *childrenJob;
};

struct DestURIStructType {
   std::string id;
   std::vector<std::string> *destURIs;
};

struct DestURIsStructType {
   std::vector<DestURIStructType*> *Item;
};

struct VOProxyInfoStructType {
   std::string user;
   std::string userCA;
   std::string server;
   std::string serverCA;
   std::string voName;
   std::string uri;
   std::string startTime;
   std::string endTime;
   std::vector<std::string> attribute;
};

struct ProxyInfoStructType {
   std::string subject;
   std::string issuer;
   std::string identity;
   std::string type;
   std::string strength;
   std::string startTime;
   std::string endTime;
   std::vector<VOProxyInfoStructType*> vosInfo;
};


// Response structures

struct getVersionResponse {
   std::string version;
};

struct getJDLResponse {
   std::string jdl;
};

struct jobRegisterResponse {
   JobIdStructType *jobIdStruct;
};

struct jobStartResponse {
};

struct jobSubmitResponse {
   JobIdStructType *jobIdStruct;
};

struct jobCancelResponse {
};

struct getMaxInputSandboxSizeResponse {
   long size;
};

struct getSandboxDestURIResponse {
   StringList *path;
};

struct getSandboxBulkDestURIResponse {
   DestURIsStructType *destURIsStruct;
};

struct getQuotaResponse {
   long softLimit;
   long hardLimit;
};

struct getFreeQuotaResponse {
   long softLimit;
   long hardLimit;
};

struct jobPurgeResponse {
};

struct getOutputFileListResponse {
   StringAndLongList *OutputFileAndSizeList;
};

struct jobListMatchResponse {
   StringAndLongList *CEIdAndRankList;
};

struct getJobTemplateResponse {
   std::string jdl;
};

struct getDAGTemplateResponse {
   std::string jdl;
};

struct getCollectionTemplateResponse {
   std::string jdl;
};

struct getProxyReqResponse {
   std::string request;
};

/*struct renewProxyReqResponse {
  std::string request;
};*/

struct putProxyResponse {
};

struct getIntParametricJobTemplateResponse {
   std::string jdl;
};

struct getStringParametricJobTemplateResponse {
   std::string jdl;
};

typedef std::vector<std::string> getACLItemsResponse;

struct addACLItemsResponse {
};

struct removeACLItemResponse {
};

struct getProxyInfoResponse {
   ProxyInfoStructType *items;
};

struct enableFilePerusalResponse {
};

typedef std::vector<std::string> getPerusalFilesResponse;

//typedef time_t getProxyTerminationTimeResponse;

struct getTransferProtocolsResponse {
   StringList *protocols;
};

struct getJobStatusResponse {
   JobStatusStructType *jobStatus;
};

#endif // GLITE_WMS_WMPROXY_WMPRESPONSESTRUCT_H
