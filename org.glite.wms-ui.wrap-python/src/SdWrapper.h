#ifndef  ORG_GLITE_WMSUI_WRAPY_SDWRAPPER_H
#define ORG_GLITE_WMSUI_WRAPY_SDWRAPPER_H
/*
 * SdWrapper.h
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 *
 */

#include <vector>
#include <string>
// Service Discovery include:
#include "ServiceDiscovery.h"

/**
 * Provide a wrapper that manages Service Discovery Query
 * it allows to Retrieve information about services published on the net
 * such as LB available servers
 *
 * @brief Service Discovery wrapper class
 * @version 0.1
 * @date 15 April 2002
 * @author Alessandro Maraschini <alessandro.maraschini@datamat.it>
*/
class  ServiceDiscovery {
	public:
		/** Constructor */
		ServiceDiscovery () ;
		/** Service Discovery default Destructor */
		~ServiceDiscovery();
		/** Query the Service Discovery for available services
		*@param voName the Virtual Organisation the user is working for (empty string for all vo)
		*@param sdType the Service Type requested
		*/
		std::vector<std::string> lookForServices(const std::string &voName,const std::string &sdType);
		std::string get_error ();
	private:
		std::string sdError;	
};

#endif

