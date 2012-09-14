/**
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
* 	Authors:	Alessandro Maraschini <alessandro.maraschini@datamat.it>
* 			Marco Sottilaro <marco.sottilaro@datamat.it>
*
*/

// 	$Id$

#ifndef GLITE_WMS_CLIENT_SERVICES_JOBSUBMIT_H
#define GLITE_WMS_CLIENT_SERVICES_JOBSUBMIT_H

#include "listener.h"
// inheritance
#include "job.h"
// options utilities
#include "utilities/options_utils.h"
#include "utilities/utils.h"
#include "utilities/logman.h"
// wmproxy API
#include "glite/wms/wmproxyapi/wmproxy_api.h"
// Ad's
#include "glite/jdl/Ad.h"
#include "glite/jdl/ExpDagAd.h"
#include "glite/jdl/collectionad.h"
#include "glite/jdl/extractfiles.h"

namespace glite {
namespace wms {
namespace client {
namespace services {

/**
* Type of Jobs
*/
enum wmsJobType {
	WMS_JOB, //normal
        WMS_DAG,
        WMS_COLLECTION,
        WMS_PARAMETRIC
};



struct JobFileAd {
	/** Default constructor */
	JobFileAd( );
	/** JobId Attribute */
	std::string jobid;
	/** NodeName Attribute */
	std::string node;
	/** FIleList Attribute */
	std::vector<glite::jdl::FileAd> files;
};

struct ZipFileAd {
	/** Default constructor */
	ZipFileAd( );
	/** Zip FileName Attribute */
	std::string filename ;
	/** FIleList Attribute */
	std::vector<JobFileAd> fileads;
};



class JobSubmit : public Job {
	public :
		/*
		*	Default constructor
		*/
		JobSubmit ( );
                /*
		*	Default destructor
		*/
		~JobSubmit ( );
		/*
		*	Processes the command-line user arguments
		*	@param argc number of the input arguments
		*	@param argv string of the input arguments
		*/
		void readOptions (int argc,char **argv) ;
                /*
		*	Performs the operations to submit the job(s)
		*/
		void submission ( ) ;
	private:
		/*
		* Returns the type of job is being submitted
		*/
		const wmsJobType getJobType( );
		/**
                 *	Contacts the server in order to retrieve the list of all destionationURI's of the job (with the available protocols).
		 *  	In case of compound jobs (DAG, collections etc..), it also retrieves the URIs of the nodes.
		 * 	The request is done using the JobId. Compound jobs are identified by the parent's JobId.
		 * 	The method gets back the URI's of a child node if its jobid is specified for the input parameter "child"; in case of empty string for it,
		 *	the URI's are referred to the parent node.
                 *	@param jobid the string with the JobId
		 *	@param child the string with the child JobId
		 *	@param zipURI this parameter returns the DestinationURI associated to the default protocol for ZIP files (according to the value of Options::DESTURI_ZIP_PROTO)
                 *	@return a pointer to the string with the destinationURI having the protocol that will be used for any file transfer
		 *	(or NULL in case the destinationURI with the chosen protocol is not available )
                */
	    	std::string getDestinationURI(const std::string &jobid, const std::string &child="", const std::string &protocol="") ;
		/**
		* Checks whether the user JDL contains InputSandbox files located on the local machine to be transferred to
		* the server.
		* If there are local files referenced in the InputSandbox, the total size of these files is compared to the limitations that
		* can be set  :
		*	- limitation on the file size allowed by tools used for the file transfer operations (globus-url-copy, htcp)
		* 	- the first limitation is the available User Free Quota on the server (the remaining free part of available user disk quota)
		* 	- the second one is the max InputSanbox size on the server
		* The max InputSanbox size is checked only if theUser Free Quota has been set. The submission operations go on
		* if eitheir the total size of the file doesn't exceed the fixed limitation or no limitation has been set on the server.
		* This method takes care of contacting the server to retrieve the information on these limitations.
		* (according to the type of the job) and gets back the total size (in bytes) of these files.
		* @return the total size of the local files
		*/
		int checkInputSandbox ( );
		/**
		* The composition of the ISB zipped files is stored
		* in the memory in order to be used after the registration/submission
		* of the job, when the zipped files are phisically prepared
		*/
		void toBCopiedZippedFileList( ) ;
		/**
		* The input vector is filled with the information related to the local user files that are
		* in the JDL InputSandbox attribute and needed to be transferred to the WMProxy server.
		* If file compression is allowed, all the user files are collected into zipped files.
		* The vector is composed by a list of pairs:
		* the first element is a structure containing the information on each local file (pathname, size, file protocol;etc);
		* the second one is a string representing the URI where the file will be transferred to.
		* @param tob_transferred the vector to be filled with
		*/
		void toBCopiedFileList( std::vector<std::pair<glite::jdl::FileAd, std::string > > &tob_transferred) ;
		/**
		* Checks whether the total size of local files in the user InputSandbox is compatible with
		* the limitation that could be set on the server
		* (UserFreeQuota and max InputSandbox size)
		* A WmsClientException is thrown if the total ISB size exceeds one of these limitations
		*/
		void checkUserServerQuota() ;
		/**
		* Returns a relative path that is used to archive the ISB local file in the tar files.
		* This relative path is related to the job node which JDL name is provided as input parameter.
		* Job Path of the root node is returned, calling this method with an empty string as input.
		* Since WMProxy version 2.2.0, this information is contained
		* in one of the structure field returned by the call to JobRegister/JobSubmit.
		* Calling a server with an earlier version, this information is computed
		* form this client starting from the DestionationURi information
		* (@seeJobSubmit::getJobPathFromDestURI)
		* @param node the node name of the child node
		*/
		std::string getJobPath(const std::string& node) ;
		/**
		* Returns a relative path that is used to archive the ISB local file in the tar files.
		* This path is based on the job DestionationURI with the input protocol
		* @param jobid the JobId string of either the root node or one of the child nodes
		* @param protocol the protocol of the URI from which the path is computed
		* @return The computed job relative path
		*/
		std::string getJobPathFromDestURI(const std::string& jobid, const std::string& protocol);
		/**
		* Perfoms the post processing of the job that has been just registered to the
		* WMProxy server. If the user JDL InputSandbox contains file son the local machine that
		* have to be moved to the server, file transfer operations are performed.
		* If user has requested not to perform any file transfer, a list of files, that needed to
		* be transferred to in order to complete the submission, is printed out.
		*/
		void jobPostProcessing( ) ;
		/**
		* Returns the jobid string
		*/
		std::string getJobId( );
		/**
		* Retrieve JobId from a specified node name
		*/
		std::string getJobIdFromNode(const std::string& node);
		/*
                * Performs either registration or submission.
		* The WMProxy submission service, that performs both registration and start of the job, is invoked
		* if no local files are referenced in the JDL InputSandbox. If there are files that needed
		* to be transferred to the server, only the jobRegistration is performed.
		* After the file transfer, the jobStarter method will be called.
		* @param submit perform submission (true) or registration (false)
                */
		void jobRegOrSub(const bool &submit);

		/*
		* Performs job starting
		* @param jobid the identifier of the job to be started
		*/
		void jobStarter(const std::string &jobid);
		/*
		* Collects a group of files into one or more tar files which are gzip compressed.
		* The number of archives depends on the size limit of tar archives.
		* @param to_bcopied the list of files to be archived that will be copied to the WMProxy server
		* @param destURI the destinationURI of the job where the gzip file has to be transferred
		 */
		void createZipFile (std::string filename, std::vector<JobFileAd> fileads, std::vector<pair<glite::jdl::FileAd, std::string > > &to_btransferred);
		/*
		* 	Performs the transfer a set of local files to one(more) remote machine(s) by globus-url-copy (gsiftp protocol)
 		*	@param paths list of files to be transferred (each pair is <source,destination>)
		*	@param failed this vector is filled with the information on the files for which was a failure occurred during the transfer operations
		*	@param errros this parameter is filled with the description of the errors occuring during the file transfer opertions (if any)
                *	@throw WmsClientException if any error occurs during the operations
                *	(the local file doesn't exists, defective credential, errors on remote machine)
		*/
		void gsiFtpTransfer(std::vector <std::pair<glite::jdl::FileAd, std::string> > &paths,std::vector <std::pair<glite::jdl::FileAd, std::string> > &failed, std::string &errors);
                /*
                * 	Performs the transfer a set of local files to one(more) remote machine(s) by htcp (https protocol)
                *	@param paths list of files to be transferred (each pair is <source,destination>)
		*	@param failed this vector is filled with the information on the files for which was a failure occurred during the transfer operations
		*	@param errros this parameter is filled with the description of the errors occuring during the file transfer opertions (if any)
                *	@throw WmsClientException if any error occurs during the operations
                *	(the local file doesn't exists, defective credential, errors on remote machine)
                */
                void htcpTransfer (std::vector <std::pair<glite::jdl::FileAd, std::string> > &paths, std::vector <std::pair<glite::jdl::FileAd, std::string> > &failed, std::string &errors);
		/*
		*	Gets the list of the InputSandbox files to be transferred to the DestinationURI's
		*	@param paths the list of files that still need to be transferred
		*	@param jobid the identifier of the job
		*	@param zip if TRUE, creates tar.gz file if file compression is allowed
		*	@return  the message with the list of the files in the input vector 'paths'
		*/
		std::string transferFilesList(const std::vector <std::pair<glite::jdl::FileAd, std::string> > &paths, const std::string& jobid, const bool &zip=true) ;
		/**
		* Uploads the local files in the InputSandbox to the WMProxy server. If file compression is allowed, all files are collected
		* in one or more tar file archives which are gzip compressed. The number of archives depends on the size limit of
		* tar archives. If compression is not allowed, each file is separately uploaded in succession.
		* The transfer is performed using either the defined default protocol (Options::TRANSFER_FILES_DEF_PROTO)
		* or the protocol chosen by the --proto option.
		* On the base of this choice, the specific "xxxxxTransfer" function is called.
		* @paths a vector containing the list of local files and for each file the relative destinationURI information
		* (to be used in case of single file transfer or as relative path in the creation of the tar archive)
		* @destURI a string with the destinationURI where to transfer the tar archives
		*/
		 void transferFiles(std::vector<std::pair<glite::jdl::FileAd,std::string > > &to_bcopied, const std::string &jobid);
		/**
		*  Reads the results of the registration operation for a job represented as a DAG  from the JobIdApi structure.
		* This method is only executed if the JDL InputSandbox contains local files that need
		* to be transferred to the WMProxy server.
		* The transfer of the files is performed if this operation has been selected.
		* In case of  it has not been requested (and the operation ends with the only registration),
		* an info message with the list these file is provided.
		*/
                std::string normalJob();
		/**
		*  Reads the results of the registration operation for a job represented as a DAG  from the JobIdApi structure.
		* This method is only executed if the JDL InputSandbox contains local files that need
		* to be transferred to the WMProxy server.
		* The transfer of the files is performed if this operation has been selected.
		* In case of  it has not been requested (and the operation ends with the only registration),
		* an info message with the list these file is provided.
		*/
                std::string dagJob( );
		/**
		*  Reads the results of the registration operation for a collection from the JobIdApi structure.
		* This method is only executed if the JDL InputSandbox contains local files that need
		* to be transferred to the WMProxy server.
		* The transfer of the files is performed if this operation has been selected.
		* In case of  it has not been requested (and the operation ends with the only registration),
		* an info message with the list these file is provided.
		*/
                std::string collectionJob();
		/**
		* Checks the user JDL
		*@param filestoBtransferred whether the ad has any file to be transferred (true) or not(false)
		*/
		void JobSubmit::checkAd(bool &filestoBtransferred);
		/** Failover approach: if a service call fails the client may recover
		from the reached point contacting another wmproxy endpoint */
		enum submitRecoveryStep {
			STEP_CHECK_US_QUOTA,
			STEP_REGISTER,
			STEP_SUBMIT_ALL
		};
		/** FailOver Approach: when a problem occurred, recover from a certain step
		* @param step where the submission arrived so far
		*/
		void submitRecoverStep(submitRecoveryStep step);
		/** FailOver Approach: Perform a desired step */
		void submitPerformStep(submitRecoveryStep step);
		/**
		*	String input arguments
		*/
		std::string m_collectOpt;
		std::string m_dagOpt ;
		std::string m_defJdlOpt ;
		std::string m_lrmsOpt ;
		std::string m_toOpt ;
		std::string m_inOpt ;
		std::string m_resourceOpt ;
		std::string m_nodesresOpt ;
		std::string m_validOpt ;
		std::string m_startOpt ;
		/**
                *	Boolean input arguments
                */
		bool nomsgOpt ;
		bool nolistenOpt ;
                bool registerOnly;
		bool startJob;
		bool toBretrieved; // indicate whether there are files to be retrieved
		/** Final Warning Message*/
		std::string infoMsg;
		/** Time variable*/
		long expireTime ;
		/** Sandbox Size variable */
		long isbSize;
		/** Max Single Job Size variable */
		long maxJobIsbSize;
                /**
                * JobId's
                */
                glite::wms::wmproxyapi::JobIdApi jobIds ;
		/**
                *	Ad-objects
		*/
                glite::jdl::Ad *adObj ;
                glite::jdl::JobAd *jobAd ;
		glite::jdl::JobAd jobAdSP;
		glite::jdl::ExpDagAd *dagAd  ;
        	glite::jdl::CollectionAd *collectAd ;
		glite::jdl::ExtractedAd *extractAd ;
		/**
                * JobShadow for interactive jobs
                */
                Shadow *jobShadow ;
		/**
		*	path to the JDL file
		*/
		std::string m_jdlFile ;
		/**
		*	string of the user JDL
		*/
		std::string m_jdlString ;

		/**
		* The name of the ISB tar/zip files
		*/
		std::vector<ZipFileAd> zippedFiles;
		/**
		*
		*/
		bool zipAllowed ;
		/**
		* List of Destination URI's
		*/
		std::vector< std::pair<std::string ,std::vector<std::string > > > dsURIs ;
		/**
		* Type of job  is being submitted
		*/
		wmsJobType jobType ;
};
}}}} // ending namespaces
#endif //GLITE_WMS_CLIENT_SERVICES_JOBSUBMIT_H
