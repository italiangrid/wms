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

#include <unistd.h>
#include <sys/types.h>

#include <cstring>

#include "user.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace process {

void User::copy( const struct passwd *pwd )
{
  this->remove()->zero();

  if( pwd ) {
    if( pwd->pw_name )   this->copy( this->pw_name, pwd->pw_name );
    if( pwd->pw_passwd ) this->copy( this->pw_passwd, pwd->pw_passwd );
    if( pwd->pw_gecos )  this->copy( this->pw_gecos, pwd->pw_gecos );
    if( pwd->pw_dir )    this->copy( this->pw_dir, pwd->pw_dir );
    if( pwd->pw_shell )  this->copy( this->pw_shell, pwd->pw_shell );
    this->pw_uid = pwd->pw_uid; this->pw_gid = pwd->pw_gid;
    this->u_good = true;
  }
  else this->u_good = false;

  return;
}

void User::copy( char *&dest, const char *source )
{
  dest = new char[strlen(source) + 1];
  strcpy( dest, source );

  return;
}

User *User::zero( void )
{
  memset( this, 0, sizeof(User) );

  return this;
}

User *User::remove( void )
{
  if( this->pw_name )   delete [] this->pw_name;
  if( this->pw_passwd ) delete [] this->pw_passwd;
  if( this->pw_gecos )  delete [] this->pw_gecos;
  if( this->pw_dir )    delete [] this->pw_dir;
  if( this->pw_shell )  delete [] this->pw_shell;

  return this;
}

User::User( void ) : u_good( false )
{
  const struct passwd   *pwd = ::getpwuid( ::geteuid() );
  this->zero()->copy( pwd );
}

User::User( const char *name ) : u_good( false )
{
  const struct passwd   *pwd = ::getpwnam( name );
  this->zero()->copy( pwd );
}

User::User( uid_t uid ) : u_good( false )
{
  const struct passwd   *pwd = ::getpwuid( uid );
  this->zero()->copy( pwd );
}

User::User( const User &u ) : u_good( false )
{
  this->zero()->copy( &u );
}

User::~User( void ) { this->remove(); }

User &User::uid( uid_t uid )
{
  const struct passwd    *pwd = ::getpwuid( uid );
  this->copy( pwd );

  return *this;
}

User &User::name( const char *name )
{
  const struct passwd    *pwd = ::getpwnam( name );
  this->copy( pwd );

  return *this;
}

} // process namespace
} // common namespace
} // wms namespace
} // glite namespace
