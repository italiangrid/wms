/*
 * glite-wmp-utils.cpp
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 */
#include "excman.h"
#include "utils.h"
#include "adutils.h"
#include <iostream>
using namespace std ;
using namespace glite::wms::client::utilities;
Utils *utils =NULL;
void answer(){
	// ANSWER
	utils->answerYes("Your Name is Alessandro Maraschini?", false);
}

void resolve(){
	// RESOLVE
	string resolved;
	utils->resolveHost("http://matrix",resolved);
}

void getLbs(){
	// GET LBS
	std::vector<std::vector<std::string> > lbGroup;
	string lbBase="http://lbaddress:1234";
	for (int i=0; i< 4; i++){
		// cout << "   ***   LB =" << i << endl ;
		lbBase+="N#";
		std::vector<std::string> lbs;
		for (int j=0; j<3; j++){
			lbBase+="X_";
			lbs.push_back(lbBase);
			// cout << "Added lbs: " << lbBase << endl ;
		}
		lbGroup.push_back(lbs);
	}
	std::vector<std::string> lbResult=utils->getLbs(lbGroup,2);
	for (unsigned int i=0;i<lbResult.size();i++) cout << "LB: "<<lbResult[i]<< endl ;
}

int main(int argc,char *argv[]){
try{
	WMS_EXCM_TRY()
	try{
		Options *opts ;
		opts= new Options(Options::JOBSUBMIT);
		opts->readOptions(argc, (const char**)argv);
		utils=new Utils(opts);
	}catch (glite::wmsutils::exception::Exception &exc){
		cerr << exc.what();
	}
	WMS_EXCM_CATCH(WMS_FATAL)

/*
	WMS_EXCM_TRY()
		answer();
		resolve();
	WMS_EXCM_CATCH(WMS_ERROR)

	WMS_EXCM_TRY()
		getLbs();
		checkLBNS("https://address:123456");
		checkLBNS("://address");
	WMS_EXCM_CATCH(WMS_ERROR)
*/
	return 0;
} catch (std::exception &exc){
	 cout << "FATAL standard Exception: " << exc.what() << endl ;
}
}
