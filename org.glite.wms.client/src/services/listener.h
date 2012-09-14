/**
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
* 	Authors:	Alessandro Maraschini <alessandro.maraschini@datamat.it>
* 			Marco Sottilaro <marco.sottilaro@datamat.it>
*
*/

// $Id: DAGAd.cpp,v 1.11 2005/07/04 14:57:17 amarasch Exp


#ifndef GLITE_WMS_CLIENT_SERVICES_LISTENER_H
#define GLITE_WMS_CLIENT_SERVICES_LISTENER_H

#include "glite/wmsutils/jobid/JobId.h" // JobId
#include <boost/thread/thread.hpp>  // reading thread
#include <fstream>  // pipestream

namespace glite {
namespace wms{
namespace client {
namespace services {
/********************************
* Shadow Class
********************************/
class Shadow{
	public:
		/** Default Constructor*/
		Shadow();
		/** Copy Contructor */
		Shadow(const Shadow& shadow);
		/** = operation*/
		void Shadow::operator=(const Shadow& shadow);
		/**  Default destructror */
		virtual ~Shadow();
		/**Constructor with JobId
		@param the jobid of the attached/submitted job*/
		Shadow(glite::wmsutils::jobid::JobId jobid);
		/**
		* Write in the input stream pipe
		@param txt the string to be written
		*/
		void write(const std::string &txt);
		/** Determine whether the console has been started on local machine*/
		bool isLocalConsole ();
		bool isActive(){return active;};
		/**
		* Set the jobid
		*/
		void setJobId(const std::string &jobid);
		/**
		* Set the ending message policy
		* goodbyeMessage determine whether to print (true) or not (false) a goodbye message when detaching shadow console
		*/
		void setGoodbyeMessage(bool goodbyeMessage);
		/**
		* Set the port where console shadow is listening
		*/
		void setPort(int port);
		/**
		* Set the host where console shadow is listening
		*/
		void setHost(const std::string &host);
		/**
		* Set the Prefix where console shadow executable is stored
		*/
		void setPrefix(const std::string &path);
		/**
		* Set the root Pipe
		*/
		void setPipe(const std::string &root);
		/**
		* Set the storage path
		*/
		void setStorage(const std::string &path);
		/**
		* Retrieve the name of the input stream pipe
		*@return the string representation of the path
		*/
		std::string getPipeIn();
		/**
		* Retrieve the name of the output stream pipe
		*@return the string representation of the path
		*/
		std::string getPipeOut();
		/**
		* Retrieve the shadow host
		* @return the string representation of the host
		*/
		std::string getHost();
		/**
		* Retrieve the shadow port
		* @return the port where the shadow is listening
		*/
		int getPort();
		/**
		* Retrieve the Process Id linked to the shadow
		* @return the pid number
		*/
		int getPid();
		/**
		* Retrieve the JobId
		* @return the string representation of the jobid
		*/
		glite::wmsutils::jobid::JobId getJobId(){return jobid;}
		/**
		* Read a char from the Output buffer
		* @return the output stream next char
		*/
		char emptyOut();
		/**
		* Destroy all appended files/processes/streams, release memory
		*/
		void detach();
		/** Launch the console-shadow process and retrieve host&port info*/
		void console();
		/**Retrieve the name of the pipe*/
		std::string getPipe();
	private:
		void forkProcess(const std::string& command);
		/*** Kill the shadow listener process */
		static void terminate (int sig);
		/**
		* Once the consloe listener has started, port and pid information
		* have to be retrieved from the info named pipe
		*/
		int getConsoleInfo();
		glite::wmsutils::jobid::JobId jobid ;
		std::string pipeRoot,host,storage,prefix;
		int port,pid;
		bool writing,localConsole,goodbyeMessage;
		// Used for reading root pipe
		std::ifstream *ifstreamOut;
		char c;
		// STATIC VARIABLE: detach console
		static bool active;
};
/********************************
* Listener Class
********************************/
class Listener{
	public:
		/** Default destructror */
		Listener(Shadow *shadow);
		/** Default destructror */
		virtual ~Listener();
		/** Copy Contructor */
		Listener(const Listener& listener);
		/** = operation*/
		void Listener::operator=(const Listener& listener);
		/** Empty the output stram*/
		void emptyOut();
		/** Empty the input stream*/
		void emptyIn ();
		/** Run the listener*/
		void run();
		/** */
		void operator()();
	private:
		Shadow* shadow;
		boost::thread th_shadow;
};

}}}} // ending namespaces
#endif //GLITE_WMS_CLIENT_SERVICES_LBAPI_H

