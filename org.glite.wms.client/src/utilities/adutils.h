/**
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
*       Authors:        Alessandro Maraschini <alessandro.maraschini@datamat.it>
*                       Marco Sottilaro <marco.sottilaro@datamat.it>
*/

//      $Id$

#ifndef GLITE_WMS_CLIENT_UTILS_ADUTILS_H
#define GLITE_WMS_CLIENT_UTILS_ADUTILS_H

#include <string>
#include "options_utils.h"
#include "logman.h"
#include "glite/jdl/Ad.h"


/*
 * adutils.h
 */
 
namespace classad{
	class ClassAd;
}
namespace glite {
namespace jdl { class Ad; class JobAd; class ExpDagAd;class CollectionAd; }
namespace wmsutils { namespace jobid { class JobId; } }
namespace wms{
namespace client {
namespace utilities {

enum voSrc {
	NONE,
	CERT_EXTENSION, //  only vo
	VO_OPT,		//  only vo
	CONFIG_OPT,	//  vo File
	CONFIG_VAR,	//  vo File
	JDL_FILE	//  only vo
};

const std::string JDL_WMPROXY_ENDPOINT					= "WmProxyEndPoints";
const std::string JDL_WMPROXY_SERVICE_DISCOVERY_TYPE			= "WMProxyServiceDiscoveryType";
const std::string JDL_ERROR_STORAGE					= "ErrorStorage";
const std::string JDL_OUTPUT_STORAGE					= "OutputStorage";
const std::string JDL_LB_ENDPOINT          				= "LBEndPoints";

const std::string JDL_DEFAULT_ATTRIBUTES				= "JdlDefaultAttributes"; 
const std::string JDL_SOAP_TIMEOUTS					= "SoapTimeouts";
const std::string JDL_DEFAULT_PROXY_VALIDITY				= "DefaultProxyValidity";
const std::string JDL_ENABLE_SERVICE_DISCOVERY				= "EnableServiceDiscovery";
const std::string JDL_SYSTEM_CALL_TIMEOUT				= "SystemCallTimeout";
const std::string DELEGATION_ID							= "DelegationId";
	
const std::string SOAP_GLOBAL_TIMEOUT          				= "globalTimeout";
const std::string SOAP_GET_VERSION_TIMEOUT     				= "getVersionTimeout";
const std::string SOAP_JOB_LIST_MATCH_TIMEOUT				= "jobListMatchTimeout";
const std::string SOAP_JOB_SUBMIT_TIMEOUT				= "jobSubmitTimeout";
const std::string SOAP_JOB_REGISTER_TIMEOUT				= "jobRegisterTimeout";
const std::string SOAP_JOB_START_TIMEOUT				= "jobStartTimeout";
const std::string SOAP_JOB_CANCEL_TIMEOUT				= "jobCancelTimeout";
const std::string SOAP_JOB_PURGE_TIMEOUT				= "jobPurgeTimeout";
const std::string SOAP_GET_OUTPUT_FILE_LIST_TIMEOUT			= "getOutputFileListTimeout";
const std::string SOAP_GET_SANDBOX_DEST_URI_TIMEOUT			= "getSandboxDestURITimeout";
const std::string SOAP_GET_SANDBOX_BULK_DEST_URI_TIMEOUT		= "getSandboxBulkDestURITimeout";
const std::string SOAP_GET_MAX_INPUT_SANBOX_SIZE_TIMEOUT  		= "getMaxInputSandboxTimeout";
const std::string SOAP_GET_FREE_QUOTA_TIMEOUT				= "getFreeQuotaTimeout";
const std::string SOAP_GET_STRING_PARAMETRIC_JOB_TEMPLATE_TIMEOUT	= "getStringParametricJobTemplateTimeout";
const std::string SOAP_GET_TRANSFER_PROTOCOLS_TIMEOUT			= "getTransferProtocolsTimeout";
const std::string SOAP_GET_TOTAL_QUOTA_TIMEOUT				= "getTotalQuotaTimeout";
const std::string SOAP_GET_JDL_TIMEOUT					= "getJDLTimeout";
const std::string SOAP_GET_PROXY_REQ_TIMEOUT				= "getProxyReqTimeout";
const std::string SOAP_PUT_PROXY_TIMEOUT				= "putProxyTimeout";
const std::string SOAP_GET_DELEGATED_PROXY_INFO_TIMEOUT			= "getDelegatedProxyInfoTimeout";
const std::string SOAP_GET_JOB_PROXY_INFO_TIMEOUT			= "getJobProxyInfoTimeout";
const std::string SOAP_ENABLE_FILE_PERUSAL_TIMEOUT			= "enableFilePerusalTimeout";
const std::string SOAP_GET_PERUSAL_FILES_TIMEOUT			= "getPerusalFilesTimeout";
const std::string SOAP_GET_JOB_TEMPLATE_TIMEOUT				= "getJobTemplateTimeout";
const std::string SOAP_GET_DAG_TEMPLATE_TIMEOUT				= "getDAGTemplateTimeout";
const std::string SOAP_GET_COLLECTION_TEMPLATE_TIMEOUT			= "getCollectionTemplateTimeout";
const std::string SOAP_GET_INT_PARAMETRIC_JOB_TEMPLATE_TIMEOUT		= "getIntParametricJobTemplateTimeout";
		
// Default values
const std::string DEFAULT_LBCLIENT_SERVICE_TYPE		= "org.glite.lb.server";
const std::string DEFAULT_WMPROXY_SERVICE_TYPE 		= "org.glite.wms.WMProxy";
const std::string DEFAULT_ERROR_STORAGE			= "/var/tmp";
const std::string DEFAULT_OUTPUT_STORAGE		= "/tmp";
	
#define C_CONFIG_ATTRIBUTES 14

const std::string configuAttributes[C_CONFIG_ATTRIBUTES] = {
				"DefaultProxyValidity",
			        "DefaultStatusLevel",
				"DefaultLoggingLevel",
				"ErrorStorage",
				"OutputStorage",
				"ListenerStorage",
				"LBServiceDiscoveryType",
				"WMProxyServiceDiscoveryType",
				"EnableServiceDiscovery",
				"WmProxyEndPoints",
				"JdlDefaultAttributes",
				"SoapTimeouts",
				"SystemCallTimeout",
				"DelegationId"};
		
class AdUtils{
	public:
	
		/*
		*Default constructor
		*/
		AdUtils(Options *wmcOpts);
		/*
		*Default destructor
		*/
		~AdUtils( );
		
		/**
		* parse the specified files and override the default Ad with the user custom entries
		*@param pathUser poitning to the user configuration file
		*@param pathDefault poitning to the default configuration file
		*@return the merged Ad
		*/
		glite::jdl::Ad* loadConfiguration(const std::string& pathUser,
			const std::string& pathDefault,const std::string& pathGeneral,
			const std::string& voName);
		static void setDefaultValuesAd(glite::jdl::Ad* jdl,
			glite::jdl::Ad* conf,
			const std::string& pathOpt="");
		static void setDefaultValues(glite::jdl::JobAd* jdl,
			    		     glite::jdl::Ad* conf);
		static void setDefaultValues(glite::jdl::ExpDagAd* jdl,
					     glite::jdl::Ad* conf);
		static void setDefaultValues(glite::jdl::CollectionAd* jdl,
			                     glite::jdl::Ad*);
		static int getSoapTimeout(const std::string& timeoutName, 
					  glite::jdl::Ad*);
		/** look for, check, parse and retrieve VO name and value */
		void parseVo(voSrc src, std::string& voPath, std::string& voName);
		/** Return the list of all unknown values*/
		std::vector<std::string> getUnknown(glite::jdl::Ad* jdl);
		/** Try and build a Dagad istance and retrieve its jobid-node map
		*@param jdl the dagad string representation
		*@return a mapping between dagad jobid sons and thier correspondent node name, empty map if any error occurred
		 */
		static std::map< std::string, std::string > getJobIdMap(const std::string& jdl);
		/** Retrieve the node name corresponding to a required jobid
		* @param map the jobids-nodes mapping as returned from #getJobIdMap
		* @param jobid the id of the job to be looked for in the map
		* @see #getJobIdMap
		* @return the uinque name of the node corresponding to the required jobid,
		* or jobid unique string representation if any error occurred
		 */
		static std::string JobId2Node (const std::map< std::string, std::string > &map,
			glite::wmsutils::jobid::JobId jobid);
	private:
		/**
		* fill the classad attributes, if missing, from another classad.
		* The only attributes copied are the configuration ones
		* @param source the Ad instance which will be used for the filling
		* @param destination the destination Ad instance that will be filled
		*/
		void fillConfigAttributes(glite::jdl::Ad& source, glite::jdl::Ad& destination);
		
		/*
		* method used by loadConfiguration: check whether
		* configuration file is plain classad or WmsClient=[...]
		* @param return SUCCESS (false) or ERROR (true)
		*/
		bool checkConfigurationAd(glite::jdl::Ad& ad, const std::string& path);

		void checkDeprecatedAttributes(glite::jdl::Ad& ad, const std::string &path);

		std::string generateVoPath(std::string& voName);
		/*
		*Handles the input options
		*/
		Options *wmcOpts;
		/*
		* Handles the verbosity level of the  log  messages on the stdout
		*/
		LogLevel vbLevel;
};
} // glite
} // wms
} // client
} // utilities
#endif
