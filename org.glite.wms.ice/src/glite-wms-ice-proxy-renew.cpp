/* LICENSE:
Copyright (c) Members of the EGEE Collaboration. 2010. 
See http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

   http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. 
See the License for the specific language governing permissions and 
limitations under the License.

END LICENSE */

#include <cstdlib>
#include <cstdio>
#include <getopt.h>
#include <glite/security/proxyrenewal/renewal_core.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <globus_gsi_credential.h>
#include <globus_gsi_proxy.h>
#include <globus_gsi_cert_utils_constants.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

/* workaround for gsoap 2.7.13 */
#include "glite/ce/cream-client-api-c/cream_client_soapH.h"
SOAP_NMAC struct Namespace namespaces[] = {};

using namespace std;

static volatile int die = 0, child_died = 0;

static void
catchsig(int sig)
{
   switch (sig) {
      case SIGINT:
      case SIGTERM:
      case SIGQUIT:
         die = sig;
         break;
      case SIGCHLD:
         child_died = 1;
         break;
      default:
         break;
   }
}

static struct option const long_options[] = {
   { "server",   required_argument, 0, 's' },
   { "proxy",    required_argument, 0, 'p' },
   { "help",     no_argument,       0, 'h' },
   { "output",   required_argument, 0, 'o' },
   { "timeout",  required_argument, 0, 't' },
   { NULL, 0, NULL, 0}
};

static char short_options[] = "s:p:h:o:";

int
main(int argc, char *argv[], char *envp[])
{
   char *server = NULL;
   char *proxy = NULL;
   char *new_proxy = NULL;
   extern int optind;
   char arg;
   glite_renewal_core_context ctx = NULL;
   int ret;
   int timeout = 60;
   char *outputfile = NULL;

   while ((arg = getopt_long(argc, argv, short_options, long_options, NULL)) != EOF) {
      switch(arg) {
      case 's':
	server = optarg; break;
      case 'p':
	proxy = optarg; break;
      case 'o':
	outputfile = optarg;break;
      case 't':
        timeout = atoi(optarg); break;
      case 'h':
	//fprintf(stdout, "Usage: %s --server <myproxy server> --proxy <filename>\n", argv[0]);
	cerr << "Usage: "
	     << argv[0] << " --server <myproxy server> --proxy <filename> --output <output file proxy>\n";
	return 1;
      
	
      }
   }

   if (server == NULL || proxy == NULL || outputfile == NULL) {
     cout << "both server and proxy parameters must be given and output file name\n";
     return 1;
   }

   if(!boost::filesystem::exists( boost::filesystem::path(proxy,boost::filesystem::native) )) {
     cerr << "Proxy file [" << proxy << "] doest not exist" << endl;
     return 1;
   }


   ret = glite_renewal_core_init_ctx(&ctx);
   if (ret) {
     fprintf(stderr, "glite_renewal_core_init_ctx() failed\n");
     cout << "glite_renewal_core_init_ctx() failed\n";
     return 1;
   }

   ctx->log_dst = GLITE_RENEWAL_LOG_NONE;

   globus_module_activate(GLOBUS_GSI_CERT_UTILS_MODULE);
   globus_module_activate(GLOBUS_GSI_PROXY_MODULE);

   struct sigaction     sa;
   
   sigset_t mask;

   memset(&sa,0,sizeof(sa));
   sa.sa_handler = catchsig;
   sigaction(SIGINT,&sa,NULL);
   sigaction(SIGQUIT,&sa,NULL);
   sigaction(SIGTERM,&sa,NULL);
   sigaction(SIGCHLD,&sa,NULL);
   sigaction(SIGPIPE,&sa,NULL);

   sigemptyset(&mask);
   sigaddset(&mask, SIGINT);
   sigaddset(&mask, SIGQUIT);
   sigaddset(&mask, SIGTERM);
   sigaddset(&mask, SIGCHLD);
   sigaddset(&mask, SIGPIPE);
   sigprocmask(SIG_UNBLOCK, &mask, NULL);

   map<string, string> envMap;
 
   for (char **env = envp; *env != 0; ++env) {
    //std::cout << "**** ENVP **** " << *env << '\n';
    vector<string> pieces;
    boost::split(pieces, *env, boost::is_any_of("="));
    if(pieces.size()==2) {
      envMap[pieces[0]] = pieces[1];
    }
   }  
/* 
   if(envp[0] && envp[1])
     ::setenv( envp[0], envp[1], 1);

   if(envp[2] && envp[3])
     ::setenv( envp[2], envp[3], 1);
*/ 

   map<string, string>::const_iterator it = envMap.begin();
   for( ; it != envMap.end(); ++it ) {
	if(it->first == "X509_USER_CERT")
	  ::setenv( "X509_USER_CERT", it->second.c_str(), 1 );
        if(it->first == "X509_USER_KEY")
          ::setenv( "X509_USER_KEY", it->second.c_str(), 1 );
   }

   pid_t pid = ::getpid();
   string outputstream = string("/tmp/glite-wms-ice-proxy-renew.output.") + boost::lexical_cast<string>( pid );
   {
     ofstream out;
     out.open(outputstream.c_str(), ios::trunc);
     out << "OK " << endl;
   }
   pid_t retchld = fork();
   if ( retchld == -1 ) {
     ofstream out;
     out.open(outputstream.c_str(), ios::trunc);
     out << "ERROR - There's a problem with fork(): " << strerror(errno) << endl;
     return 1;
   }
   
   if (retchld == 0) {
     // child process that has to renew the proxy
     
     ret = glite_renewal_core_renew(ctx, server, 0, proxy, &new_proxy);
     if (ret) {
       ofstream out;
       out.open(outputstream.c_str(), ios::trunc);
       out << "ERROR - " << argv[0] <<": glite_renewal_core_renew() failed: "
           << ctx->err_message << " - timeout=[" << timeout << "] - myproxyserver=["<<server<<"] - proxy=["<<proxy << "]" 
	   << " - HOSTCERT="<< (::getenv("X509_USER_CERT") ? ::getenv("X509_USER_CERT") : "" )
	   << " - HOSTKEY=" << (::getenv("X509_USER_KEY") ? ::getenv("X509_USER_KEY") : "" )
	   << endl;
       return 1;
     }
   
     ret = glite_renewal_core_destroy_ctx(ctx);

     ::rename( new_proxy, outputfile );

     ::chmod( outputfile, S_IRUSR | S_IWUSR );
	
     return 0;
     
   } else {
     // parent process
     int i = 0;
     while(i++<timeout) {
       //cout << "sleep " << i<<endl;
       sleep(1);
       int status;
       int retwaitpid = ::waitpid(retchld, &status, WNOHANG|WUNTRACED);
       if(retwaitpid == retchld) // the child finished (changed status)
         {
	   //cout << "Child finished. Child's return status=" << status << endl;
	   if(status == 0)
	     return 0;
	   else {
	     return 1;
	   }
	 }
     }
     kill(retchld, SIGKILL);
     
     //cout << argv[0] << " killed the renewal child after timeout of " << timeout << " seconds. The proxy " << proxy << " has NOT been renewed! " << endl;
     ofstream out;
     out.open(outputstream.c_str(), ios::trunc);
     out << "ERROR - " << argv[0] << " killed the renewal child after timeout of " << timeout << " seconds. The proxy " << proxy << " has NOT been renewed! " << endl;
     return 1;
   }
   return 0;
}
