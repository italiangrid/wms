/**
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
*       Authors:        Alessandro Maraschini <alessandro.maraschini@datamat.it>
*                       Marco Sottilaro <marco.sottilaro@datamat.it>
*/

//      $Id$

#ifndef GLITE_WMS_CLIENT_UTILS
#define GLITE_WMS_CLIENT_UTILS
/**
 * utils.h
 */


// edglog /usr/include/unistd.h compilation problem workaround:
// declaration of `char* crypt(const char*, const char*) throw ()'
// throws different exceptions
#if defined __GLIBC__ && __GLIBC__ == 2 && __GLIBC_MINOR__ == 3
#include <sys/cdefs.h>
#undef __THROW
#define __THROW
#endif


 // std header files
#include <string>
#include <vector>
#include <map>

// wms.client utilities
#include "excman.h"
#include "logman.h"
#include "options_utils.h"
#include "adutils.h"

namespace glite {
namespace wms{
namespace client {
namespace utilities {


class Utils{
public:
	/**
        * Default constructor
        */
	Utils(Options *wmcOpts);
        /**
        * Default destructor
        */
	~Utils( );
	/**
	* Type of Menu
	*/
	enum WmcMenu {
		MENU_JOBID,
		MENU_CE,
		MENU_SINGLECE,
		MENU_SINGLEJOBID,
		MENU_FILE,
		MENU_SINGLEFILE,
	};
	enum WmcInputType{
		JOBID_TYPE,
		CE_TYPE
	};
	/**Service Discovery Service available types*/
	enum SdServiceType{
		/** Logging and bookkeeping server - service type*/
		LB_SD_TYPE,
		/** WMProxy endpoint - service type*/
		WMP_SD_TYPE
	};


	/**
	* Prompt the users (if --noint option is not active) a question with Y/N answer
	* @param question the question to be prompted to the user
	* @param defaultAnswer the default answer if enter is typed (true= "YES")
	* @param defaultValue default value returned in case of interaction is not allowed (--noint option)
	* @return the answer value or the default value in case of interaction is not allowed
	*/
	bool answerYes (const std::string& question, bool defaultAnswer=false, bool defaultValue=false);
	/**
	* Prompt the users if they want to overwrite a file if it already exists
	* @param path the pathname of the file
	* @return TRUE if the file can be saved to the indicated path (if it doesn't exists or overwriting is allowed), FALSE otherwise
	*/
	bool askForFileOverwriting(const std::string &path);
	/**
	* Asks users to chose either all or some items in a list; the question is printed on the std-output;
        * users insert their reply on the the std-input
	* @param jobids a vector contatining the list of jobids in which perform the choice
	* @param type of menu
	* @return the list of chosen items
	*/
        std::vector<std::string> askMenu(std::vector<std::string> &items, const enum WmcMenu &type);
	/**
	* Extrapolate the list of possible LB address associated with the specified NS address index (or all when no index specified)
	* @param lbGroup a map of all the LB available. Each vector contains the LBs for a certain NS
	* @param nsNum the NetworkServer address to be referred to whn extrapolating
	*/
	std::vector<std::string> getLbs(const std::vector<std::vector<std::string> >& lbGroup,int nsNum=-1);
        /**
        *  generates a random number included in the range[min,max]
        * @param min the minimum value of the range
        * @param max the maxium value of the range
        * @return the generated number
        */
        static const int getRandom (const unsigned int &max );
	/**
	*  Retrieves the client installation path
        * @return the installation path string representation
        */
        std::string getPrefix();
	/**
	* Gets the extension used for archive files
	* @return the file extension string
	*/
	static std::string getArchiveExtension( ) ;
	/**
	* Gets the extension used for compressed files
	* @return the file extension string
	*/
	static std::string getZipExtension( ) ;
	/**
	* Extracts from the input string the name of the archive filename according to
	* the default archive extension
	* @return the name of the archive file or an empty string if the input name is invalid
	*
	*/
	static std::string getArchiveFilename (const std::string file);
        /**
	* Extrapolate the list of possible WMProxy addresses
	* @return  a vector of strings containg the list of addresses
	*/
	std::vector<std::string> getWmps( );
        /**
	* Query the service discovery for published WMProxy enpoints
	* @param the VirtualOrganisation to be queried for
	* @return  a vector of strings containg the list of found addresses
	*/
	std::vector<std::string> lookForServiceType(SdServiceType st, const std::string &vo="");
        /**
        * Gets the ErrorStorage pathname
        * @return the pathname string
        */
        const std::string getErrorStorage( );
        /**
        * Gets the OuputStorage pathname
        * @return the pathname string
        */
        std::string getOutputStorage( );
	/**
        * Gets the log file pathname
        * @return the pathname string
        */
        std::string getLogFileName(void);
	/**
	* Match the most relevant HTTP status codes with the corresponding error message strings
	* (some of them are: 400, 401, 404, 407, 408, 414, 500, 501, 505)
	* @param code http status code
	* @return the string with the error message, or empty string if no matching has been found
	*/
	static std::string httpErrorMessage(const int &code);
	/**
        * Gets the conf pathname
        * @return the pathname string
        */
        glite::jdl::Ad* getConf(){return wmcConf;}
	/**
	* Resolves an hostname, supposed to be an alias, into its CNAME.
	* @param hostname the hostanem to resolve.
	* @param resolved to be filled with DNS record A host entry.
	* @return whether there has been an error (true) or success (false)
	*/
	void resolveHost(const std::string& hostname, std::string& resolved);
	/**
	* Check the format of a jobid string
	* the format is <protocol>://<lb host>:<lb port>/<unique_string>
	* @param jobid jobid string to be checked
        * @return the jobid string
        *@throw WmsClientException in case of bad format
	*/
        static std::string checkJobId(std::string jobid);
	/**
	* Check the format of a list of jobids
	* the format is <protocol>://<lb host>:<lb port>/<unique_string>
        * In case the list contains some bad id's, question whether the user want to continue is asked
        *@param jobids the list of input jobid's (at the end of execution, bad format id's has been removed);
	*@return the list of the rights jobid's
	*/
	std::vector<std::string>  checkJobIds(std::vector<std::string> &jobids);
	/**
	* Check the format of a jobid string
	* the format is <protocol>://<lb host>:<lb port>/<unique_string>
	* @param jobid jobid string to be checked
        * @return the jobid string
        *@throw WmsClientException in case of bad format
	*/
        static void checkResource(const std::string& resource);
	/**
	* Check the format of a list of jobids
	* the format is <protocol>://<lb host>:<lb port>/<unique_string>
        * In case the list contains some bad id's, question whether the user want to continue is asked
        *@param jobids the list of input jobid's (at the end of execution, bad format id's has been removed);
	*@return the list of the rights jobid's
	*/
	std::vector<std::string> checkResources(std::vector<std::string> &resources);
	/**
	* Get the unique string of the input JobId
	* @param jobid the input jobid
        * @return the unique string
        *@throw WmsClientException in case of bad format
	*/
	static std::string getUnique(std::string jobid);
	/**
	* Get a string with the list of the items
	* @param items vector with the items to put in the string list
	* @return the string with the list of input items
	*/
	static const std::string getList (const std::vector<std::string> &items);

        /**
	* Build a char stripe
	* @param len the length of the stripe to be built
        * @param ch the character to be used for the stripe
        * @param msg the message that can be put in the middle
        * @return the built stripe
	*/
        const std::string getStripe (const int &len, const std::string &ch, const std::string &msg = "");
	/**
	* Exit from the process. If ncessary prompt some information
	* @param exitCode command exit code, 0 in case of success, error otherwise
	*/
	static void ending(unsigned int exitCode=0);
	/**
	*  Convert the input time string to the number of seconds since 1st of January 1970
        * and if a type of option is specified, check if it is a valid value for that option
        * (is later or earlier)
	* the formats of the input time string could be:
	*	- MM<sep>DD<sep>hh<sep>mm<sep>YYYY
	*	- MM<sep>DD<sep>hh<sep>mm
	*	- hh<sep>mm
	* where MM=month, DD=day of the month, YYYY=year hh=hours , min=minutes
	* @st the input time string
	* @opts type of time option
	* @return the number of seconds since 1st of January 1970
	*/
	static const long checkTime ( const std::string &st, int &days,  int &hours, int &minutes, const Options::TimeOpts &opt = Options::TIME_NO_OPT);
	/**
	* generate a unique string
        * @return the generated string
	*/
        static std::string getUniqueString(void);
        /**
        * Gets the virtualOrganisation associated to this option
	* @return the point to the virtualOrganisation string
        */
        std::string getVirtualOrganisation(void){return virtualOrganisation;}
        /**
        * checks if a pathname is a valid file
        *@param pathname the pathname that has to be checked
        *@return true if the pathname is a valid file (false, otherwise)
        */
        static const bool isFile (const std::string &pathname);
        /**
        * checks if a pathname is a valid directory
        *@param pathname the pathname that has to be checked
        *@return true if the pathname is a valid directory (false, otherwise)
        */
        static const bool isDirectory (const std::string &pathname);
        /**
        * gets the absolute path of the file
        * @param file the file
        * @return the absolute pathname
        */
        static const std::string getAbsolutePath(const std::string &file );
	/**
        *  Gets the size of a file (in bytes)
        * @param path the pathanme of the file
        * @return the number of bytes in the file
        * @throw WmsClientException if the file doesn't exist or in case it is not possible to open the file
        */
        static int getFileSize (const std::string &path) ;
	/**
	* Removes a file
	* @param file the pathname of the file to be removed
	*@throw WmsClientException if an error occurred during the operation
	*/
	static void removeFile(const std::string &file) ;
	/**
        * Reads <nbytes> bytes from a file; if nbytes is set to the default value (=0),
        * the entire text of the file is read
        * @param path the pathanme of the file
        * @return the text read from the file
        * @throw WmsClientException if the file doesn't exist or in case of any error occured during the reading operations
        */
        std::string fromFile (const std::string &path) ;
	/**
	* Saves a message into a file after checking whether it already exists.
	* If it does and user interaction has not been disabled (by --noint),
	* the user is asked which action has to be performed
	* (either overwriting or appending ; otherwise nothing)
        * @param path the pathname of the file
	* @param msg the message to be saved
	* @return 0 in case of success (-1 otherwise)
	*/
	int saveToFile(const std::string &path, const std::string &msg) ;
	/**
        * Saves a list of items in the specified file
        * @param path the pathname of the file
	* @param list the vector containing the list to be saved
	* @param header the string to be saved at the beginning of the file
        * @return 0 if the list has been successfully saved (-1 otherwise)
        */
	 const int saveListToFile (const std::string &path, const std::vector<std::string> &list, const std::string header="");
	/**
        * Saves the identifier of a submitted job in the specified file
        * @param path the pathname of the file
	* @param jobid the job identifier string
        * @return 0 if the jobid has been successfully saved (-1 otherwise)
        */
         const int saveJobIdToFile (const std::string &path, const std::string jobid, std::string failed = "");
        /**
        * Reads a file and gets the list of items contained in it
        * @param the pathname of the file to be read
        * @return the vector containg the list of items
        * @throw WmsClientException if the file doesn't exist or in case of any error occured during the reading operations
        */
        std::vector<std::string> getItemsFromFile (const std::string &path);
        /**
	* Adds the star (*) wildcard to the input pathname
        * @param path the input pathname
        * @return the modified pathname with the wildcard
	*/
	static const std::string addStarWildCard2Path(std::string& path);
 	/**
 	* Removes '/' characters at the end of the of the input pathname
        * @param fpath the input pathname
        * @return the normalized pathname
 	*/
	 static const std::string normalizePath( const std::string &fpath ) ;
	/**
	* Removes the file protocol string at the beginning of the path if it is present
	* @param fpath the input pathname
        * @return the normalized pathname
	*/
	 static const std::string normalizeFile( const std::string &fpath ) ;
	 /**
         * removes white spaces form the begininng and from the end of the input string
         * @param str the input string
         * @return the string without white spaces
         */
         static const std::string cleanString(std::string str);
         /**
	* Converts the input integer to a string, adding a "zero"-digit ahead if it has one digit ( 0< d < 9)
	*/
	static std::string twoDigits(unsigned int d );
	/**
	*	Extracts the filename from the input path (checking if the file exists on the local machine)
	*	@param path the input file pathname
	*	@return the name of the file
	*/
	static std::string  getFileName (const std::string& path) ;
	/**
	* Returns a string message with the zlib error description
	* according to the input error code
	*
	*/
	static std::string gzError(int code) ;
	/**
	*	Performs gz compression on the file which path location
	* 	is specified as input
	*	@param source the path of the file to be compressed
	*	@param level level of compression
	*	@return Z_OK if the operation successfully ends, otherwise the zlib error code
	*/
	static int compression(FILE *source, FILE *dest, int level) ;
	/**
	*	Compresses a file with gzip. Method to be used only with WMProxy having version
	*	greater than 2.1.1 (the version numbers that the getVersion service returns).
	*	@param filename the pathname of the file to be compressed
	*	@param the path of the gzip file
	*/
	std::string fileCompression(const std::string &filename) ;
	/**
	*	Extracts the protocol from the input URI
	*	throws an exception if the input URI doesn't have protocol string
	*	@param path the input URI
	*	@return the string with the protocol
	*/
	static std::string  getProtocol (const std::string& uri) ;
	/**
	*	Gets back the path of the URI removing information on the protocol, hostname and port
	*	@param uri the input URI
	*	@return the string with the absolute path extracted
	*/
 	static std::string getAbsolutePathFromURI (const std::string& uri) ;
	/**
	* Checks if a vector of strings contains a string item
	* @param vect the vector to be checked
	* @param item the string to be looked for
	*@return TRUE if the vector contains the string item, FALSE otherwise
	*/
	static bool hasElement (const std::vector<std::string> &vect, std::string item);
	 /**
        *	header for JobId output files
        */
        static const std::string JOBID_FILE_HEADER ;
	static const std::string JOBID_FAILED_HEADER ;
	/**
	* Returns the Default VO retrieved by the user proxy
	* @return the Default VO string
	*/
	static std::string getDefaultVo() ;	
        /**
        * Forks the process and executes command line
        * @command the command to execute
        * @params the params of the command line
        * @errormsg the error message in case of failure
	* @return the exit code of the command
        */
        int doExecv(const std::string &command, std::vector<std::string> &params, std::string &errormsg, const int &delay);
        /**
        * Gets a relative path and extracts the IP address of the host
        * @relpath the relative path
        * @return the IP address of the host
        */
        std::string resolveAddress(std::string relpath) ;
private:
	/**
	* Performs parsing on the FQAN fields an retrurns a vector which elements
	* are the single fields
	* @param fqan the FQAN string to be parsed
	* @return the list of fields in the input FQAN string
	*/
	static std::vector<std::string> parseFQAN(const std::string &fqan);
	/**
	* Returns the VO in the FQAN input string
	* @return the string with the VO
	*/
	static std::string FQANtoVO(const std::string fqan);
	/**
	* Check the WMS client installation path
	*/
	void checkPrefix();
	/**
	* Check the WMS client configuration file
	@return definite VirtualOrganisation found value
	*/
	std::string checkConf();
	/** Look for possible configuration file */
	/**
	*	convert the input time string to number of seconds form 1970
	*	the formats of the input time string could be:
	*		- MM<sep>DD<sep>hh<sep>mm<sep>YYYY
	*		- MM<sep>DD<sep>hh<sep>mm
	*		- hh<sep>mm
	*	where MM=month, DD=day of the month, YYYY=year
	*		  hh=hours , min=minutes
	*	(check also if the number of fields is equal to the nf parameter)
	*	@st the input time string
	*	@sep field separator
	*	@now the current time
	*	@nf number of the fields expected in the input string
	*	@return the number of seconds
	*	@throw ---------------------------------- in case any format error in the input string
	*/
	static const long getTime(const std::string &st, const std::string &sep, const time_t &now, const unsigned int &nf = 0) ;
	/**
        * Saves the input message into a file
        * @param path the pathname of the file
	* @param msg the message to be saved
        * @param append TRUE=if the file already exists, append the input message at the end of the file; FALSE=create the file if it does not exist, otherwise overwrite it
        * @return 0 in case of success (-1 otherwise)
        */
         int toFile (const std::string &path, const std::string &msg, const bool &append=false);
	 /**
        * Generates the log file.
	* The file is created only if either --debug or --logfile has been spevified among the user input options.
        * @return a pointer to a string with the pathname, NULL if no logfile has been generated
        */
        std::string generateLogFile ( );
        /**
        * Gets the default Log File pathname
        * /<OutStorage-path>/<commandname>_<UID>_<PID>_<timestamp>.log
        */
        const std::string getDefaultLog ( );
        /**
	*	extract the tokens from the input string by a given separator
	*	<field_1><separator><fields_2>....... etc
	*	@param  instr input string
	*	@sep string separator between the tokens
	*	@return a vector of strings with the extracted tokens
	*/
	static const std::vector<std::string> extractFields(const std::string &instr, const std::string &sep);
        /**
        * perforns the base64 enconding of the input object
        *@param enc the object to be encoded
        *@param enc_size encoding size
        *@param out output string
        *@param out_max_size max size of the output string
        *@return the size of the output string, -1 in case of error during the encoding
        */
        static const int base64Encoder(void *enc, int enc_size, char *out,  int out_max_size);
	/**
        * converts the input string to a md5base64 string
        * @param the input string
        * @return the result of the conversion
        */
 	static  const char *str2md5Base64(const char *s);
	// Ad configuration files:
	glite::jdl::Ad *wmcConf;
	// Option files:
	Options *wmcOpts;
        // Ad utilities
        AdUtils *wmcAd;
        // debug flag (from options)
        bool debugInfo;
        // verbosity level
        LogLevel vbLevel ;
        //Log file pathname
        Log* logInfo ;
	// General configuration inner values
	std::string prefix;
	// Virutal Organisation value
	std::string virtualOrganisation;
	

}; // end class definition

} // glite
} // wms
} // client
} // utilities
#endif
