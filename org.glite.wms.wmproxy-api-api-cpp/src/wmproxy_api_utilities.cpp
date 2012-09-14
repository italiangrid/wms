// Copyright (c) Members of the EGEE Collaboration. 2004. 
// See http://www.eu-egee.org/partners/ for details on the copyright
// holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 

//     http://www.apache.org/licenses/LICENSE-2.0 

// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

#include <cstring>
#include <openssl/pem.h>
#include <vector>
#include <iostream>
#include <sstream> //string streams
#include <fstream> // file streams
#include <stdlib.h> // getenv(...)
#include <unistd.h> // getuid()
#include <sys/types.h> // getuid()
#include <ctype.h>
#include "glite/wms/wmproxyapi/wmproxy_api_utilities.h"


namespace glite {
namespace wms {
namespace wmproxyapiutils {

using namespace std ;
using namespace glite::wms::wmproxyapi;
/*
* Gets the current time
* @return the seconds elapsed since Jan 1, 1970
*/
const time_t getTime( ){
	time_t now = time(NULL);
	struct tm *ns = localtime(&now);
	return mktime (ns);
}

/**
Checks the existance of the input pathname
**/
const char* checkPathExistence(const char* path){
	ifstream f_in(path);
	if (  !f_in.good() ) return NULL;
	else return path;
}

/*****************************************************************
Calculates the  Trusted Certificates directory pathname
******************************************************************/
const char* getTrustedCert(ConfigContext *cfs){
	char *path = NULL ;
	if (cfs!=NULL){
        	if (cfs->trusted_cert_dir!=""){
                	path = (char*)checkPathExistence(cfs->trusted_cert_dir.c_str());
                 }
         } else {
		path = getenv ("X509_CERT_DIR");
                if ( !path ){
			path =(char*) checkPathExistence( "/etc/grid-security/certificates");
                }
         }
        return path ;
}
/*****************************************************************
Calculates the  Proxy file pathname
******************************************************************/
const char* getProxyFile(ConfigContext *cfs){
	char* path = NULL;
	if (cfs!=NULL){
        	if (cfs->proxy_file!=""){
                	path = (char*)checkPathExistence(cfs->proxy_file.c_str());
                 }
	 } else {
		const char * env_proxy = getenv ("X509_USER_PROXY");
		if(env_proxy!=NULL){
                	path = (char*)checkPathExistence(env_proxy);
		}else{
			// Append UID to X509 default file
			stringstream uid_string;
			uid_string << getuid() ;
			string result ="/tmp/x509up_u" + uid_string.str();
			path = (char*)checkPathExistence( result.c_str() );
		}
	}
        return path ;
}

/*****************************************************************
Calculates the EndPoint URL
******************************************************************/
const char* getEndPoint (ConfigContext *cfs){
	char* ep = NULL;
	if (cfs!=NULL){
		ep = (char*)(cfs->endpoint).c_str();
	 } 
        return ep ;
}

/*************************************************************************
	private method:	converts ASN1_UTCTIME to time_t
*************************************************************************/
time_t ASN1_UTCTIME_get(const ASN1_UTCTIME *s)
        {
        struct tm tm;
        int offset;
        memset(&tm,'\0',sizeof tm);
#define g2(p) (((p)[0]-'0')*10+(p)[1]-'0')
        tm.tm_year=g2(s->data);
        if(tm.tm_year < 50)
                tm.tm_year+=100;
        tm.tm_mon=g2(s->data+2)-1;
        tm.tm_mday=g2(s->data+4);
        tm.tm_hour=g2(s->data+6);
        tm.tm_min=g2(s->data+8);
        tm.tm_sec=g2(s->data+10);
        if(s->data[12] == 'Z')
                offset=0;
        else
                {
                offset=g2(s->data+13)*60+g2(s->data+15);
                if(s->data[12] == '-')
                        offset= -offset;
                }
#undef g2
    return timegm(&tm)-offset*60;
}

/*************************************************************************
* private method: checks if the vector v contains the "elem" string
*************************************************************************/
bool contains(const std::vector<pair<std::string,long > > &v, const std::string elem) {
	bool found = false;
	for (unsigned int i=0 ; i < v.size(); i++ ){
		if ( v[i].first == elem  ){
			found = true;
			break;
		}
	}
	return found;
}

bool contains(const std::vector<std::string> &v, const std::string elem) {
	bool found = false;
	for (unsigned int i=0 ; i < v.size(); i++ ){
		if ( v[i] == elem  ){
			found = true;
			break;
		}
	}
	return found;
}

/*****************************************************************************************
* private method: return a long representing the Epoch seconds  for
* the input date in the format yyyymmddhhmmssZ
* ( Z=time zone)
**************************************************************************************/
long asn1TimeToTimeT(char *asn1time, size_t len) {
   char   zone;
   struct tm time_tm;

   if (len == 0) len = strlen(asn1time);

   if ((len != 13) && (len != 15)) return 0; /* dont understand */

   if ((len == 13) &&
       ((sscanf(asn1time, "%02d%02d%02d%02d%02d%02d%c",
         &(time_tm.tm_year),
         &(time_tm.tm_mon),
         &(time_tm.tm_mday),
         &(time_tm.tm_hour),
         &(time_tm.tm_min),
         &(time_tm.tm_sec),
         &zone) != 7) || (zone != 'Z'))) return 0; /* dont understand */

   if ((len == 15) &&
       ((sscanf(asn1time, "20%02d%02d%02d%02d%02d%02d%c",
         &(time_tm.tm_year),
         &(time_tm.tm_mon),
         &(time_tm.tm_mday),
         &(time_tm.tm_hour),
         &(time_tm.tm_min),
         &(time_tm.tm_sec),
         &zone) != 7) || (zone != 'Z'))) return 0; /* dont understand */

   /* time format fixups */

   if (time_tm.tm_year < 90) time_tm.tm_year += 100;
   --(time_tm.tm_mon);

   return timegm(&time_tm);
}


/*************************************************************************
* private method: parses the proxy info and extracts the fqan's
*************************************************************************/
const string proxy_parser (BIO *bp, const unsigned char **pp,
			long length, int offset, int depth, int indent, int dump,
			std::vector<std::string> &fqans, std::vector<std::pair<std::string, long> > &exps) {
	const unsigned char *p,*ep,*tot,*op,*opp;
	long len;
	int tag,xclass;
	int nl,hl,j,r = 0;
	ASN1_OBJECT *o=NULL;
	ASN1_OCTET_STRING *os=NULL;
	int dump_indent;
	string fqan = "";
	time_t d1 = 0;
	time_t d2 = 0;
	static long left = -1;
#if 0
	dump_indent = indent;
#else
	// Because we know BIO_dump_indent()
	dump_indent = 6;
#endif
	p= *pp;
	tot=p+length;
	op=p-1;
	while ((p < tot) && (op < p))
	{
		op=p;
		j=ASN1_get_object(&p,&len,&tag,&xclass,length);
		if (j & 0x80){
			break;
		}
		hl=(p-op);
		length-=hl;
		if (j & V_ASN1_CONSTRUCTED) {
			ep=p+len;
			if ((j == 0x21) && (len == 0)){
				for (;;) {
						fqan=proxy_parser(bp,&p,(long)(tot-p),
							offset+(p - *pp),depth+1,
							indent,dump, fqans, exps);
						if (fqan.size()>0){
							break;
						}
					if ((r == 0) ||  ((r == 2) || (p >= tot)) ){
						break;
					}
				}
			}else{
				while (p < ep) {
					fqan=proxy_parser(bp,&p,(long)len,
						offset+(p - *pp),depth+1,
						indent,dump, fqans,exps);
					if (fqan.size()>0){
						break;
					}
					if (r == 0) {
						break;
					}
				}
			}
		} else if (xclass != 0) {
			p+=len;
		} else {
			nl=0;
			// Extracts info on start/expiration date
			if (tag == V_ASN1_GENERALIZEDTIME) {
				if (d1==0) {
					d1 = asn1TimeToTimeT((char*)p,len);
				} else {
					d2 = asn1TimeToTimeT((char*)p,len);
				}
			}
			// Extracts FQAN string
			if (tag == V_ASN1_OCTET_STRING){
				int i,printable=1;
				opp=op;
				os=d2i_ASN1_OCTET_STRING(NULL,&opp,len+hl);
				if (os != NULL) {
					opp=os->data;
					for (i=0; i<os->length; i++) {
						if ((	(opp[i] < ' ') &&
							(opp[i] != '\n') &&
							(opp[i] != '\r') &&
							(opp[i] != '\t')) ||
							(opp[i] > '~')) {
								printable=0;
								break;
							}
					}
					if (printable && (os->length > 0)) {
						fqan = fqan.assign((char*)os->data);
					}
					M_ASN1_OCTET_STRING_free(os);
					os=NULL;
					}
				}
			p+=len;
			if ((tag == V_ASN1_EOC) && (xclass == 0)){
				break;
			}
		}
		length-=len;

		//<FQAN-timeleft> info
		if (fqan.size()>0  &&
			left >=0 &&
			contains(exps, fqan)==false) {
			exps.push_back(make_pair(fqan,left));
			left = -1;
		}
		//<FQAN> info
		if (fqan.size()>0  &&
			contains(fqans, fqan) == false) {
			fqans.push_back(fqan);
		}
	} // while

	if (o != NULL) {
		ASN1_OBJECT_free(o);
	}
	if (os != NULL) {
		M_ASN1_OCTET_STRING_free(os);
	}
	*pp=p;
	// Time left
	if (d1>0 || d2>0 ) {
		if (d2>d1) {
			left = d2 - time(NULL) ;
		} else {
			left = d1 - time(NULL);
		}
	}
	return(fqan);
}

/**************************************************************************
* private method: returns a vector (fqans) with the list of FQAN's
in the input voms proxy
**************************************************************************/
void get_fqans (BIO *bp, const unsigned char **pp, long length, int offset,
	     int depth, int indent, int dump, std::vector<std::string> &fqans) {
	std::vector<std::pair<std::string, long> > v;
	proxy_parser (bp, pp, length, offset, depth, indent, dump, fqans, v);
}

/*************************************************************************
*  private method: returns a vector (exps) which elements are pairs
like tihs: <FQAN string, VOMS proxy time left>
*************************************************************************/
void get_expiration_info (BIO *bp, const unsigned char **pp, long length, int offset,
	     int depth, int indent, int dump, std::vector<std::pair<std::string, long> > &exps) {
	std::vector<std::string> v;
	proxy_parser (bp, pp, length, offset, depth, indent, dump, v, exps);
}

/*************************************************************************
* looks for the list of fqan's
*************************************************************************/
const vector<std::string> getFQANs(std::string pxfile){
	int i = 0;
	int lasttag=-1;
	const unsigned char *p1 ;
 	string fqan = "";
	FILE *fp = NULL ;
	X509 *cert = NULL ;
	BIO *bp = NULL;
	X509_EXTENSION *ex;
	ASN1_OBJECT *asnobject;
	ASN1_OCTET_STRING *asndata;
	vector<string> fqans ;
	// opens and reads the input proxy file
	fp = fopen(pxfile.c_str(), "r");
	if (!fp){
		throw *createWmpException(new ProxyFileException, "getFQANs", string ("no such proxy file:" + pxfile) );
	}
	cert = PEM_read_X509(fp, NULL, NULL, NULL);
	if (!cert){
		throw  *createWmpException(new ProxyFileException, "getFQANs",
			string ("unable to read X509 proxy file: "+ pxfile) ) ;
	}
	fclose(fp);
   	bp = BIO_new(BIO_s_file());
	if (!bp){
		throw  *createWmpException(new ProxyFileException, "getFQANs",
			string ("ssl error - unable to read X509 proxy file: "+ pxfile) ) ;
	}
	BIO_set_close(bp, BIO_CLOSE);
	BIO_set_fp(bp,stdout,BIO_FP_TEXT);
	// extracts the FQAN's
 	for (i = 0; i < X509_get_ext_count(cert); ++i) {
		lasttag=-1;
       		ex = X509_get_ext(cert, i);
        	asnobject = X509_EXTENSION_get_object(ex);
        	asndata = X509_EXTENSION_get_data(ex);
        	p1 = ASN1_STRING_data(asndata);
		get_fqans(bp, // bp
				&p1,//pp
				ASN1_STRING_length(asndata), // length
				0, // offset
				0, //depth
				0, //ndent
				0, //dump
				 fqans);
	}
	BIO_free(bp);
	return fqans ;
}
/*************************************************************************
* Proxy TimeLeft methods
*************************************************************************/
const long getCertTimeLeft( std::string pxfile ) {
	time_t timeleft = 0;
	BIO *in = NULL;
	X509 *x = NULL;
	in = BIO_new(BIO_s_file());
	if (in) {
		BIO_set_close(in, BIO_CLOSE);
		if (BIO_read_filename( in, pxfile.c_str() ) > 0) {
			x = PEM_read_bio_X509(in, NULL, 0, NULL);
			if (x) {
				timeleft = ( ASN1_UTCTIME_get   ( X509_get_notAfter(x))  - time(NULL) ) / 60 ;
			} else{
				throw  *createWmpException(new ProxyFileException, "getProxyTimeLeft",
					string ("unable to read X509 proxy file: "+ pxfile) ) ;
			}
		} else {

			throw  *createWmpException(new ProxyFileException, "getProxyTimeLeft",
				string ("unable to open X509 proxy file: "+ pxfile) ) ;
		}
		BIO_free(in);
		free(x);
	} else {
		throw  *createWmpException(new ProxyFileException, "getProxyTimeLeft",
				string ("unable to allocate memory for the proxy file: "+ pxfile) ) ;
	}
        return timeleft;
}

const long getProxyTimeLeft( std::string pxfile ) {
	int i = 0;
	int lasttag=-1;
	const unsigned char *p1 ;
 	string fqan = "";
	FILE *fp = NULL ;
	X509 *cert = NULL ;
	BIO *bp = NULL;
	X509_EXTENSION *ex;
	ASN1_OBJECT *asnobject;
	ASN1_OCTET_STRING *asndata;
	vector<pair<string,long > > exps ;
	long proxy_timeleft = 0;
	long voms_timeleft = 0;
	long timeleft = 0;
	fp = fopen(pxfile.c_str(), "r");
	if (!fp){
		throw *createWmpException(new ProxyFileException, "getFQANs", string ("no such proxy file:" + pxfile) );
	}
	cert = PEM_read_X509(fp, NULL, NULL, NULL);
	if (!cert){
		throw  *createWmpException(new ProxyFileException, "getFQANs",
			string ("unable to read X509 proxy file: "+ pxfile) ) ;
	}
	fclose(fp);
	proxy_timeleft = ( ASN1_UTCTIME_get   ( X509_get_notAfter(cert))  - time(NULL) )  ;
   	bp = BIO_new(BIO_s_file());
	if (!bp){
		throw  *createWmpException(new ProxyFileException, "getFQANs",
			string ("ssl error - unable to read X509 proxy file: "+ pxfile) ) ;
	}
	BIO_set_close(bp, BIO_CLOSE);
	BIO_set_fp(bp,stdout,BIO_FP_TEXT);
 	for (i = 0; i < X509_get_ext_count(cert); ++i) {
		lasttag=-1;
       		ex = X509_get_ext(cert, i);
        	asnobject = X509_EXTENSION_get_object(ex);
        	asndata = X509_EXTENSION_get_data(ex);
        	p1 = ASN1_STRING_data(asndata);
		get_expiration_info (bp, // bp
				&p1,//pp
				ASN1_STRING_length(asndata), // length
				0, // offset
				0, //depth
				0, //ndent
				0, //dump
				 exps);
	}
	// free memory
	BIO_free(bp);
	// FInal result
	if (exps.empty()==false) {
		// Reads the time-left for the main VO
		voms_timeleft = exps[0].second ;
		if (voms_timeleft < proxy_timeleft) {
			timeleft = voms_timeleft ;
		} else{
			timeleft = proxy_timeleft ;
		}
	} else {
		// no VOMS info
		timeleft = proxy_timeleft;
	}
	return timeleft ;
}

} // wmproxy namespace
} // wms namespace
} // glite namespace

