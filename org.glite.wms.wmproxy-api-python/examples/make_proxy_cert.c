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

/// Make a GSI Proxy chain from a request, certificate and private key
/**
 *  The proxy chain is returned in *proxychain. If debugfp is non-NULL,
 *  errors are output to that file pointer. The proxy will expired in
 *  the given number of minutes starting from the current time.
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include <stdarg.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>
#include <errno.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/des.h>
#include <openssl/rand.h>

#define GRST_MAX_CHAIN_LEN       9
#define GRST_RET_FAILED         1000
#define GRST_RET_OK                     0


const char* PROXY_REQUEST="PROXY_REQUEST";
const char* output_file = "proxyresult" ;
const char* env = "-e";
const char* proxyinp = "-p";
const char* output = "-o";
bool envOpt = false ;
bool proxyOpt = false ;
bool outputOpt = false ;


static void mpcerror(FILE *debugfp, char *msg)
{
  if (debugfp != NULL)
    {
      fputs(msg, debugfp);
      ERR_print_errors_fp(debugfp);
    }
}


int checkOption (const char* option) {

  if ( strcmp( option , env ) == 0 )
  {
  envOpt = true;
  return 1;
  }
  else if ( strcmp (option , proxyinp) == 0 )
  {
  proxyOpt = true;
  return 1;
  }
  else if ( strcmp (option , output ) == 0 )
  {
  outputOpt = true;
  return 1;
  }
  else
  {
      printf("Wrong input option, please use:\n");
      printf("-e <environment variable>\n");
      printf("-p <proxy string>\n");
      printf("-o <output file>\n");
      fflush(stdout);
      return 0;
    }
  return 0;
}


void HELP_USAGE () {

       printf("-e  <environment variable>\n");
       printf("-p  <proxy string>\n");
       printf("-o  <output file>\n");
       fflush(stdout);
}



time_t GRSTasn1TimeToTimeT(char *asn1time, size_t len)
{
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


const long getCertTimeLeft(const char* pxfile ) {
	time_t timeleft = 0;
	BIO *in = NULL;
	X509 *x = NULL;
	in = BIO_new(BIO_s_file());
	if (in) {
		BIO_set_close(in, BIO_CLOSE);
		if (BIO_read_filename( in, pxfile ) > 0) {
			x = PEM_read_bio_X509(in, NULL, 0, NULL);
				if (x) {
					timeleft = ( ASN1_UTCTIME_get   ( X509_get_notAfter(x))  - time(NULL) ) / 60 ;
				} else {
            				printf("getProxyTimeLeft: unable to read X509 proxy file\n");
      					return GRST_RET_FAILED;
    				}
 			} else {
              			printf("getProxyTimeLeft: unable to open X509 proxy file\n");
      				return GRST_RET_FAILED;
		}
		BIO_free(in);
		free(x);
	} else {
              	printf("getProxyTimeLeft: unable to allocate memory for the proxy file\n");
      		return GRST_RET_FAILED;
	}
        return timeleft;
}



int makeProxyCert(char **proxychain, FILE *debugfp,
                          char *reqtxt, char *cert, char *key, int minutes)
{
  char *ptr, *certchain;
  int i, /*subjAltName_pos, */ncerts;
  long serial = 2796, ptrlen;
  EVP_PKEY *pkey, *CApkey;
  const EVP_MD *digest;
  X509 *certs[GRST_MAX_CHAIN_LEN];
  X509_REQ *req;
  X509_NAME *name, *CAsubject, *newsubject;
  X509_NAME_ENTRY *ent;
  FILE *fp;
  FILE *proxyresult;
  BIO *reqmem , *certmem;
  time_t notAfter;

  /* read in the request */
  reqmem = BIO_new(BIO_s_mem());
  BIO_puts(reqmem, reqtxt);

  if (!(req = PEM_read_bio_X509_REQ(reqmem, NULL, NULL, NULL)))
    {
      mpcerror(debugfp,
              "MakeProxyCert(): error reading request from BIO memory\n");
      BIO_free(reqmem);
      return GRST_RET_FAILED;
    }

  BIO_free(reqmem);

  /* verify signature on the request */
  if (!(pkey = X509_REQ_get_pubkey (req)))
    {
      mpcerror(debugfp,
              "MakeProxyCert(): error getting public key from request\n");
      return GRST_RET_FAILED;
    }

  /* read in the signing certificate */
  if (!(fp = fopen(cert, "r")))
    {
      mpcerror(debugfp,
            "MakeProxyCert(): error opening signing certificate file\n");
      return GRST_RET_FAILED;
    }

  for (ncerts = 1; ncerts < GRST_MAX_CHAIN_LEN; ++ncerts)
   if (!(certs[ncerts] = PEM_read_X509(fp, NULL, NULL, NULL))) break;

  if (ncerts == 1) /* zeroth cert with be new proxy cert */
    {
      mpcerror(debugfp,
            "MakeProxyCert(): error reading signing certificate file\n");
      return GRST_RET_FAILED;
    }

  fclose(fp);

  CAsubject = X509_get_subject_name(certs[1]);

  /* read in the CA private key */
  if (!(fp = fopen(key, "r")))
    {
      mpcerror(debugfp,
            "MakeProxyCert(): error reading signing private key file\n");
      return GRST_RET_FAILED;
    }

  if (!(CApkey = PEM_read_PrivateKey (fp, NULL, NULL, NULL)))
    {
      mpcerror(debugfp,
            "MakeProxyCert(): error reading signing private key in file\n");
      return GRST_RET_FAILED;
    }

  fclose(fp);

  /* get subject name */
  if (!(name = X509_REQ_get_subject_name (req)))
    {
      mpcerror(debugfp,
            "MakeProxyCert(): error getting subject name from request\n");
      return GRST_RET_FAILED;
    }

  /* create new certificate */
  if (!(certs[0] = X509_new ()))
    {
      mpcerror(debugfp,
            "MakeProxyCert(): error creating X509 object\n");
      return GRST_RET_FAILED;
    }

  /* set version number for the certificate (X509v3) and the serial number
     need 3 = v4 for GSI proxy?? */
  if (X509_set_version (certs[0], 3L) != 1)
    {
      mpcerror(debugfp,
            "MakeProxyCert(): error setting certificate version\n");
      return GRST_RET_FAILED;
    }

  ASN1_INTEGER_set (X509_get_serialNumber (certs[0]), serial++);

  if (!(name = X509_get_subject_name(certs[1])))
    {
      mpcerror(debugfp,
      "MakeProxyCert(): error getting subject name from CA certificate\n");
      return GRST_RET_FAILED;
    }

  if (X509_set_issuer_name (certs[0], name) != 1)
    {
      mpcerror(debugfp,
      "MakeProxyCert(): error setting issuer name of certificate\n");
      return GRST_RET_FAILED;
    }

  /* set issuer and subject name of the cert from the req and the CA */
  ent = X509_NAME_ENTRY_create_by_NID( NULL, OBJ_txt2nid("commonName"), MBSTRING_ASC, "proxy", -1);

  newsubject = X509_NAME_dup(CAsubject);

  X509_NAME_add_entry(newsubject, ent, -1, 0);

  if (X509_set_subject_name(certs[0], newsubject) != 1)
    {
      mpcerror(debugfp,
      "MakeProxyCert(): error setting subject name of certificate\n");
      return GRST_RET_FAILED;
    }

  /* set public key in the certificate */
  if (X509_set_pubkey(certs[0], pkey) != 1)
    {
      mpcerror(debugfp,
      "MakeProxyCert(): error setting public key of the certificate\n");
      return GRST_RET_FAILED;
    }

  /* set duration for the certificate */
  if (!(X509_gmtime_adj (X509_get_notBefore(certs[0]), -300)))
    {
      mpcerror(debugfp,
      "MakeProxyCert(): error setting beginning time of the certificate\n");
      return GRST_RET_FAILED;
    }

  if (!(X509_gmtime_adj (X509_get_notAfter(certs[0]), 60 * minutes)))
    {
      mpcerror(debugfp,
      "MakeProxyCert(): error setting ending time of the certificate\n");
      return GRST_RET_FAILED;
    }

  /* go through chain making sure this proxy is not longer lived */

  notAfter = GRSTasn1TimeToTimeT(ASN1_STRING_data(X509_get_notAfter(certs[0])), 0);

  for (i=1; i < ncerts; ++i)
       if (notAfter >
           GRSTasn1TimeToTimeT(ASN1_STRING_data(X509_get_notAfter(certs[i])),
                               0))
         {
           notAfter =
            GRSTasn1TimeToTimeT(ASN1_STRING_data(X509_get_notAfter(certs[i])),
                                0);

           ASN1_UTCTIME_set(X509_get_notAfter(certs[0]), notAfter);
         }

  /* sign the certificate with the signing private key */
  if (EVP_PKEY_type (CApkey->type) == EVP_PKEY_RSA)
    digest = EVP_md5();
  else
    {
      mpcerror(debugfp,
      "MakeProxyCert(): error checking signing private key for a valid digest\n");
      return GRST_RET_FAILED;
    }

  if (!(X509_sign (certs[0], CApkey, digest)))
    {
      mpcerror(debugfp,
      "MakeProxyCert(): error signing certificate\n");
      return GRST_RET_FAILED;
    }

  /* store the completed certificate chain */

  certchain = strdup("");

  for (i=0; i < ncerts; ++i)
     {
       certmem = BIO_new(BIO_s_mem());

       if (PEM_write_bio_X509(certmem, certs[i]) != 1)
         {
           mpcerror(debugfp,
            "MakeProxyCert(): error writing certificate to memory BIO\n");
           return GRST_RET_FAILED;
         }

       ptrlen = BIO_get_mem_data(certmem, &ptr);
       certchain = realloc(certchain, strlen(certchain) + ptrlen + 1);

       strncat(certchain, ptr, ptrlen);

       BIO_free(certmem);
     }

  *proxychain = certchain;

   //prints the proxy processed in a file "proxyresult"
   proxyresult = fopen(output_file, "w+");
   fprintf(proxyresult, *proxychain);
   fclose(proxyresult);

  return GRST_RET_OK;
}


char* findProxyFileName(void)
{
  char *p;

  p = getenv("X509_USER_PROXY");

  if (p != NULL) return strdup(p);

  p = malloc(sizeof("/tmp/x509up_uXYYYXXXYYY"));

  sprintf(p, "/tmp/x509up_u%d", getuid());

  return p;
}


int main(int argc, char *argv[])
 {
  char *certtxt;
  time_t timeleft;
  const char* request = NULL;
  const char* proxy = findProxyFileName();
  int optind = 1;

  if ( ( argc < 3 ) || ( checkOption(argv[optind]) == 0))
      {
       printf("Mandatory option or his argument missing, please use:\n");
       printf("-e  <environment variable>\n");
       printf("-p  <proxy string>\n" );
       fflush(stdout);
       return GRST_RET_FAILED;
     }
     else if ( argc > 7 )
     {
       printf( "Too many arguments, please use:\n");
       HELP_USAGE();
       fflush(stdout);
       return GRST_RET_FAILED;
     }
     else {
        while ( optind < argc )
	{
	if (checkOption (argv[optind]) == 1) {
          if ( envOpt == true) {
	   // Reads the proxy request from ENV
    	   PROXY_REQUEST = (char*)argv[optind+1] ;
           request = getenv (PROXY_REQUEST);
    	  } else if ( proxyOpt == true) {
	   // Reads the proxy request from input as a string
    	   request = (char*)argv[optind+1] ;
   	   } else if ( outputOpt == true ) {
	   // Sets the output file 
     	   output_file = (char*)argv[optind+1] ;
   	   } else
	   {
           printf( "Wrong input option, please use:\n");
	   HELP_USAGE();
           fflush(stdout);
           return GRST_RET_FAILED;
    	   }
         }
         optind = optind +2 ;
        }
  }

  if ((envOpt == true) && (proxyOpt == true))
   {
    printf( "The following options cannot be specified together, please use:\n");
    printf("-e  <environment variable>\n");
    printf("-p  <proxy string>\n");
    fflush(stdout);
    return GRST_RET_FAILED;
   }

  if ( request == NULL )
     {
       printf( "Error while reading proxy request from input: NULL\n");
       HELP_USAGE();
       fflush(stdout);
       return GRST_RET_FAILED;
     }

  if (proxy == NULL)
     {
       printf( "Error while reading proxy from input: NULL\n");
       HELP_USAGE();
       fflush(stdout);
       return GRST_RET_FAILED;
     }
  timeleft = getCertTimeLeft(proxy);
  // makes certificate
  if ( makeProxyCert( &certtxt, stderr, (char*)request, (char*) proxy, (char*)proxy, timeleft) )
   {
    printf("MakeProxyCert(): function failed\n");
    fflush(stdout);
    return GRST_RET_FAILED;
    }
   return GRST_RET_OK;
  }
