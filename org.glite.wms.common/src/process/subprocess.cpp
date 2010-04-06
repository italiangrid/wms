/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://project.eu-egee.org/index.php?id=115 for details on the
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

#include <signal.h>
#include <sys/wait.h>

#include "subprocess.h"
#include "process.h"

namespace glite {
namespace wms {
namespace common {
namespace process {

Subprocess::Subprocess( pid_t pid ) : s_ended( false ), s_signaled( false ),
				      s_exit( 0 ), s_signal( 0 ), s_pid( pid )
{}

Subprocess::Subprocess( Functor &func ) : s_ended( false ), s_signaled( false ),
					  s_exit( 0 ), s_signal( 0 ), s_pid( -1 )
{
  Process    *main = Process::self();
  Subprocess *me = main->fork( func );

  this->s_ended = me->s_ended; this->s_signaled = me->s_signaled;
  this->s_exit = me->s_exit; this->s_signal = me->s_signal;
  this->s_pid = me->s_pid;

  delete me;
}

Subprocess::~Subprocess( void ) {}

int Subprocess::signal( int signal )
{
  return ::kill( this->s_pid, signal );
}

void Subprocess::set_status( int status )
{
  if( WIFEXITED(status) ) {
    this->s_ended = true;
    this->s_exit = WEXITSTATUS(status);
  }
  else if( WIFSIGNALED(status) ) {
    this->s_signaled = true;
    this->s_signal = WTERMSIG(status);
  }

  return;
}

} // process namespace
} // common namespace
} // wms namespace
} // glite namespace
