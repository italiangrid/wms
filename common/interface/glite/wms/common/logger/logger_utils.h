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
// File: logger_utils.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>

// $Id$

#ifndef GLITE_WMS_COMMON_LOGGER_LOGGER_UTILS_H
#define GLITE_WMS_COMMON_LOGGER_LOGGER_UTILS_H

#include <cstdlib>
#include <boost/lexical_cast.hpp>
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#ifndef GLITE_WMS_NO_LOGGING

#define LOG(level, message) \
do { \
	glite::wms::common::logger::threadsafe::edglog \
		<< glite::wms::common::logger::setlevel(level) \
		<< message << std::endl; \
} while (0)

#define MESSAGE(message) \
__FUNCTION__ + "(" + __FILE__ + ":" + boost::lexical_cast<std::string>(__LINE__) + "): " << message

#define Debug(message) \
LOG( \
		glite::wms::common::logger::debug, \
		std::string("[Debug] ") + MESSAGE(message) \
   )
#define Info(message) \
LOG( \
		glite::wms::common::logger::info, \
		std::string("[Info] ") + MESSAGE(message) \
   )
#define Warning(message) \
LOG( \
		glite::wms::common::logger::warning, \
		std::string("[Warning] ") + MESSAGE(message) \
   )
#define Error(message) \
LOG( \
		glite::wms::common::logger::error, \
		std::string("[Error] ") + MESSAGE(message) \
   )
#define Severe(message) \
LOG( \
		glite::wms::common::logger::severe, \
		std::string("[Severe] ") + MESSAGE(message) \
   )
#define Critical(message) \
LOG( \
		glite::wms::common::logger::critical, \
		std::string("[Critical] ") + MESSAGE(message) \
   )
#define Fatal(message) \
do { \
	LOG( \
			glite::wms::common::logger::fatal, \
			std::string("[Fatal] ") + MESSAGE(message) \
	   ); \
		std::abort(); \
} while (0)

#else

#define Debug(message)
#define Info(message)
#define Warning(message)
#define Error(message)
#define Severe(message)
#define Critical(message)
#define Fatal(message) do { abort(); } while (0)

#endif

#endif

// Local Variables:
// mode: c++
// End:
