/**
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
*       Authors:        Alessandro Maraschini <alessandro.maraschini@datamat.it>
*                       Marco Sottilaro <marco.sottilaro@datamat.it>
*/

//      $Id$

#ifndef GLITE_WMS_CLIENT_UTILS_LOGMAN_H
#define GLITE_WMS_CLIENT_UTILS_LOGMAN_H
/*
 * logman.h
 */
#include "excman.h"

namespace glite{
namespace wms{
namespace client{
namespace utilities{

// LOG LEVEL
enum LogLevel{
	WMSLOG_UNDEF,
        WMSLOG_DEBUG,
        WMSLOG_INFO,
        WMSLOG_WARNING,
        WMSLOG_ERROR,
        WMSLOG_FATAL,
        WMSLOG_NOMSG
};

class Log {
        public:
                /**
                * Default constructor (if no pathname is specified as input for the logfile, default file is <applName>_<UID>_<PID>_<timestamp>.log )
		* @param path the log file pathname
                * @param verbosity level
                */
                Log(LogLevel level = WMSLOG_WARNING);
  		/**
		* Creates a log file at the specified path
		* @param path pathname to the log files
		*/
		void createLogFile(const std::string &path);
                /**
                * prints the exception messages
                * @param exc the exception
                * @param debug flag indicating whether the formatted message have to be printed on the std-output
                */
                void print (severity sev, const std::string &header,glite::wmsutils::exception::Exception &exc, const bool debug=true, const bool cache=false);
                /**
		* prints a formatted error description of the input exception
                * @param sev severity of the message (WMS_NONE, WMS_INFO, WMS_WARNING, WMS_ERROR, WMS_FATAL)
                *@param title the title of the message
                *@param msg the message string
                * @param debug flag indicating whether the formatted message have to be printed on the std-output
                */
                void print (severity sev, const std::string &header, const std::string &msg="", const bool debug=true, const bool cache=false);
                /**
		* Prints a formatted message about the WMProxy server is being called
                * @param service the string with name of the service
                */
		void service(const std::string& service);
                /**
		* Prints a formatted message about the WMProxy server is being called which a list of parameters
                * @param service the string with name of the service
               * @param params the list of parameters
                */
		void service(const std::string& service, const std::vector <std::pair<std::string , std::string> > &params);
                /**
		* Prints a formatted message about the WMProxy server is being called for the
		* job which its identifier is jobid
                * @param service the string with name of the service
               * @param jobid the identifier of teh job
                */
		void service(const std::string& service, const std::string& jobid);
                /**
		* Prints a formatted message about the result of calling of WMProxy service
                * @param service the string with name of the service
                * @param msg the result message to be printed
                */
		void result(const std::string& service, const std::string msg) ;
		/**
                * get the absolute pathname of the log file
                *@return the pathname string
                */
                std::string getPathName (void) { return m_logFile; };


	private :
        	/**
                * log-file pathname
                */
                std::string m_logFile ;
        	/**
                * log-cache
                */
		std::string logCache;
                /**
                * debugging messages on the std-output
                */
		glite::wms::client::utilities::LogLevel dbgLevel ;
};

}}}} // ending namespaces
#endif
