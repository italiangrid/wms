#ifndef  ORG_GLITE_WMSUI_WRAPY_UCWRAPPER_H
#define ORG_GLITE_WMSUI_WRAPY_UCWRAPPER_H
/*
 * UcWrapper.h
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 *
 */

#include <vector>
/*********************
* VOMS includes:
*********************/
#include "glite/security/voms/voms_api.h"

/**
 * Provide a wrapper that manages user cerificate general information
 * it allows to check the proxy cerfticate's parameters such us validity, retrieve issuer etc etc.
 * it also allows to manipulate VirtualOrganisation certificate properties, retreving, if present, VO default names and groups
 *
 * @brief User Credential Event wrapper class
 * @version 0.1
 * @date 15 April 2002
 * @author Alessandro Maraschini <alessandro.maraschini@datamat.it>
*/
class  UCredential {
	public:
		/** Constructor */
		UCredential ( const std::string& proxy_file) ;
		/**User Credential default Destructor
		*/
		~UCredential();
		/** Retrieve the default Virtual Organisation name
		* @return the default VO
		*/
		std::string  getDefaultVoName ();
		/** Retrieve the default FQAN for the current certificate
		* @return the default FQAN
		*/
		std::string  getDefaultFQAN ();
		/** Retrieve the Certificate Issuer*/
		std::string  getIssuer() ;
		/** Retrieve the last allowed date*/
		int getExpiration() ;
		/** Retrieve the vector of all  the Virtual Organisation names */
		std::vector <std::string> getVoNames ();
		/***Retrieve all groups belonging to the specified VirtualOrganisation
		* @param voName the name of the Virtual Organisation where to retrieve groups */
		std::vector <std::string> getGroups ( const std::string& voName ) ;
		/** Returns the groups belonging to the default VirtualOrganisation*/
		std::vector <std::string > getDefaultGroups () ;
		/** Check wheater the specifie Virtual Organisation is contained in the Vo certificate extension*/
		bool containsVo ( const std::string& voName )  ;
		/**Set an attribute value (of string type)
		* @param err the string description passed by reference. Python will consider it as a returning parameter
		* @return a couple of [ int , string ] representing the error code (0 for success) and the error string representation
		*/
		std::string get_error () ;
	private:
		int vo_data_error , timeleft ;
		std::string proxy_file;
		// std::string vo_maraska;
		bool load_voms ( vomsdata& d ) ;
};

#endif

