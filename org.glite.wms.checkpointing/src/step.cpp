// File: step.cpp
// Author: Alessio Gianelle <gianelle@pd.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include "glite/wms/checkpointing/step.h"
#include "glite/wms/checkpointing/ChkptException.h"

namespace glite {
namespace wms {
namespace checkpointing {

// Construct 
Step::Step( int istep ) : s_type( integer ), s_u_istep( istep ) {}

Step::Step( const std::string &sstep ) : s_type( label ), s_u_sstep( NULL ) 
{
  this->s_u_sstep = new std::string( sstep );
}

Step::Step( const char *sstep ) : s_type ( label ), s_u_sstep( NULL )
{
  this->s_u_sstep = new std::string( sstep );
}

Step::Step( const Step &step ) : s_type( step.s_type ), s_u_sstep( NULL )
{
  if( step.s_type == integer ) this->s_u_istep = step.s_u_istep;
  else this->s_u_sstep = new std::string( *step.s_u_sstep );
}

Step::~Step( void ) 
{
  if( this->s_type == label ) delete this->s_u_sstep;
}

Step &Step::operator=( const Step &that )
{
  if( this != &that ) {
    if( (this->s_type == label) && (that.s_type == label) )
      this->s_u_sstep->assign( *that.s_u_sstep );
    else if( this->s_type == label ) {
      delete this->s_u_sstep;
      this->s_u_istep = that.s_u_istep;
      this->s_type = integer;
    } else if( that.s_type == label ) {
      this->s_u_sstep = new std::string( *that.s_u_sstep );
      this->s_type = label;
    } else {
      this->s_u_istep = that.s_u_istep;
      this->s_type = integer;
    }
  }
  return( *this );
}

int Step::getInteger( void ) { 
  if (this->s_type == integer ) 
    return( this->s_u_istep ); 
  else 
    throw WTException( __FILE__, __LINE__, "Step::getInteger", "label", "integer");
}

const std::string &Step::getLabel( void ) { 
  if (this->s_type == label ) 
    return( *this->s_u_sstep ); 
  else 
    throw WTException( __FILE__, __LINE__, "Step::getLabel", "integer", "label");
}


} // checkpointing
} // wms
} // glite









