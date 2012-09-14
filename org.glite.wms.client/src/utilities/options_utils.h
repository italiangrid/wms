/**
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
*       Authors:        Alessandro Maraschini <alessandro.maraschini@datamat.it>
*                       Marco Sottilaro <marco.sottilaro@datamat.it>
*/

//      $Id$

#ifndef GLITE_WMS_CLIENT_OPTIONSUTILS_H
#define GLITE_WMS_CLIENT_OPTIONSUTILS_H

#include <string>
#include <vector>

// WMProxy API's
#include "glite/wms/wmproxyapi/wmproxy_api.h"

#include "logman.h"

extern "C" {
#include <getopt.h>
}

namespace glite {
namespace wms{
namespace client {
namespace utilities {

std::string glite_wms_client_toLower ( const std::string &src);

class Options
{

	public:

		/*
		*	options attribute codes
		*/
		enum OptsAttributes{
                	NONE_ATTR = 1000 ,
			ALL ,
                        AUTODG ,
                        COLLECTION,
			CONFIG ,
			DAG,
			DBG , //debug
			DEFJDL,
                        DELEGATION,
			DIR ,
                        ENDPOINT,
			EXCLUDE ,
			GET,
			FILENAME,
			FROM ,
			HELP ,
			INPUT ,
			JDL,
			JDLORIG,
   			LISTONLY,
			LRMS ,
			LOGFILE ,
			NODESRES,
			NODISPLAY,
			NOGUI ,
			NOINT ,
			NOLISTEN ,
			NOMSG ,
			NOPURG ,
			OUTPUT ,
			PORT ,
			PROTO,
			PROXY,
			RANK ,
                        REGISTERONLY,
			RESOURCE ,
			SET,
			START ,
			STATUS ,
			TO ,
			TRANSFER,
			UNSET,
			USERTAG,
			VALID,
			VERBOSE ,
			VERSION,
                        VO,
			INPUTFILE
		} ;
                enum TimeOpts{
                	TIME_NO_OPT,
			TIME_FROM,
                        TIME_TO,
                        TIME_VALID
                } ;
		/*
		*	Type of command
		*/
		enum WMPCommands{
			JOBSUBMIT,
			JOBCANCEL,
			JOBSTATUS,
			JOBLOGINFO,
			JOBMATCH,
			JOBOUTPUT,
			JOBATTACH,
                        JOBDELEGATION,
			JOBINFO,
			JOBPERUSAL
		};
		/*
		*	Default constructor
		*/
		Options (const WMPCommands &command);
		/*
		*	Default destructor
		*/
		 ~Options( );
		/**
		*	 Reads the input options for the submission
		*	@param argc number of options
		*	@param argv options
		*/
		void readOptions(const int &argc, const char **argv);
		/**
		*	Returns the value of the option string-attribute
		*	@param attribute name of the attribute
		*	@return the pointer to the string attribute value (NULL if the attribute has not been set)
		*/
		std::string getStringAttribute (const Options::OptsAttributes &attribute);
		/**
		*	Returns the value of the option string-attribute
		*	@param attribute name of the attribute
		*	@return the pointer to the int attribute value (NULL if the attribute has not been set)
		*/
		int* getIntAttribute (const Options::OptsAttributes &attribute);
		/**
		*	Returns the value of the option string-attribute
		*	@param attribute name of the attribute
		*	@return the value of the boolean attribute
		*/
		bool getBoolAttribute (const Options::OptsAttributes &attribute);
		/**
		*	Returns the value of the option list of strings-attribute
		*	@param attribute name of the attribute
		*	@return the vector with list
		*/
		const std::vector<std::string> getListAttribute (const Options::OptsAttributes &attribute);
                /**
		*	Returns the short help usage message for an option attribute
		*	@param attribute name of the attribute
		*	@return the description string of the attribute
		*/
		const std::string getAttributeUsage (const Options::OptsAttributes &attribute);
                /**
		*	Returns a vector containing the list of available protocols
		*	@return the vector with the list of available protocols
		*/
		static const std::vector<std::string> getProtocols() ;
		 /**
		*	Returns a string the list of available protocols
		*	@return the string with the list of available protocols
		*/
		static const std::string getProtocolsString() ;
		/**
		*	Returns a vector containing the list of job identifiers
		*	@return a vector with the list of jobid's
		*/
		const std::vector<std::string> getJobIds( );
		/**
		*	Returns the job identifiers for commands that need only one JobId
		*	@return the string with the JobId
		*/
		const std::string getJobId( );
		/**
		*	Returns the path to the JDL file
		*	@return a pointer to the string of the JDL pathname
		*/
		std::string Options::getPath2Jdl ();
		/**
		*	Displays the usage help message for the submission
		*	@param exename name of the programme executable
		*	@param long displays the long usage help if it is "true"
		*/
		void submit_usage(const char* &exename, const bool &long_usg=false) ;
		/**
		*	Displays the usage help message for the submission command
		*	@param exename name of the programme executable
		*	@param long displays the long usage help if it is "true"
		*/
		void status_usage(const char* &exename, const bool &long_usg=false) ;
		/**
		*	Displays on the standard output the usage help message for the status command
		*	@param exename name of the programme executable
		*	@param long displays the long usage help if it is "true"
		*/
		void loginfo_usage(const char* &exename, const bool &long_usg=false) ;
		/**
		*	Displays on the standard output  the usage help message for the cancel command
		*	@param exename name of the programme executable
		*	@param long displays the long usage help if it is "true"
		*/
		void cancel_usage(const char* &exename, const bool &long_usg=false) ;
		/**
		*	Displays on the standard output the usage help message for the listmatch command
		*	@param exename name of the programme executable
		*	@param long displays the long usage help if it is "true"
		*/
		void lsmatch_usage(const char* &exename, const bool &long_usg=false) ;
		/**
		*	Displays on the standard output the usage help message for the output command
		*	@param exename name of the programme executable
		*	@param long displays the long usage help if it is "true"
		*/
		void output_usage(const char* &exename, const bool &long_usg=false) ;
		/**
		*	Displays on the standard output the usage help message for the attach command
		*	@param exename name of the programme executable
		*	@param long displays the long usage help if it is "true"
		*/
		void attach_usage(const char* &exename, const bool &long_usg=false) ;
		/**
		*	Displays on the standard output  the usage help message for the delegate-proxy command
		*	@param exename name of the programme executable
		*	@param long displays the long usage help if it is "true"
		*/
		void delegation_usage(const char* &exename, const bool &long_usg=false) ;
		/**
		*	Displays on the standard output  the usage help message for the proxy-info command
		*	@param exename name of the programme executable
		*	@param long displays the long usage help if it is "true"
		*/
		void jobinfo_usage(const char* &exename, const bool &long_usg=false) ;
		/**
		*	Displays on the standard output  the usage help message for the job-perusal command
		*	@param exename name of the programme executable
		*	@param long displays the long usage help if it is "true"
		*/
		void perusal_usage(const char* &exename, const bool &long_usg=false) ;
                /**
                *	Prints on the std output the help usage message for the command
                *	which was being handled and exits from the execution
                */
                void printUsage(const char* exename ) ;
		/**
		* Gets the message string with the version of the UI client
		*/
		static std::string getVersionMessage( );
                /**
                *	Gets a string with the name of the application
                *	@return the string with the name
                */
		std::string getApplicationName() ;
                 /**
                *	Gets a string with the list of the input options
                *	@return the string with the name
                */
		std::string getOptionsInfo() ;
		/***
                *	Gets the level of verbosity for the log info
                *	@return the level
                */
                const int getVerbosityLevel ( );
		/**
		* Returns the minimum size allowed for each file in transfer operations.
		* The computration is based on the limtation fixed by the required File Transfer Protocol and the limitation
		* of the archiving tool (libtar; if zipped feature is allowed).
		* If the protocol is not specified and the zipped feature is not allowed, the default value is
		* returned
		* @param protocol the File Transfer Protocol string (either https or gsiftp ... )
		* @param zipped TRUE if Zipped feature is allowed, FALSE otherwise (the default value)
		* @return the allowed minimum size
		*/
		static const long getMinimumAllowedFileSize (const std::string &protocol, const bool &zipped=false);
                /**
		*	Constants for the help and version messages
		*/
		static const std::string HELP_UI  ;
		static const std::string HELP_VERSION  ;
		static const std::string HELP_COPYRIGHT ;
		static const std::string HELP_EMAIL ;
		/**
		*	"Report-bug" message
		*/
		static const std::string BUG_MSG ;
		/**
		* WMProxy version since the getTransferProtocols is available
		* (see the getVersion service)
		*	version >= 2.2.0
		*/
		static const int WMPROXY_GETPROTOCOLS_VERSION ;
		static const int WMPROXY_GETPROTOCOLS_MINOR_VERSION ;
		static const int WMPROXY_GETPROTOCOLS_SUBMINOR_VERSION ;
                /**
		*	Constants for the verbosity level
		*/
		static const unsigned int DEFAULT_VERBOSITY;
		static const unsigned int MAX_VERBOSITY;
		/**
		* Default protocol for file transfer operations
		*/
		static const std::string TRANSFER_FILES_DEF_PROTO;
		/**
		* Default protocol for file archives and file compression
		*/
		static const std::string JOBPATH_URI_PROTO;
		/**
		* Default protocol for file transfer operations by htcp
		*/
		static const std::string TRANSFER_FILES_HTCP_PROTO;
		/**
		* Default protocol for file transfer operations by globus-url-copy
		*/
		static const std::string TRANSFER_FILES_GUC_PROTO;
		/**
		* LIst of protocol allowed for file transfer operations
		*/
		static const char* TRANSFER_FILES_PROTOCOLS[ ];
		/**
		* Constant string to allow specifing "Retrieve all protocols"
		* when calling WMP services with "protocol" input parameter
		*/
		static const std::string WMP_ALL_PROTOCOLS;
		/**
		* Limitations on File sizes:
		* default max size (bytes) allowed for file transfer
		*/
		static const long MAX_DEFAULT_FILE_SIZE;
		/**
		* Limitations on File sizes:
		* max size (bytes) allowed for tar files
		*/
		static const long MAX_TAR_SIZE ;
		/**
		* Limitations on File sizes:
		* max size (bytes) allowed for globus-url-copy
		*/
		static const long MAX_GUC_SIZE ;
		/**
		* Limitations on File sizes:
		* max size (bytes) allowed for htcp
		*/
		static const long MAX_HTCP_SIZE;
		/**
		* Byte offset for tar files
		*/
		static const long Options::TAR_OFFSET;
	private:
        	/**
                * Gets the default name of the application that is being executed
                */
        	std::string getDefaultApplicationName() ;
		/**
		*	Checks if an option is defined for a specific command (submit, cancel, etc...)
		*	using this class. It checks both long and short option. The check is based on
		*	the fields defined for the long option arrays (submitLongOpts, etc....)
		*	@param the option string to be checked
		*	@return 1 if the option is defined, -1 if the input string doesn't represent a valid option
		*/
		const int checkOpts(const std::string& opt) ;
		/**
		*	Checks if an argument value of the input option doesn't have its first character equal to "-"
		* 	@param opt the long name of the option
		*	@param arg the string with the value of the argument option
		*	@param shortopt the short option character (if defined)
		*	@return the string with the argument value if it is valid
		*/
		const std::string  Options::checkArg(const std::string &opt, const std::string &arg,  const Options::OptsAttributes &code , const std::string &shortopt="" );
		/**
		* Maps the common short option to the correspondent OptsAttributes enumeration code
		* @param opt the short option code
		* @return the OptsAttributes enumeration code
		*/
		const int checkCommonShortOpts (const int &opt);
		/**
		*	Sets the value of the option attribute
		*	@param in_opt code assigned to the option
		*	(the last parameter in the long option struct assigned to each option)
		*	@param command line options
                *	@param msg appends the information message string on the option has been set to this string
		*/
		void setAttribute (const int &in_opt, const char **argv);

                /**
		*	Long options
		*	(no short options defined for this set)
		*/
		static const char* LONG_ALL ;
                static const char* LONG_COLLECTION;
		static const char* LONG_DAG ;
		static const char* LONG_DEBUG ;
		static const char* LONG_DEFJDL ;
		static const char* LONG_DIR ;
		static const char* LONG_FROM ;
		static const char* LONG_GET ;
		static const char* LONG_HELP ;
		static const char* LONG_LISTONLY;
		static const char* LONG_LRMS	;
		static const char* LONG_LOGFILE;
		static const char* LONG_NODESRES;
		static const char* LONG_NODISPLAY;
		static const char* LONG_NOGUI	;
		static const char* LONG_NOINT ;
		static const char* LONG_NOLISTEN ;
		static const char* LONG_NOMSG	;
		static const char* LONG_PROTO	;
		static const char* LONG_RANK ;
                static const char* LONG_REGISTERONLY ;
		static const char* LONG_SET ;
		static const char* LONG_START ;
		static const char* LONG_TO	;
		static const char* LONG_TRANSFER	;
		static const char* LONG_UNSET ;
		static const char* LONG_USERTAG ;
		static const char* LONG_VERSION;
                static const char* LONG_VO;
		static const char* LONG_INPUTFILE ;

		/**
		*	long option std::strings and corresponding
		*	short option characters
		*/
		// automatic delegation
		static const char* LONG_AUTODG ;
		static const char SHORT_AUTODG ;
		// output
		static const char* LONG_OUTPUT ;
		static const char SHORT_OUTPUT ;
		// input
		static const char* LONG_INPUT ;
		static const char SHORT_INPUT;
		// config
		static const char* LONG_CONFIG	;
		static const char SHORT_CONFIG ;
		// filename
		static const char* LONG_FILENAME;
		static const char SHORT_FILENAME ;
		// port - proxy
		static const char* LONG_PROXY	;
		static const char* LONG_PORT ;
		static const char SHORT_P ;
		// resource
		static const char* LONG_RESOURCE ;
		static const char SHORT_RESOURCE ;
		// valid & verbosity
		static const char* LONG_VALID ;
		static const char* LONG_VERBOSE ;
		static const char SHORT_V ;
		// status
		static const char* LONG_STATUS ;
		static const char SHORT_STATUS ;
		// exclude
		static const char* LONG_EXCLUDE ;
		static const char* LONG_ENDPOINT ;
		static const char SHORT_E ;
		// delegation
		static const char* LONG_DELEGATION ;
		static const char SHORT_DELEGATION ;
		// JDL
		static const char* LONG_JDL ;
		static const char* LONG_JDLORIG ;
		static const char SHORT_JDLORIG ;
		// no purger
		static const char* LONG_NOPURG ;
		static const char SHORT_NOPURG ;
		/**
		*	short usage constants
		*/
		static const std::string USG_ALL ;
                static const std::string USG_AUTODG ;
                static const std::string USG_COLLECTION	;
		static const std::string USG_CONFIG	;
		static const std::string USG_DAG ;
		static const std::string USG_DEBUG ;
		static const std::string USG_DEFJDL ;
		static const std::string USG_DIR ;
                static const std::string USG_DELEGATION ;
                static const std::string USG_ENDPOINT ;
		static const std::string USG_EXCLUDE ;
		static const std::string USG_FILENAME;
		static const std::string USG_FROM ;
		static const std::string USG_GET ;
		static const std::string USG_HELP ;
		static const std::string USG_INPUT ;
		static const std::string USG_JDL ;
		static const std::string USG_JDLORIG ;
                static const std::string USG_LISTONLY;
		static const std::string USG_LRMS	;
		static const std::string USG_LOGFILE;
		static const std::string USG_NODESRES	;
		static const std::string USG_NODISPLAY	;
		static const std::string USG_NOGUI	;
		static const std::string USG_NOINT ;
		static const std::string USG_NOLISTEN ;
		static const std::string USG_NOMSG	;
		static const std::string USG_NOPURG ;
		static const std::string USG_OUTPUT ;
		static const std::string USG_PORT ;
		static const std::string USG_PROTO ;
		static const std::string USG_PROXY ;
		static const std::string USG_RANK ;
                static const std::string USG_REGISTERONLY ;
		static const std::string USG_RESOURCE ;
		static const std::string USG_SET ;
		static const std::string USG_START ;
		static const std::string USG_STATUS ;
		static const std::string USG_TO	;
		static const std::string USG_TRANSFER	;
		static const std::string USG_UNSET ;
		static const std::string USG_USERTAG ;
		static const std::string USG_VALID ;
		static const std::string USG_VERBOSE ;
		static const std::string USG_VERBOSITY ;
		static const std::string USG_VERSION;
                static const std::string USG_VO;
		static const std::string USG_INPUTFILE;
		/**
		*	require/not-require char for short option
		*	(used by definition of the short options)
		*/
		static const char short_required_arg ;
		static const char short_no_arg ;
		/**
		*	std::string attributes
		*/
		std::string m_dag ;
		std::string m_def_jdl ;
                std::string m_collection ;
		std::string m_config ;
                std::string m_delegation ;
		std::string m_nodesres;
		std::string m_dir ;
		std::string m_endpoint;
		std::string m_exclude ;
		std::string m_fileprotocol ;
		std::string m_from ;
		std::string m_input ;
		std::string m_lrms ;
		std::string m_logfile ;
		std::string m_output ;
		std::string m_resource ;
		std::string m_start ;
		std::string m_status ;
		std::string m_to ;
		std::string m_valid ;
		std::string m_vo ;
		std::string m_inputfile ;

		/**
		*	boolean attributes
		*/
		bool all ;
                bool autodg ;
		bool debug ;
		bool get ;
		bool help ;
		bool jdl ;
		bool jdlorig ;
                bool listonly ;
		bool nodisplay ;
		bool nogui ;
		bool noint ;
		bool nolisten ;
		bool nomsg ;
		bool nopurg ;
		bool proxy ;
		bool rank ;
                bool registeronly;
		bool unset;
		bool set ;
		bool transfer ;
		bool version ;
		/**
		*	numerical attributes
		*/
		unsigned int *port ;
		unsigned int *verbosity ;
		/**
		*	Listing-attributes
		*/
		std::vector<std::string> usertags;
		std::vector<std::string> filenames;;
		/**
		*	Long options for each command
		*/
		static const struct option submitLongOpts[]  ;
		static const struct option statusLongOpts[]  ;
		static const struct option loginfoLongOpts[]  ;
		static const struct option cancelLongOpts[]  ;
		static const struct option lsmatchLongOpts[]  ;
		static const struct option outputLongOpts[]  ;
		static const struct option attachLongOpts[]  ;
                static const struct option delegationLongOpts[]  ;
		static const struct option jobInfoLongOpts[]  ;
		static const struct option perusalLongOpts[]  ;
		/**
		*	pointer to the long options of the command
		*/
		const struct option *longOpts  ;
		/**
		*	pointer to the short options of the command
		*/
		char* shortOpts  ;

		unsigned int numOpts ;
		/**
		*	type of user command
		*/
		WMPCommands cmdType ;
                /**
                * Application Name
                */
		std::string applName ;
		 /**
                * Input command string
                */
		std::string inCmd ;
		 /**
                * warnMsg
                */
		std::string warnsMsg;
		/**
		*	path of the user JDL file
		*/
		std::string m_jdlFile ;
		/**
		*	jobIds vector
		*/
		std::vector<std::string> jobIds ;
		/**
		* Single JobId
		*	(commands that needed only one id)
		*/
		std::string singleId ;
		/**
                *
                */
                 LogLevel verbosityLevel ;
};
} // glite
} // wms
} // client
} // utilities
#endif

