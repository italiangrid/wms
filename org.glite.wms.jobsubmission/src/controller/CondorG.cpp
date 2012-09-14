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
  "Submit", "Remove"
};

CondorG::CondorG( const configuration::JCConfiguration *config ) : cg_submit( config->condor_submit() ),
								   cg_remove( config->condor_remove() ),
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
  }

  return this;
}

int CondorG::execute(std::string &info)
{
  int                         result = -1;
  FILE                       *fp;
  char                       *pc, buffer[BUFSIZ];
  boost::mutex::scoped_lock   lock( this->cg_mutex );

  if (this->cg_command.size() == 0 ) {
    info = "Command not set.";
  } else {
    if( (fp = popen(this->cg_command.c_str(), "r")) == NULL ) {
      info = "Cannot open pipe";
      result = -1;
    } else {
      info.erase();
      while ((pc = fgets(buffer, BUFSIZ, fp)) != NULL ) {
        info += pc;
      }
      result = pclose(fp);
    }
  }

  return result;
}

};

} JOBCONTROL_NAMESPACE_END;
