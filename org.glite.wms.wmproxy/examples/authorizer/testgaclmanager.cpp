/*
*	Copyright (c) Members of the EGEE Collaboration. 2004.
*	See http://www.eu-egee.org/partners/ for details on the copyright holders.
*
*	Licensed under the Apache License, Version 2.0 (the "License");
*	you may not use this file except in compliance with the License.
*	You may obtain a copy of the License at
*
*	    http://www.apache.org/licenses/LICENSE-2.0
*
*	Unless required by applicable law or agreed to in writing, software
*	distributed under the License is distributed on an "AS IS" BASIS,
*	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*	See the License for the specific language governing permissions and
*	limitations under the License.
*/

#include "gaclmanager.h"


#define GACL_DIR		"./gacl/"

using namespace std;


void testFileNotFound (std::string file){
	cout << "Test: file not found exception " << endl ;

	GaclManager gacl( file, false ) ;

	gacl.printGacl( );
};


int testCheckAllowPermission (std::string file, std::string user, std::string permission){

	int allow = 0;

	cout << "Test: checkAllowPermission: " << endl ;
	cout << "---------------------------" << endl ;
	cout << "file		= " << file << endl ;
	cout << "user		= " << user << endl ;
	cout << "permission	= " << permission << endl ;
	cout << "---------------------------" << endl ;

	GaclManager gacl( file ) ;

	cout << "testgacl>gacl------------------------" << endl;
	gacl.printGacl ( ) ;
	cout << "---------------------------" << endl;

	allow = gacl.checkAllowPermission ( user, permission ) ;

	cout << "testgacl> allow = [" << allow << "]" << endl ;

	return allow ;

};

int testCheckAllowReadForAnyUser ( std::string file){
	int allow = 0;

	cout << "Test: checkReadForAnyUser: " << endl ;
	cout << "---------------------------" << endl ;
	cout << "file		= " << file << endl ;
	cout << "---------------------------" << endl ;

	GaclManager gacl( file ) ;

	cout << "testgacl>gacl------------------------" << endl;
	gacl.printGacl ( ) ;
	cout << "---------------------------" << endl;

	allow = gacl.checkAllowReadForAnyUser ( ) ;

	cout << "testgacl> allow = [" << allow << "]" << endl ;

	return allow ;
};

int testCheckAllowWriteForAnyUser ( std::string file  ){
	int allow = 0;

	cout << "Test: checkWriteForAnyUser: " << endl ;
	cout << "---------------------------" << endl ;
	cout << "file		= " << file << endl ;
	cout << "---------------------------" << endl ;

	GaclManager gacl( file ) ;

	cout << "testgacl>gacl------------------------" << endl;
	gacl.printGacl ( ) ;
	cout << "---------------------------" << endl;

	allow = gacl.checkAllowWriteForAnyUser ( ) ;

	cout << "testgacl> allow = [" << allow << "]" << endl ;

	return allow ;

};

int testCheckAllowExecForAnyUser ( std::string file ){

	int allow = 0;

	cout << "Test: checkExecForAnyUser: " << endl ;
	cout << "---------------------------" << endl ;
	cout << "file		= " << file << endl ;
	cout << "---------------------------" << endl ;

	GaclManager gacl( file ) ;

	cout << "testgacl>gacl------------------------" << endl;
	gacl.printGacl ( ) ;
	cout << "---------------------------" << endl;

	allow = gacl.checkAllowExecForAnyUser ( ) ;

	cout << "testgacl> allow = [" << allow << "]" << endl ;

	return allow ;

};

int testCheckAllowListForAnyUser ( std::string file){

	int result  = 0;

	cout << "Test: checkListForAnyUser: " << endl ;
	cout << "---------------------------" << endl ;
	cout << "file		= " << file << endl ;
	cout << "---------------------------" << endl ;

	GaclManager gacl( file ) ;

	cout << "testgacl>gacl------------------------" << endl;
	gacl.printGacl ( ) ;
	cout << "---------------------------" << endl;

	result = gacl.checkAllowListForAnyUser ( ) ;

	cout << "testgacl> result = [" << result << "]" << endl ;

	return result ;
};

int testAllowPermission( std::string file, std::string user, std::string permission, std::string rawname = "", std::string rawvalue = ""){
	int result  = 0;

	cout << "Test: checkAllowPermission: " << endl ;
	cout << "---------------------------" << endl ;
	cout << "file		= " << file << endl ;
	cout << "user		= " << user << endl ;
	cout << "permission	= " << permission << endl ;
	cout << "---------------------------" << endl ;

	GaclManager gacl( file ) ;

	cout << "testgacl>gacl------------------------" << endl;
	gacl.printGacl ( ) ;
	cout << "---------------------------" << endl;

	result = gacl.allowPermission ( user, permission, rawname, rawvalue ) ;

	cout << "testgacl> result = [" << result << "]" << endl ;

	return result ;
};
int testAllowExecToAnyUser( std::string file){
	int result  = 0;

	cout << "Test: checkAllowPermission: " << endl ;
	cout << "---------------------------" << endl ;
	cout << "file		= " << file << endl ;
	cout << "---------------------------" << endl ;

	GaclManager gacl( file ) ;

	cout << "testgacl>gacl------------------------" << endl;
	gacl.printGacl ( ) ;
	cout << "---------------------------" << endl;

	result = gacl.allowExecToAnyUser (  ) ;

	cout << "testgacl> result = [" << result << "]" << endl ;

	return result ;
};

int testDenyPermission(std::string file, std::string user, std::string permission, std::string rawname = "", std::string rawvalue=""){
	int result  = 0;

	cout << "Test: checkDenyPermission: " << endl ;
	cout << "---------------------------" << endl ;
	cout << "file		= " << file << endl ;
	cout << "user		= " << user << endl ;
	cout << "permission	= " << permission << endl ;
	cout << "---------------------------" << endl ;

	GaclManager gacl( file ) ;

	cout << "testgacl>gacl------------------------" << endl;
	gacl.printGacl ( ) ;
	cout << "---------------------------" << endl;

	result = gacl.denyPermission (user, permission, rawname, rawvalue) ;

	cout << "testgacl> result = [" << result << "]" << endl ;

	return result ;
};


void allowReadToAnyUser  (  ){
};

void allowExecToAnyUser  (std::string file  ){
};
void allowListToAnyUser  (  ){
};

void denyReadToAnyUser  (  ){
};
void denyWriteToAnyUser  (  ){
};
int  testDenyExecToAnyUser  (std::string file   ){

	int result  = 0;

	cout << "Test: checkDenyPermission: " << endl ;
	cout << "---------------------------" << endl ;
	cout << "file		= " << file << endl ;
	cout << "---------------------------" << endl ;

	GaclManager gacl( file ) ;

	cout << "testgacl>gacl------------------------" << endl;
	gacl.printGacl ( ) ;
	cout << "---------------------------" << endl;

	result = gacl.denyExecToAnyUser (  ) ;

	cout << "testgacl> result = [" << result << "]" << endl ;

	return result ;

};
void denyListToAnyUser  (  ){
};

void checkResult ( int result, int expected ){

	switch (result ){
		case 0: {
			cout << "result = 0 ( or false ) "<< endl ;
			break;
		}
		case 1 :{
			cout << "result =  (or true) "<< endl ;
			break;
		}
		default :{
			cout << "error !!!"<< endl ;
		}
	} ;

		if ( result == expected)
			cout << "SUCCESS" << endl ;
		else
			cout << "FAILED" << endl ;
};

void  checkAllowPermissionTests ()
{
	int i = 0;
	 int result =-1 ;
	std::string file = "" ;

	// -------------------------------------------------
	cout << "-------------------------------------------------------------------------" << endl;
	cout << "checkAllowPermission TESTS " << endl;
	cout << "-------------------------------------------------------------------------\n\n" << endl;

	cout << "*****************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  checks which  exec is allowed for any-user  / method(checkAllowPermission)" << endl;
	cout << "*****************************************************************************************************************************************" << endl;
	cout << "expected result = true " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-exec-allow.gacl") ;
	cout << "file = " << file << endl;
	result = testCheckAllowPermission (file, "any-user", "exec");
	checkResult (result, 1);

	// -------------------------------------------------

	cout << "*****************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " - checks which exec is not allowed for any-user / method(checkAllowPermission)" << endl;
	cout << "******************************************************************************************************************************************" << endl;
	cout << "expected result = false " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append( "any-exec-notallow.gacl") ;
	cout << "file = " << file << endl;
	result = testCheckAllowPermission (file, "any-user", "exec");
	checkResult (result, 0);

	// -------------------------------------------------

	cout << "*****************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " - check which exec is denied for any-user / method()" << endl;
	cout << "******************************************************************************************************************************************" << endl;
	cout << "expected result = false " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-exec-deny.gacl") ;
	cout << "file = " << file << endl;
	result = testCheckAllowPermission (file, "any-user", "exec");
	checkResult (result, 0);

	// -------------------------------------------------

	cout << "*****************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " - file with exec both allowed and denied / method(checkAllowPermission)" << endl;
	cout << "******************************************************************************************************************************************" << endl;
	cout << "expected result = error " << endl;
	cout << "---------------------------------------------------" << endl;
	file =  file.assign(GACL_DIR).append("any-exec-allow_deny.gacl") ;
	cout << "file = " << file << endl;
	result =testCheckAllowPermission (file, "any-user", "exec");
	checkResult (result, -1);

	// -------------------------------------------------

	cout << "*****************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " - file with exec both allowed and denied / method(checkAllowPermission)" << endl;
	cout << "******************************************************************************************************************************************" << endl;
	cout << "expected result = error " << endl;
	cout << "---------------------------------------------------" << endl;
	file =  file.assign(GACL_DIR).append("any-exec-notallow_notdeny.gacl") ;
	cout << "file = " << file << endl;
	result =testCheckAllowPermission (file, "any-user", "exec");
	checkResult (result, -2);
	// -------------------------------------------------

	cout << "*****************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  checks which  exec is allowed for any-user / method(checkAllowPermission)" << endl;
	cout << "*****************************************************************************************************************************************" << endl;
	cout << "expected result = true " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-exec-allow.gacl") ;
	cout << "file = " << file << endl;
	result = testCheckAllowPermission (file, "any-user", "exec");
	checkResult (result, 1);

	// -------------------------------------------------

	cout << "*********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  checks which  list  is allowed for any-user / method(checkAllowPermission)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = true " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-list-allow.gacl") ;
	cout << "file = " << file << endl;
	result = testCheckAllowPermission (file, "any-user", "list");
	checkResult (result, 1);

	// -------------------------------------------------
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  checks which  write  is denied for any-user / method(checkAllowPermission)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = true " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-write-deny.gacl") ;
	cout << "file = " << file << endl;
	result = testCheckAllowPermission (file, "any-user", "write");
	checkResult (result, 0);

	// -------------------------------------------------

}
void checkAllowReadForAnyUserTests ()
{
	int i, result = 0;
	std::string file = "" ;

	cout << "-------------------------------------------------------------------------" << endl;
	cout << "checkAllowReadForAnyUserTest TESTS " << endl;
	cout << "-------------------------------------------------------------------------\n\n" << endl;

	cout << "**********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  checks which read  is allowed for any-user / method(checkAllowReadForAnyUserTest)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = true " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-read-allow.gacl") ;
	cout << "file = " << file << endl;
	result = testCheckAllowReadForAnyUser (file);
	checkResult (result, 1);

	// -------------------------------------------------

	cout << "*********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  checks which  read  is denied for any-user / method(checkAllowReadForAnyUserTest)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = true " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-read-deny.gacl") ;
	cout << "file = " << file << endl;
	result = testCheckAllowReadForAnyUser (file);
	checkResult (result, 0);

}

void checkAllowWriteForAnyUserTests ()
{
	int i, result = 0;
	std::string file = "" ;

	cout << "-------------------------------------------------------------------------" << endl;
	cout << "checkAllowWriteForAnyUserTest TESTS " << endl;
	cout << "-------------------------------------------------------------------------\n\n" << endl;

	cout << "**********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  checks which  write  is allowed for any-user / method(checkAllowReadForAnyUserTest)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = true " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-write-allow.gacl") ;
	cout << "file = " << file << endl;
	result = testCheckAllowWriteForAnyUser (file);
	checkResult (result, 1);

	// -------------------------------------------------

	cout << "*********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  checks which  write  is denied for any-user / method(checkAllowReadForAnyUserTest)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = true " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-write-deny.gacl") ;
	cout << "file = " << file << endl;
	result = testCheckAllowWriteForAnyUser (file);
	checkResult (result, 0);

}

void checkAllowListForAnyUserTests ()
{
	int i, result = 0;
	std::string file = "" ;

	cout << "-------------------------------------------------------------------------" << endl;
	cout << "checkAllowListForAnyUserTest TESTS " << endl;
	cout << "-------------------------------------------------------------------------\n\n" << endl;

	cout << "**********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  checks which  list  is allowed for any-user / method(checkAllowListForAnyUserTest)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = true " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-write-allow.gacl") ;
	cout << "file = " << file << endl;
	result = testCheckAllowListForAnyUser (file);
	checkResult (result, 1);

	// -------------------------------------------------

	cout << "*********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  checks which  list  is denied for any-user / method(checkAllowListForAnyUserTest)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = true " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-write-deny.gacl") ;
	cout << "file = " << file << endl;
	result = testCheckAllowListForAnyUser (file);
	checkResult (result, 0);

}

void checkAllowExecForAnyUserTests ()
{
	int i, result = 0;
	std::string file = "" ;

	cout << "-------------------------------------------------------------------------" << endl;
	cout << "checkAllowExecForAnyUserTest TESTS " << endl;
	cout << "-------------------------------------------------------------------------\n\n" << endl;

	cout << "**********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  checks which exec  is allowed for any-user / method(checkAllowExecForAnyUserTest)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = true " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-exec-allow.gacl") ;
	cout << "file = " << file << endl;
	result =  testCheckAllowExecForAnyUser(file);
	checkResult (result, 1);

	// -------------------------------------------------

	cout << "*********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  checks which exec  is denied for any-user / method(checkAllowExecForAnyUserTest)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = true " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-exec-deny.gacl") ;
	cout << "file = " << file << endl;
	result = testCheckAllowExecForAnyUser (file);
	checkResult (result, 0);

}

void allowExecToAnyUserTests ()
{
	int i, result = 0;
	std::string file, command = "" ;

	cout << "-------------------------------------------------------------------------" << endl;
	cout << "allow Exec TESTS " << endl;
	cout << "-------------------------------------------------------------------------\n\n" << endl;

	cout << "**********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  allows Exec for any user  method (allowExecToAnyUser)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result =0 " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-notexec.gacl") ;
	command = command.assign("cp ").append(file).append (" ").append( file).append(".test");
	system((char*)command.c_str());
	file = file +".test";
	cout << "file = " << file << endl;
	result = testAllowExecToAnyUser(file);
	cout << "NEW FILE--------" << endl;
	system((char*)command.c_str());
	cout << "---------------" << endl;
	checkResult (result, 0);
	// -------------------------------------------------

	cout << "**********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  allows Exec for any user where it is denied / method (allowExecToAnyUser)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = 0 " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-exec-deny.gacl") ;
	command = command.assign("cp ").append(file).append (" ").append( file).append(".test");
	system((char*)command.c_str());
	file = file +".test";
	cout << "file = " << file << endl;
	result = testAllowExecToAnyUser(file);
	command = command.assign( "more " ). append(file);
	cout << "NEW FILE--------" << endl;
	system((char*)command.c_str());
	cout << "---------------" << endl;
		checkResult (result, 0);

	// -------------------------------------------------

	cout << "**********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  allows Exec for any user where it is BOTH denied and allowed / method (allowExecToAnyUser)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = 0 " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-exec-allow_deny.gacl") ;
	command = command.assign("cp ").append(file).append (" ").append( file).append(".test");
	system((char*)command.c_str());
	file = file +".test";
	cout << "file = " << file << endl;
	result = testAllowExecToAnyUser(file);
	command = command.assign( "more " ). append(file);
	cout << "NEW FILE--------" << endl;
	system((char*)command.c_str());
	cout << "---------------" << endl;
	checkResult (result, 0);

	// -------------------------------------------------

	cout << "**********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  allows Exec for any user (the file doesn't exist) )/ method (allowExecToAnyUser)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = 0 " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("dummy-allow.gacl.test") ;
	command = command.assign("ls ").append(file);
	system((char*)command.c_str());
	cout << "file = " << file << endl;
	result = testAllowExecToAnyUser(file);
	command = command.assign( "more " ). append(file);
	cout << "NEW FILE--------" << endl;
	system((char*)command.c_str());
	cout << "---------------" << endl;
	checkResult (result, 0);


	/*
	cout << "**********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  allow Exec to any user in a file where it is already set  / method (allowPermissionToAnyUser)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = true " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-exec-allow.gacl") ;
	command ="cp " + file +" " file +".test";
	system(command);
	file += file +".test";
	cout << "file = " << file << endl;
	result = testAllowPermissionToAnyUser(file);
	checkResult (result, 1);

	// -------------------------------------------------

	cout << "*********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  checks which exec  is denied for any-user / method(checkAllowExecForAnyUserTest)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = true " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-exec-deny.gacl") ;
	command ="cp " + file +" " file +".test";
	system(command);
	file += file +".test";
	cout << "file = " << file << endl;
	result = testAllowPermission (file);
	checkResult (result, 0);
	*/
}
void denyExecToAnyUserTests ()
{
	int i, result = 0;
	std::string file, command = "" ;

	cout << "-------------------------------------------------------------------------" << endl;
	cout << "deny Exec  TESTS " << endl;
	cout << "-------------------------------------------------------------------------\n\n" << endl;

	cout << "**********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " - denies Exec for any user  method (denyExecToAnyUser)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result =0 " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-notexec.gacl") ;
	command = command.assign("cp ").append(file).append (" ").append( file).append(".test");
	system((char*)command.c_str());
	file = file +".test";
	cout << "file = " << file << endl;
	result = testDenyExecToAnyUser(file);
	command = command.assign( "more " ). append(file);
	cout << "NEW FILE--------" << endl;
	system((char*)command.c_str());
	cout << "---------------" << endl;
	checkResult (result, 0);

	// -------------------------------------------------

	cout << "**********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " - denied Exec for any user where it is denied / method (denyExecToAnyUser)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = 0 " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-exec-allow.gacl") ;
	command = command.assign("cp ").append(file).append (" ").append( file).append(".test");
	system((char*)command.c_str());
	file = file +".test";
	cout << "file = " << file << endl;
	result = testDenyExecToAnyUser(file);
	command = command.assign( "more " ). append(file);
	cout << "NEW FILE--------" << endl;
	system((char*)command.c_str());
	cout << "---------------" << endl;
	checkResult (result, 0);

	// -------------------------------------------------

	cout << "**********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -   denies Exec for any user where it is BOTH denied and allowed / method (denyExecToAnyUser)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = 0 " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("any-exec-allow_deny.gacl") ;
	command = command.assign("cp ").append(file).append (" ").append( file).append(".test");
	system((char*)command.c_str());
	file = file +".test";
	cout << "file = " << file << endl;
	result = testDenyExecToAnyUser(file);
	command = command.assign( "more " ). append(file);
	cout << "NEW FILE--------" << endl;
	system((char*)command.c_str());
	cout << "---------------" << endl;
	checkResult (result, 0);
	// -------------------------------------------------

	cout << "**********************************************************************************************************************************************" << endl;
	cout << "Test number " << (++i) << " -  denies Exec for any user (the file doesn't exist) )/ method (allowExecToAnyUser)" << endl;
	cout << "*********************************************************************************************************************************************" << endl;
	cout << "expected result = 0 " << endl;
	cout << "---------------------------------------------------" << endl;
	file = file.assign(GACL_DIR).append("dummy-deny.gacl.test") ;
	command = command.assign("ls ").append(file);
	system((char*)command.c_str());
	cout << "file = " << file << endl;
	result = testDenyExecToAnyUser(file);
	command = command.assign( "more " ). append(file);
	cout << "NEW FILE--------" << endl;
	system((char*)command.c_str());
	cout << "---------------" << endl;
	checkResult (result, 0);
}

int main ( )
{
	std::string command = "rm *.gacl.test";
	system((char*)command.c_str());


	try{
		testFileNotFound ( "file.gacl" );

	// checkAllowPermissionTests () ;

	// checkAllowReadForAnyUserTests ();

	// checkAllowWriteForAnyUserTests ();

	// checkAllowListForAnyUserTests ();

	// checkAllowExecForAnyUserTests();

	//allowExecToAnyUserTests() ;

	 //denyExecToAnyUserTests() ;

	} catch( GaclException &exc){
		cout << "exception [" << flush << exc.what ( ) << "]" << endl ;
	}

	return 0;
}
