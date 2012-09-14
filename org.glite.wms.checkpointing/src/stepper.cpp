// File: stepper.cpp
// Author: Alessio Gianelle <gianelle@pd.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include "glite/wms/checkpointing/stepper.h"
#include "glite/wms/checkpointing/ChkptException.h"

namespace glite {
namespace wms {
namespace checkpointing {

// Constructor
StepsSet::StepsSet( const std::vector<std::string>& llabel, int cstep ) : ss_first(  cstep ? cstep - 1: cstep ), 
  ss_last( llabel.size() - 1 ), 
  ss_current( cstep ? cstep - 1: cstep),
  ss_ittype( label ), ss_steps( llabel )
  {}
 
StepsSet::StepsSet( int last, int cstep ) : ss_first( cstep ), ss_last( last ), ss_current( cstep ), ss_ittype ( integer )
  {}

// Initialization

void StepsSet::initialize( const std::vector<std::string>& llabel, int cstep )
{
  this->ss_first = cstep ? cstep - 1: cstep;
  this->ss_last = llabel.size() - 1;
  this->ss_current = cstep ? cstep - 1: cstep;
  this->ss_ittype = label;
  this->ss_steps = llabel;
}

void StepsSet::initialize( int last, int cstep )
{
  this->ss_first = cstep;
  this->ss_last = last;
  this->ss_current = cstep;
  this->ss_ittype = integer;
}

int StepsSet::getNextInt( void ) 
{ 
  this->ss_current++;
  if ( this->ss_current > this->ss_last )
    throw EoSException(__FILE__, __LINE__, "StepsSet::getNextInt");
  else
    return this->ss_current;
}

const std::string StepsSet::getNextLabel( void ) 
{ 
  this->ss_current++;
  if ( this->ss_current > this->ss_last )
    throw EoSException(__FILE__, __LINE__, "StepsSet::getNextLabel");
  else
    return this->ss_steps[this->ss_current];
}

int  StepsSet::getCurrentInt( void ) { 
  if ( this->ss_current > this->ss_last )
    throw EoSException( __FILE__, __LINE__, "getCurrentInt" );
  else 
    return this->ss_current; 
}

const std::string StepsSet::getCurrentLabel(void ) { 
  if ( this->ss_current > this->ss_last )
    throw EoSException( __FILE__, __LINE__, "getCurrentLabel" );
  else 
    return this->ss_steps[ss_current]; 
}

} // checkpointing
} // wms
} // glite




