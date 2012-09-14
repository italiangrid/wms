/*
*	Copyright (c) Members of the EGEE Collaboration. 2004.
*	See http://www.eu-egee.org/partners/ for details on the
*	copyright holders.
*
*	Licensed under the Apache License, Version 2.0 (the "License");
*	you may not use this file except in compliance with the License.
*	You may obtain a copy of the License at
*	 
*	     http://www.apache.org/licenses/LICENSE-2.0
*	 
*	Unless required by applicable law or agreed to in writing, software
*	distributed under the License is distributed on an "AS IS" BASIS,
*	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
*	either express or implied.
*	See the License for the specific language governing permissions and
*	limitations under the License.
*/


#include "wmp_job_examples.h"
#include <openssl/md5.h>
#include <assert.h>
#include <netdb.h> 			// gethostbyname
#include <sys/time.h>		// gettimeofday
#include <unistd.h>			// gettimeofday
#include <fstream>
#include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <string>

using namespace std;
namespace exc = glite::wmsutils::exception;

namespace glite {
namespace wms {
namespace wmproxyapi {
namespace examples {
namespace utilities {

/*
*	removes the white spaces at the beginning and
*	at the end of the input string
*	@param str input string
*/
const char* clean(char *str)
{
    int ii = 0;
    int len = 0;
    unsigned int p = 0;
    string *s = NULL;
   // erases white space at the beginning of the string
    for (ii = 0; str[ii] == ' ' || str[ii] == '\t'; ii++);
    str = &(str[ii]);

    len = strlen (str);
    // erases white space at the end of the string
    if (len > 0) {
        for (ii = len - 1; (str[ii] == ' ' || str[ii] == '\t'); ii--);
        str[ii + 1] = '\0';
    }
    s = new string(str);
    p = s->find("\n");
    if ( p != string::npos ){
	*s = s->substr (0, ii);
    }
    return ((char*)s->c_str()) ;
}

/*
*	reads the jobid from a file
*	@param path filepath
*	@return a pointer to the  jobid string
*/
string* jobidFromFile (const string &path)
{
	ostringstream bstream;
	string *j_id = NULL;
	char ch ;

         ifstream file(path.c_str());
         if (file.is_open())   {
         	while( ! file.eof() )
                {
                        file.get(ch);
                        bstream << ch;
                }
               file.close();

		j_id = new string( bstream.str() );
		*j_id = clean ((char*)j_id->c_str());
        }

	return j_id;
}
/*
*	gets a string with messages of the input exception
*	@param b_ex input BaseException
*	@return the error message of the exception
*/
const string handle_exception (const BaseException &b_ex )
{
	string meth = b_ex.methodName.c_str();
	string *errcode =b_ex.ErrorCode ;
	string *description = b_ex.Description ;

	string excmsg = "";

	if (meth.size() > 0){
		excmsg += "method: " +meth + "\n";
	}

	if (errcode && errcode->size() > 0 ){
			excmsg += "error code:\t" + *errcode + "\n";
	}
	if (description && description->size() > 0 ){
			excmsg += "description:\t" + *description + "\n";
	}

	return excmsg;
}

/*
*	save the text in the input string buffer into a file
*	@param path filepath
*	@param bfr input string buffer
*	@return 0 in case of success; -1 in case of any error
*/
int saveToFile (const string &path, const string &bfr)
{
	int result = 0;
	char date[1024] = "";
	time_t now = time(NULL);
	struct tm *tn = localtime(&now);

 	 sprintf (date, "%d-%d-%d-%d%d%d :\n\n",
       				 tn->tm_year,
				 tn->tm_mon,
				 tn->tm_mday,
				 tn->tm_hour,
				 tn->tm_min,
				 tn->tm_sec );

	ofstream outputstream(path.c_str(), ios::app);
	if (outputstream.is_open() )
	{
		outputstream << date << ends;
		outputstream << bfr << ends;
		outputstream.close();
	} else {
		result  = -1;
	}
	return result ;
}

/*
*	contacts the endpoint configurated in the context
*	in order to retrieve the http(s) destionationURI of the job
*	identified by jobid
*	@param jobid the identifier of the job
*	@param cfs context
*	@return a pointer to the string with the http(s) destinationURI (or NULL in case of error)
*
*/
string* getInputSBDestURI(const string &jobid, const ConfigContext *cfs)
{
	string *dest_uri = NULL;
	vector <string> dest ;
	vector <string>::iterator it ;

	if (cfs){
		dest = getSandboxDestURI (jobid, (ConfigContext *)cfs);
		for (it = dest.begin() ; it != dest.end() ; it++){
			dest_uri = new string( *it );
			if ( dest_uri->substr (0, 4) == "http" ){
				break;
			} else {
				dest_uri = NULL;
			}
		}
		//input DestURI
		if (dest_uri){
			*dest_uri += "/input" ;
		} else{
			cerr << "fatal error: couldn't retrieve the DestinationURI information for the job : " << jobid << "\n";
			exit(-1);
		}
	}
	return dest_uri;
}

/*
*	checks if a file exists
*	@param file_in the file to be checked
*	@return a pointer to the string with the filepath (NULL if it doesn't exist)
*/
string* checkFileExistence(const string* file_in){
	ifstream f_in( file_in->c_str());
	if (  !f_in.good() ){
		return NULL;
	}else{
		return (new string(*file_in));
	}
}


/*
*	performs user proxy delegation with del_id delegation identifier string
*	@param cfs context (proxy, endpoint, trusted certificates)
*	@param del_id identifier of the delegation operation
*	@param verbose verbosity
*/
const std::string makeDelegation (ConfigContext *cfs, const string &del_id, const bool &verbose){
	// gets Proxy
	string proxy = getProxyReq(del_id, cfs) ;
	// sends the proxy to the endpoint service
	putProxy(del_id, proxy, cfs);
	if (verbose){
		cout << "proxy successfully delegated with delegation-id : " << del_id << "\n";
	}
        return proxy;
}
/*
*	base64 encoding methods
*/
static const int base64_encode(void *enc, int enc_size, char *out,  int out_max_size)
{
    static const char* b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    unsigned char* enc_buf = (unsigned char*)enc;
    int out_size = 0;
    unsigned int  bits = 0;
    unsigned int  shift = 0;
    while ( out_size < out_max_size ) {
	if ( enc_size>0 ) {
	    // Shift in byte
	    bits <<= 8;
	    bits |= *enc_buf;
	    shift += 8;
	    // Next byte
	    enc_buf++;
	    enc_size--;
	} else if ( shift>0 ) {
	    // Pad last bits to 6 bits - will end next loop
	    bits <<= 6 - shift;
	    shift = 6;
	} else {
	    // Terminate with Mime style '='
	    *out = '=';
	    out_size++;
	    return out_size;
	}
	// Encode 6 bit segments
	while ( shift>=6 ) {
	    shift -= 6;
	    *out = b64[ (bits >> shift) & 0x3F ];
	    out++;
	    out_size++;
	}
    }
    // Output overflow
    return -1;
}

const char *str2md5base64(const char *s)
{
    MD5_CTX md5;
    unsigned char d[16];
    char buf[50];
    int l;
    MD5_Init(&md5);
    MD5_Update(&md5, s, strlen(s));
    MD5_Final(d, &md5);
    l = base64_encode(d, 16, buf, sizeof(buf) - 1);
    if (l < 1) {
	return NULL;
   }
    buf[l - 1] = 0;
    return strdup(buf);
}

/*
*	peforms proxy delegation automatically generating the delegation identifier string
*	@param cfs context (proxy, endpoint, trusted certificates)
*	@param verbose verbosity
*/
string* makeAutomaticDelegation (ConfigContext *cfs, const bool &verbose){

	char hostname[1000]; /* used to hold string for encrypt */
	char *del_id ;
	//automatic generation of delegation string
	gethostname(hostname, 100);
	del_id = (char*)str2md5base64(hostname);
	if (verbose){
		cout << "\nautomatic proxy delegation ....\n";
		cout << "delegation id =" << del_id << "\n";
	}
	// get/put Proxy
	makeDelegation (cfs, del_id);
	//gets back the generated del-id
	return (new string(del_id));
}

/*
*	checks if the user free quota could support (in size) the transferring of a set of files
*	to the endpoint specified in the context
*	@param files the set of files to be transferred
*	@param cfs context (proxy, endpoint, trusted certificates)
*	@param verbose verbosity
*/
bool checkFreeQuota ( const vector<pair<string,string> > &files, ConfigContext *cfs, const bool &verbose )
 {
	int hd = 0;
	string file = "";
	struct stat file_info;
	FILE* fptr = NULL;
	bool not_exceed = true;
	// results of getFrewQuota
	pair<long, long> free_quota ;
	long soft_limit = 0;
	long size = 0;
	// gets the user free quota from the endpoint
	free_quota = getFreeQuota(cfs);

	// soft limit
	soft_limit = free_quota.first;
	// if the previous function gets back a negative number
	// the user free quota is not set on the server and
	// no check is performed (this functions returns not exceed=true)
	if ( soft_limit  > 0 ) {
		// number of bytes to be transferred
		for ( unsigned int i = 0 ; i < files.size( ) ; i++ ) {
			// size of the file
			file = files[i].first ;
			fptr = fopen(file.c_str(), "rb");
			if (fptr == NULL) {
				cerr << "\nerror : InputSandbox file not found (" << file<< ")" << endl;
				exit(-1);
			}
			hd = open(file.c_str(), O_RDONLY) ;
			fstat(hd, &file_info);
			close(hd) ;
			// adds up the size of the size
			size += file_info.st_size ;
			if ( size > soft_limit) {
				not_exceed = false;
				fclose (fptr);
				break;
			}
			fclose (fptr);
		}
		if ( verbose ) {
			cout << "\nuser free quota = " << soft_limit;
			if ( not_exceed ){
				cout << " ok\n";
				cout << "\nfree quota is " << soft_limit << " byte(s)\nInputSandbox files size is " << size << " byte(s)." << endl;
			} else{
				cout << "failed: over-quota (not enough user free quota to transfer the file(s) )\n";
			}
		}
	}
	return not_exceed ;
 }

 string normalize_path( const string &fpath )
{
  string                   modified;
  string::const_iterator   last, next;
  string::reverse_iterator check;

  last = fpath.begin();
  do {
    next = find( last, fpath.end(), '/' );

    if( next != fpath.end() ) {
      modified.append( last, next + 1 );

      for( last = next; *last == '/'; ++last );
    }
    else modified.append( last, fpath.end() );
  } while( next != fpath.end() );

  check = modified.rbegin();
  if( *check == '/' ) modified.assign( modified.begin(), modified.end() - 1 );

  return modified;
}

string addWildCards2Path(string& path, const string &wc){
	const string WILDCARDS []={"*","[","]","{","}","?","$"} ;
        const int size = sizeof(WILDCARDS);
        int i = 0;
        for ( i = 0 ; i < size; i++ ){
		if ( wc.compare (WILDCARDS [i] ) == 0 ){
			break;
                }
        }
	if ( i == size ){
		throw exc::Exception (__FILE__, __LINE__,
                	"glite::wms::wmproxyapi::examples::utilities::addWildCards2Path" ,
                        -1,
                        "invalid wildcard (" + wc +")" );
        }
        if ( path.compare( path.length()-1, 1, "/" ) != 0 ){
                path += "/";
        }
        path += "*";
	return path;
 }
} //glite
}//wms
}//wmproxyapi
}//examples
}//utilities
