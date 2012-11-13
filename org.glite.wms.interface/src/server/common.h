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
// File: common.h
// Author: Giuseppe Avellino <egee@datamat.it>
//

#include <string>

namespace glite
{
namespace jdl
{
class Ad;
}

namespace jobid
{
class JobId;
}

namespace wms {
namespace wmproxy {
namespace server {

extern std::string sandboxdir_global;

// Possible values for jdl type attribute
enum type {
   TYPE_JOB,
   TYPE_DAG,
   TYPE_COLLECTION,
};

// Common methods used in both operations and coreoperations
void setGlobalSandboxDir();
/**
* Log Remote host info, call load script file,checkConfiguration, setGlobalSandboxDir
*/
void initWMProxyOperation (const std::string& operation);
void callLoadScriptFile(const std::string& operation);
int  getType(std::string jdl, glite::jdl::Ad * ad = NULL);
void checkJobDirectoryExistence(glite::jobid::JobId jid,int level = 0);
}}}}
