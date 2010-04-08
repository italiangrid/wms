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

#include "glite/wms/common/configuration/exceptions.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace configuration {

CannotConfigure::CannotConfigure( void ) : exception(), cc_what() {}

CannotConfigure::~CannotConfigure( void ) throw() {}

const char *CannotConfigure::what( void ) const throw()
{
  this->cc_what.assign( this->reason() );

  return this->cc_what.c_str();
}

string CannotConfigure::reason( void ) const
{
  return string( "Configuration problem: " );
}

OtherErrors::OtherErrors( const char *error ) : CannotConfigure(), oe_error( error )
{}

OtherErrors::~OtherErrors( void ) throw() {}

string OtherErrors::reason( void ) const
{
  string     answ( this->CannotConfigure::reason() );

  answ.append( this->oe_error );

  return answ;
}

InvalidExpression::InvalidExpression( const string &expr ) : CannotConfigure(), ie_expr( expr )
{}

InvalidExpression::~InvalidExpression( void ) throw() {}

string InvalidExpression::reason( void ) const
{
  string    answ( this->CannotConfigure::reason() );

  answ.append( "invalid expression \"" ); answ.append( this->ie_expr ); answ.append( "\" found." );

  return answ;
}

CannotReadFile::CannotReadFile( const string &name ) : CannotConfigure(), crf_file( name )
{}

CannotReadFile::~CannotReadFile( void ) throw() {}

string CannotReadFile::reason( void ) const
{
  string      answ( this->CannotConfigure::reason() );

  answ.append( "cannot parse configuration file \"" );
  answ.append( this->crf_file ); answ.append( "\", probably syntax error in the classad." );

  return answ;
}

UndefinedParameter::UndefinedParameter( const char *name ) : CannotConfigure(), up_param( name )
{}

UndefinedParameter::~UndefinedParameter( void ) throw() {}

string UndefinedParameter::reason( void ) const
{
  string    answ( this->CannotConfigure::reason() );

  answ.append( "undefined parameter \"" ); answ.append( this->up_param ); answ.append( "\" found." );

  return answ;
}

UndefinedVariable::UndefinedVariable( const string &name ) : CannotConfigure(), uv_variable( name )
{}

UndefinedVariable::~UndefinedVariable( void ) throw() {}

string UndefinedVariable::reason( void ) const
{
  string    answ( this->CannotConfigure::reason() );

  answ.append( "undefined environment variable: \"" );
  answ.append( this->uv_variable ); answ.append( "\"." );

  return answ;
}

CannotOpenFile::CannotOpenFile( const char *name ) : CannotConfigure(), cof_file( name )
{}

CannotOpenFile::~CannotOpenFile( void ) throw() {}

string CannotOpenFile::reason( void ) const
{
  string   answ( this->CannotConfigure::reason() );

  answ.append( "cannot open file \"" ); answ.append( this->cof_file ); answ.append( "\"." );

  return answ;
}

CannotFindFile::CannotFindFile( const std::string &name, const vector<string> &paths ) : CannotConfigure(),
											 cff_file( name ), cff_paths( paths )
{}

CannotFindFile::~CannotFindFile( void ) throw() {}

string CannotFindFile::reason( void ) const
{
  size_t      n, max = this->cff_paths.size();
  string      answ( this->CannotConfigure::reason() );

  answ.append( "file \"" ); answ.append( this->cff_file ); answ.append( "\" not found in the path(s) " );

  for( n = 0; n < max; n++ ) {
    answ.append( this->cff_paths[n] );

    if( n < max ) answ.append( ", " );
  }

  return answ;
}

ModuleMismatch::ModuleMismatch( const ModuleType &type ) : CannotConfigure(), mm_etype( type ) {}

ModuleMismatch::~ModuleMismatch( void ) throw() {}

string ModuleMismatch::reason( void ) const
{
  string    answ( this->CannotConfigure::reason() );

  answ.append( "unknown module \"" ); answ.append( this->mm_etype.get_stringtype() ); answ.append( "\"." );

  return answ;
}

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace
