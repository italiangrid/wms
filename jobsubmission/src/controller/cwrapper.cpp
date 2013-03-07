// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

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
#include <cstdlib>

#include <string>
#include <exception>

#include <classad_distribution.h>

#include "glite/jobid/JobId.h"

#include "glite/lb/producer.h"
#include "glite/lb/context.h"

#include "JobController.h"
#include "JobControllerExceptions.h"
#include "cwrapper.h"

using namespace std;
USING_JOBCONTROL_NAMESPACE;

extern "C" {

struct edg_wljc_jobcontroller_t {
  void      *jc_context;
  char      *jc_error;
};

edg_wljc_Context edg_wljc_ContextInitialize( edg_wll_Context *logcont )
{
  controller::JobController   *cont;
  edg_wljc_jobcontroller_t    *wrapper = new edg_wljc_jobcontroller_t;

  wrapper->jc_context = NULL; wrapper->jc_error = NULL;

  try {
    cont = new controller::JobController( logcont );

    wrapper->jc_context = reinterpret_cast<void *>( cont );
  }
  catch( exception &err ) {
    string   reason( err.what() );

    wrapper->jc_error = new char[reason.length() + 1];
    strcpy( wrapper->jc_error, reason.c_str() );
  }

  return wrapper;
}

void edg_wljc_ContextFree( edg_wljc_Context wrapper )
{
  if( wrapper->jc_context ) delete reinterpret_cast<controller::JobController *>( wrapper->jc_context );

  if( wrapper->jc_error ) delete [] wrapper->jc_error;

  delete wrapper;

  return;
}

int edg_wljc_IsGood( edg_wljc_Context wrapper )
{ return( wrapper->jc_error == NULL ); }

void edg_wljc_GetError( edg_wljc_Context wrapper, char **error )
{
  if( wrapper->jc_error == NULL ) *error = strdup( "OK" );
  else *error = strdup( wrapper->jc_error );

  return;
}

void edg_wljc_ClearError( edg_wljc_Context wrapper )
{
  if( wrapper->jc_error ) {
    delete [] wrapper->jc_error;
    wrapper->jc_error = NULL;
  }

  return;
}

int edg_wljc_Submit( edg_wljc_Context wrapper, const char *classad )
{
  int    res = 1;

  if( (wrapper->jc_error == NULL) && (wrapper->jc_context != NULL) ) {
    controller::JobController  *cont = reinterpret_cast<controller::JobController *>( wrapper->jc_context );
    classad::ClassAd           *ad;
    classad::ClassAdParser      parser;

    ad = parser.ParseClassAd( classad );

    if( ad ) {
      try {
	res = cont->submit( ad );
      }
      catch( exception &err ) {
	string    reason( err.what() );

	wrapper->jc_error = new char[reason.length() + 1];
	strcpy( wrapper->jc_error, reason.c_str() );
      }
    }
    else {
      string      reason( "Cannot parse JDL, reason: \"" );
      reason.append( classad::CondorErrMsg );
      reason.append( "\"" );

      wrapper->jc_error = new char[reason.length() + 1];
      strcpy( wrapper->jc_error, reason.c_str() );
    }
  }

  return res;
}

int edg_wljc_Cancel( edg_wljc_Context wrapper, const edg_wlc_JobId id )
{
  int    res = 1;

  if( (wrapper->jc_error == NULL) && (wrapper->jc_context != NULL) ) {
    controller::JobController      *cont = reinterpret_cast<controller::JobController *>( wrapper->jc_context );

    try {
      res = cont->cancel( glite::jobid::JobId(id) );
    }
    catch( controller::ControllerError &err ) {
      string    reason( err.what() );

      wrapper->jc_error = new char[reason.length() + 1];
      strcpy( wrapper->jc_error, reason.c_str() );
    }
  }

  return res;
}

}
