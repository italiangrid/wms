/* Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>

#include "glite/wms/common/configuration/JCConfiguration.h"

#include "CondorG.h"

USING_COMMON_NAMESPACE;
using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

CondorG      *CondorG::cg_s_instance = NULL;
const char   *CondorG::cg_s_commands[] = {
  "Unknown command",
  "Submit", "Remove", "Release"
};

CondorG::CondorG( const configuration::JCConfiguration *config ) : cg_submit( config->condor_submit() ),
								   cg_remove( config->condor_remove() ), cg_release( config->condor_release() ),
								   cg_command()
{
  if( cg_s_instance == NULL ) cg_s_instance = this;
}

CondorG::~CondorG( void )
{
  if( cg_s_instance == this ) cg_s_instance = NULL;
}

CondorG *CondorG::set_command( command_t command, const string &parameter )
{
  switch( command ) {
  case unknown:
  default:
    this->cg_command.erase();

    break;
  case submit:
    this->cg_command.assign( this->cg_submit ); this->cg_command.append( 1, ' ' );
    this->cg_command.append( parameter );
    this->cg_command.append( " 2>&1" );

    break;
  case remove:
    this->cg_command.assign( this->cg_remove ); this->cg_command.append( 1, ' ' );
    this->cg_command.append( parameter );
    this->cg_command.append( " 2>&1" );

    break;
  case release:
    this->cg_command.assign( this->cg_release); this->cg_command.append( 1, ' ' );
    this->cg_command.append( parameter );
    this->cg_command.append( " 2>&1" );

    break;
  }

  return this;
}

int CondorG::execute( std::string &info )
{
  int                         result = -1;
  FILE                       *fp;
  char                       *pc, buffer[BUFSIZ];
  boost::mutex::scoped_lock   lock( this->cg_mutex );

  if( this->cg_command.size() == 0 ) info.assign( "Command not set." );
  else {
    if( (fp = popen(this->cg_command.c_str(), "r")) == NULL ) {
      info.assign( "Cannot open pipe" );
      result = -1;
    }
    else {
      info.erase();

      while( (pc = fgets(buffer, BUFSIZ, fp)) != NULL ) info.append( pc );

      result = pclose( fp );
    }
  }

  return result;
}

}

} JOBCONTROL_NAMESPACE_END
