/***************************************************************************
    filename  : SdWrapper.cpp
    begin     : Jul 06
    author    : Alessandro Maraschini
    email     : egee@datamat.it
    copyright : (C) 2006 by DATAMAT
***************************************************************************/
#include "SdWrapper.h"
#include <iostream>

using namespace std ;

// SERVICE DISCOVERY IMPLEMENTATION
/** Constructor */
ServiceDiscovery::ServiceDiscovery (){
	sdError="";
};
/** Default Dtor*/
ServiceDiscovery::~ServiceDiscovery(){};


/*
* Query the service discovery for published WMProxy enpoints
*/
std::vector<std::string> ServiceDiscovery::lookForServices(const string &voName,const string &sdType){

	// Setup needed variables
	std::vector<std::string> wmps ;
	SDException ex;
	SDServiceList *serviceList=NULL;
	SDVOList *vos =NULL;
	char **names=NULL;
	sdError="";

	// Setup VO instance (if not-empty string):
	if (!voName.empty()){
		names = new char*[1];
		names[0] = new char[voName.length() +1];
		strcpy(names[0], voName.c_str());
		SDVOList vosAPP = {1, names};
		vos=&vosAPP;
	}

	//  SD_listServices parameters: type, site, volist, exception
	serviceList = SD_listServices(sdType.c_str(), NULL, vos, &ex);
	if (serviceList != NULL) {
		if ( serviceList->numServices > 0 )  {
			for(int k=0; k < serviceList->numServices; k++) {
				wmps.push_back(strdup(serviceList->services[k]->endpoint));
			}
		}else{
			// NO RESULT!!
			sdError = "Service not found: "+sdType;
		}
		SD_freeServiceList(serviceList);
		if (!voName.empty()){
			delete []names[0];
			delete []names;
		}
	} else {
		if (ex.status == SDStatus_SUCCESS){
			sdError ="Service not known: "+ sdType ;
		} else {
			sdError ="Service Discovery Failed: " + string(ex.reason);
		}
	}
	return wmps;
}

std::string ServiceDiscovery::get_error (){
	return sdError;
}

