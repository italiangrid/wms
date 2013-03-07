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
// File: gsoapfaultmanipulator.h
// Author: Giuseppe Avellino <egee@datamat.it>
//

#ifndef GLITE_WMS_WMPROXY_WMPGSOAPFAULTMANIPULATOR_H
#define GLITE_WMS_WMPROXY_WMPGSOAPFAULTMANIPULATOR_H

/**
 * Determines the type of the service fault referring to the error code
 * @param code fault code
 * @return the mapped code
 */
int getServiceFaultType(int code);

/**
 * Initializes the fault stack pointer with the appropriate service fault type
 * @param code fault code
 * @return a void stack pointer
 */
void * initializeStackPointer(int code);

/**
 * Sets the fault stack pointer and the other fields of the gSOAP fault
 * structure
 * @param soap soap instance pointer
 * @param type fault type
 * @param sp stack pointer
 */
void setFaultDetails(struct soap *soap, int type, void *sp);

/**
 * Sets the fields of the Base Fault Type. The service faults are "subclasses"
 * of the Base Faul Type
 * @param soap soap instance pointer
 * @param code fault code
 * @param method_name the name of the method that has caused the fault
 * @param time_stamp time stamp
 * @param error_code error code
 * @param description fault description
 * @param stack a vector containing the fault stack
 */
void setSOAPFault(struct soap *soap, int code, const std::string& method_name,
                  time_t time_stamp, int error_code, const std::string& description,
                  std::vector<std::string> stack);

/**
 * Sets the fields of the Base Fault Type. The service faults are "subclasses"
 * of the Base Faul Type. The stack vector is set as an empty vector.
 *
 * @param soap soap instance pointer
 * @param code fault code
 * @param method_name the name of the method that has caused the fault
 * @param time_stamp time stamp
 * @param error_code error code
 * @param description fault description
 */
void setSOAPFault(struct soap *soap, int code, const std::string& method_name,
                  time_t time_stamp, int error_code, const std::string& description);

#endif // GLITE_WMS_WMPROXY_WMPGSOAPFAULTMANIPULATOR_H
