/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "url_cu_suite.h"
#include "jobadapter/url/URL.h"

#include <string>
#include <iostream>

using namespace std;
using namespace glite::wms::helper::jobadapter::url;


string Url_test::ok_no_port_no_path = "http://www.cnaf.infn.it/";
string Url_test::ok_with_port_with_path = "http://www.cnaf.infn.it:8080/dir1/dir2/file";
string Url_test::ok_no_port_with_path = "http://www.cnaf.infn.it/dir1/dir2/file";
string Url_test::ok_with_port_no_path = "http://www.cnaf.infn.it:8080/";

string Url_test::ko_without_protocol = "www.cnaf.infn.it/:8080";
string Url_test::ko_without_path = "http://www.cnaf.infn.it:8080";
string Url_test::ko_with_alfanum_port = "http://www.cnaf.infn.it/:8O8O";

/*
URL Url_test::url_ok_no_port_no_path;
URL Url_test::url_ok_with_port_with_path;
URL Url_test::url_ok_no_port_with_path;
URL Url_test::url_ok_with_port_no_path;

URL Url_test::url_ko_without_protocol;
URL Url_test::url_ko_without_path;
URL Url_test::url_ko_with_alfanum_port;


URL Url_test::url_empty;
URL Url_test::url_void;
*/

//  problematic...!!!
/*  Temo diano errore e secondo me non dovrebbero:
 *      "www.cnaf.infn.it/"
 *      "http://www.cnaf.infn.it"
 *      "http://www.cnaf.infn.it/"
 *      "http://www.cnaf.infn.it/"
 * 
 *  Temo non diano errore (o errori non gestiti) e invece dovrebbero:
 *      "://www.cnaf.infn.it/"
 *      "pippo://www.cnaf.infn.it/"
 *      "pippo://www.cnaf.infn.it/"
 *      "www.cnaf.infn.it://pippo/"
 * 
 * - numero di porta..ok qualsiasi range??
 * - qualsiasi carattere ammesso nell'url?? spazi?? punteggiatura?
 * */


void Url_test::setUp(){
    cout<<"setUp url"<<endl;
    
/* TODO: TEST FIXTURE?  HOW TO DO IT? 
 * string empty_string("");
    url_ok_no_port_no_path() ;
    url_ok_with_port_with_path() ;
    url_ok_no_port_with_path();
    url_ok_with_port_no_path ();

    url_empty(empty_string);
    url_void;
*/    
}

void Url_test::tearDown(){
    cout<<"TearDownAd url"<<endl;
}

void Url_test::constructors_case(){
    
try {        
    URL voidUrl;
    URL url_ok_with_portUrl_no_path(ok_with_port_no_path);
    URL url_ok_no_port_no_path(ok_no_port_no_path);
    URL url_ok_with_portUrl_with_path(ok_with_port_with_path);
    URL url_ok_no_port_with_path(ok_no_port_with_path);
} catch (ExInvalidURL exInvURL){
    CPPUNIT_FAIL("Exception creating a new URL: ");
    cout<<"Details: "<<exInvURL.parameter()<<endl;
}    

try {        
    URL emptyUrl(""); 
} catch (ExInvalidURL exInvURL){
    CPPUNIT_ASSERT(true);
}

try {        
    URL url_ko_without_protocol(ko_without_protocol); 
} catch (ExInvalidURL exInvURL){
    CPPUNIT_ASSERT(true);
}

try {        
    URL url_ko_without_path(ko_without_path); 
} catch (ExInvalidURL exInvURL){
    CPPUNIT_ASSERT(true);
}

try {        
    URL url_ko_with_alfanum_port(ko_with_alfanum_port); 
} catch (ExInvalidURL exInvURL){
    CPPUNIT_ASSERT(true);
}
   
}

void Url_test::is_empty_case(){
    URL url_void;
    URL url_ok_with_port_with_path(ok_with_port_with_path);
    CPPUNIT_ASSERT(!url_ok_with_port_with_path.is_empty());
    CPPUNIT_ASSERT(url_void.is_empty());
}

void Url_test::protocol_case(){
    URL url_ok_no_port_no_path("http://www.cnaf.infn.it/");
    URL url_ok_with_port_with_path("http://www.cnaf.infn.it:8080/dir1/dir2/file");
    URL url_ok_no_port_with_path("http://www.cnaf.infn.it/dir1/dir2/file");
    URL url_ok_with_port_no_path("http://www.cnaf.infn.it:8080/");
    URL url_void;
    
    CPPUNIT_ASSERT(url_ok_no_port_no_path.protocol()=="http");
    CPPUNIT_ASSERT(url_ok_with_port_with_path.protocol()=="http");
    CPPUNIT_ASSERT(url_ok_no_port_with_path.protocol()=="http");
    CPPUNIT_ASSERT(url_ok_with_port_no_path.protocol()=="http");
    
    CPPUNIT_ASSERT(url_void.protocol()=="");
}

void Url_test::host_case(){
    URL url_ok_no_port_no_path("http://www.cnaf.infn.it/");
    URL url_ok_with_port_with_path("http://www.cnaf.infn.it:8080/dir1/dir2/file");
    URL url_ok_no_port_with_path("http://www.cnaf.infn.it/dir1/dir2/file");
    URL url_ok_with_port_no_path("http://www.cnaf.infn.it:8080/");
    URL url_void;

    CPPUNIT_ASSERT(url_ok_no_port_no_path.host()=="www.cnaf.infn.it");
    CPPUNIT_ASSERT(url_ok_with_port_with_path.host()=="www.cnaf.infn.it");
    CPPUNIT_ASSERT(url_ok_no_port_with_path.host()=="www.cnaf.infn.it");
    CPPUNIT_ASSERT(url_ok_with_port_no_path.host()=="www.cnaf.infn.it");
    CPPUNIT_ASSERT(url_void.host()=="");
}

void Url_test::port_case(){
    URL url_ok_no_port_no_path("http://www.cnaf.infn.it/");
    URL url_ok_with_port_with_path("http://www.cnaf.infn.it:8080/dir1/dir2/file");
    URL url_ok_no_port_with_path("http://www.cnaf.infn.it/dir1/dir2/file");
    URL url_ok_with_port_no_path("http://www.cnaf.infn.it:8080/");
    URL url_void;

    CPPUNIT_ASSERT(url_ok_no_port_no_path.port()=="");
    CPPUNIT_ASSERT(url_ok_with_port_with_path.port()=="8080");
    CPPUNIT_ASSERT(url_ok_no_port_with_path.port()=="");
    CPPUNIT_ASSERT(url_ok_with_port_no_path.port()=="8080");
    CPPUNIT_ASSERT(url_void.port()=="");
}

void Url_test::path_case(){

    URL url_ok_no_port_no_path("http://www.cnaf.infn.it/");
    URL url_ok_with_port_with_path("http://www.cnaf.infn.it:8080/dir1/dir2/file");
    URL url_ok_no_port_with_path("http://www.cnaf.infn.it/dir1/dir2/file");
    URL url_ok_with_port_no_path("http://www.cnaf.infn.it:8080/");
    URL url_void;
          
    CPPUNIT_ASSERT(url_ok_with_port_with_path.path()=="/dir1/dir2/file");
    CPPUNIT_ASSERT(url_ok_no_port_with_path.path()=="/dir1/dir2/file");
    CPPUNIT_ASSERT(url_ok_with_port_no_path.path()=="/");
    CPPUNIT_ASSERT(url_void.path()=="");
    
}


void Url_test::as_string_case(){
    URL url_ok_no_port_no_path("http://www.cnaf.infn.it/");
    URL url_ok_with_port_with_path("http://www.cnaf.infn.it:8080/dir1/dir2/file");
    URL url_ok_no_port_with_path("http://www.cnaf.infn.it/dir1/dir2/file");
    URL url_ok_with_port_no_path("http://www.cnaf.infn.it:8080/");
    URL url_void;
   
    CPPUNIT_ASSERT_NO_THROW(url_ok_no_port_no_path.as_string());
    CPPUNIT_ASSERT_NO_THROW(url_ok_with_port_with_path.as_string());
    CPPUNIT_ASSERT_NO_THROW(url_ok_no_port_with_path.as_string());
    CPPUNIT_ASSERT_NO_THROW(url_ok_with_port_no_path.as_string());
    CPPUNIT_ASSERT_NO_THROW(url_void.as_string());
 
    try {
        URL test(url_ok_no_port_no_path.as_string());
        URL test2(url_ok_with_port_with_path.as_string());  
        URL test3(url_ok_no_port_with_path.as_string());  
        URL test4(url_ok_with_port_no_path.as_string());
    } catch(ExInvalidURL exInvURL){
        CPPUNIT_FAIL("Faild creating a new URL object using an old object with asString method");
    }   
}
