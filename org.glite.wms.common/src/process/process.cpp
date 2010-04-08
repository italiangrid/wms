/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org\partners for details on the
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

#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

#include <algorithm>

#include "subprocess.h"
#include "process.h"
#include "user.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace process {

namespace {

class GetProcess {
public:
  GetProcess( pid_t pid );

  inline bool operator()( boost::shared_ptr<Subprocess> &proc ) { return this->gp_pid == proc->pid(); }

private:
  pid_t    gp_pid;
};

GetProcess::GetProcess( pid_t pid ) : gp_pid( pid ) {}

class IsSubproc {
public:
  IsSubproc( Subprocess *proc );

  inline bool operator()( boost::shared_ptr<Subprocess> &proc ) { return this->is_proc == proc.get(); }

private:
  Subprocess    *is_proc;
};

IsSubproc::IsSubproc( Subprocess *proc ) : is_proc( proc ) {}

} // Anonymous namespace

Process *Process::p_s_instance = NULL;

Process::Process( void ) : p_son( false ), p_daemon( false ), p_havestd( true ), p_pid( ::getpid() ), p_list()
{}

Process::~Process( void )
{
  p_s_instance = NULL; 
}

Subprocess *Process::fork( Functor &functor )
{
  pid_t          pid;
  Subprocess    *proc = NULL;

  pid = ::fork();

  if( pid == 0 ) { // We are in the son
    this->p_list.clear(); this->p_pid = ::getpid();
    this->p_son = true;

    exit( functor.run() );
  }
  else if( pid > 0 )  { // We are in the father
    proc = new Subprocess( pid );

    this->p_list.push_back( ProcPtr(proc) );
  }

  return proc;
}

void Process::remove( Subprocess *proc )
{
  list<ProcPtr>::iterator  procIt;

  procIt = find_if( this->p_list.begin(), this->p_list.end(), IsSubproc(proc) );

  if( procIt != this->p_list.end() ) this->p_list.erase( procIt );

  return;
}

Subprocess *Process::wait_first( void )
{
  int                      status;
  pid_t                    pid;
  Subprocess              *proc = NULL;
  list<ProcPtr>::iterator  procIt;

  pid = ::wait( &status );

  if( pid > 0 ) {
    procIt = find_if( this->p_list.begin(), this->p_list.end(), GetProcess(pid) );

    if( procIt != this->p_list.end() ) {
      (*procIt)->set_status( status );

      proc = procIt->get();
    }
  }

  return proc;
}

void Process::wait_one( Subprocess *proc )
{
  int                      status;
  pid_t                    pid;
  list<ProcPtr>::iterator  procIt;

  procIt = find_if( this->p_list.begin(), this->p_list.end(), IsSubproc(proc) );
  if( procIt != this->p_list.end() ) {
    pid = ::waitpid( proc->pid(), &status, 0 );

    if( pid > 0 ) proc->set_status( status );
  }

  return;
}

int Process::drop_privileges_forever( const char *newname )
{
  int     res = 0;
  User    oldUser, newUser( newname );

  if( newUser && (oldUser.uid() == 0) ) {
    res = ::setgid( newUser.gid() );

    if( res == 0 ) res = ::setuid( newUser.uid() );
  }

  return res;
}

int Process::drop_privileges( const char *newname )
{
  int     res = 0;
  User    oldUser, newUser( newname );

  if( newUser && (oldUser.uid() == 0) ) {
    res = ::setegid( newUser.gid() );

    if( res == 0 ) res = ::seteuid( newUser.uid() );
  }

  return res;
}

int Process::regain_privileges( void )
{
  int     res = 0;

  if( !(res = ::setuid(0)) ) res = ::setgid( 0 );

  return res;
}

int Process::make_daemon( bool chdir, bool close )
{
  int    ret = 0;

  if( !this->p_daemon ) {
    this->p_havestd = !close;

    ret = ::daemon( static_cast<int>(!chdir), static_cast<int>(!close) );

    if( ret != -1 ) this->p_daemon = true;
  }
  else ret = -1;

  return ret;
}

pid_t Process::parent( void ) { return ::getppid(); }

} // process namespace
} // common namespace
} // wms namespace
} // glite namespace
