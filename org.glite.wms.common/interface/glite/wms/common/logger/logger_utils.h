// File: logger_utils.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#ifndef GLITE_WMS_COMMON_LOGGER_LOGGER_UTILS_H
#define GLITE_WMS_COMMON_LOGGER_LOGGER_UTILS_H

#include <cstdlib>
#include <boost/lexical_cast.hpp>
#include "glite/wms/common/logger/wms_log.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include <sstream>

#ifndef GLITE_WMS_NO_LOGGING

#ifdef GLITE_WMS_HAVE_SYSLOG_LOGGING


#define MESSAGE(message) \
__FUNCTION__ << "(" << __FILE__ << ":" << boost::lexical_cast<std::string>(__LINE__) << ")> " << message

#define Debug(message) \
do { \
        std::ostringstream os; \
        os << "[Debug] " << MESSAGE(message) << std::endl; \
        glite::wms::common::logger::wms_log::get_instance()->debug(os.str()); \
} while (0)

#define Info(message) \
do { \
        std::ostringstream os; \
        os << "[Info] " << MESSAGE(message) << std::endl; \
        glite::wms::common::logger::wms_log::get_instance()->info(os.str()); \
} while (0)

#define Warning(message) \
do { \
        std::ostringstream os; \
        os << "[Warning] " << MESSAGE(message) << std::endl; \
        glite::wms::common::logger::wms_log::get_instance()->warning(os.str()); \
} while (0)

#define Error(message) \
do { \
        std::ostringstream os; \
        os << "[Error] " << MESSAGE(message) << std::endl; \
        glite::wms::common::logger::wms_log::get_instance()->error(os.str()); \
} while (0)

#define Severe(message) \
do { \
        std::ostringstream os; \
        os <<"[Sever] " <<  MESSAGE(message) << std::endl; \
        glite::wms::common::logger::wms_log::get_instance()->sever(os.str()); \
} while (0)

#define Critical(message) \
do { \
        std::ostringstream os; \
        os <<"[Critical] " <<  MESSAGE(message) << std::endl; \
        glite::wms::common::logger::wms_log::get_instance()->critical(os.str()); \
} while (0)

#define Fatal(message) \
do { \
        std::ostringstream os; \
        os << "[Fatal] " << MESSAGE(message) << std::endl; \
        glite::wms::common::logger::wms_log::get_instance()->fatal(os.str()); \
	std::abort(); \
} while (0)

#else  //#ifndef GLITE_WMS_HAVE_SYSLOG_LOGGING

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



#endif //#ifdef GLITE_WMS_HAVE_SYSLOG_LOGGING


#else  //#ifndef GLITE_WMS_NO_LOGGING

#define Debug(message)
#define Info(message)
#define Warning(message)
#define Error(message)
#define Severe(message)
#define Critical(message)
#define Fatal(message) do { abort(); } while (0)

#endif  //#ifndef GLITE_WMS_NO_LOGGING

#endif //#ifndef GLITE_WMS_COMMON_LOGGER_LOGGER_UTILS_H

// Local Variables:
// mode: c++
// End:
