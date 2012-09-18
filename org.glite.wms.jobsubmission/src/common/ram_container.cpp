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
#include <algorithm>

#include <classad_distribution.h>

#include "common/filelist.h"
#include "common/filelist_lock.h"
#include "common/filelist_iterator.h"

#include "id_container.h"
#include "ram_container.h"
#include "IdCompare.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {
namespace jccommon {

void RamContainer::internalCopy( IdContainer &ic )
{
  PointerId current;
  glite::wms::jobsubmission::jccommon::FileListMutex flmutex( ic.ic_container );
  glite::wms::jobsubmission::jccommon::FileListLock locker( flmutex );
  glite::wms::jobsubmission::jccommon::FileList<classad::ClassAd>::iterator
    position, end = ic.ic_container.end();
  list<PointerId>::iterator pushed;

  for( position = ic.ic_container.begin(); position != end; ++position ) {
    current.reset( position ).unset_position();
    this->rc_pointers.push_back( current );

    pushed = this->rc_pointers.end(); --pushed;

    this->rc_condors.push_back( CondorId(pushed) ); this->rc_edgs.push_back( EdgId(pushed) );
  }

  sort( this->rc_condors.begin(), this->rc_condors.end(), Compare() );
  sort( this->rc_edgs.begin(), this->rc_edgs.end(), Compare() );

  return;
}

RamContainer::RamContainer( void ) : rc_inserted( 0 ), rc_pointers(), rc_edgs(), rc_condors()
{}

RamContainer::RamContainer( IdContainer &ic ) : rc_inserted( 0 ), rc_pointers(), rc_edgs(), rc_condors()
{ this->internalCopy( ic ); }

RamContainer::~RamContainer( void )
{}

void RamContainer::copy( IdContainer &ic )
{
  this->rc_inserted = 0;
  this->rc_pointers.clear();
  this->rc_edgs.clear(); this->rc_condors.clear();

  this->internalCopy( ic );
}

string RamContainer::condor_id( const std::string &edgId )
{
  string                   condorId;
  vector<EdgId>::iterator  position;

  position = lower_bound( this->rc_edgs.begin(), this->rc_edgs.end(), edgId, Compare() );
  if( (position != this->rc_edgs.end()) && (position->edg_id() == edgId) ) condorId.assign( position->condor_id() );

  return( condorId );
}

string RamContainer::edg_id( const std::string &condorId )
{
  string                      edgId;
  vector<CondorId>::iterator  position;

  position = lower_bound( this->rc_condors.begin(), this->rc_condors.end(), condorId, Compare() );
  if( (position != this->rc_condors.end()) && (position->condor_id() == condorId) )
    edgId.assign( position->edg_id() );

  return( edgId );
}

void RamContainer::insert( const string &edgId, const string &condorId )
{
  bool                               haveEdg, haveCondor;
  vector<CondorId>::iterator         condorPos;
  vector<EdgId>::iterator            edgPos;
  list<PointerId>::iterator          pit = this->rc_pointers.end();

  do {
    condorPos = lower_bound( this->rc_condors.begin(), this->rc_condors.end(), condorId, Compare() );
    edgPos = lower_bound( this->rc_edgs.begin(), this->rc_edgs.end(), edgId, Compare() );

    haveEdg =    ((edgPos != this->rc_edgs.end()) && (edgId == edgPos->edg_id()));
    haveCondor = ((condorPos != this->rc_condors.end()) && (condorId == condorPos->condor_id()));

    if( haveEdg && haveCondor ) break; // Exit the loop if we have just inserted the same pair...
    else if( haveEdg ) {
      if( (pit = edgPos->position()) != this->rc_pointers.end() )
	this->rc_pointers.erase( pit );

      this->rc_edgs.erase( edgPos );
    }
    else if( haveCondor ) {
      if( (pit = condorPos->position()) != this->rc_pointers.end() )
	this->rc_pointers.erase( pit );

      this->rc_condors.erase( condorPos );
    }
  } while( haveEdg || haveCondor );

  if( !haveEdg || !haveCondor ) {
    this->rc_pointers.push_back( PointerId(condorId, edgId) );
    pit = this->rc_pointers.end(); --pit;

    this->rc_condors.insert( condorPos, CondorId(pit) );
    this->rc_edgs.insert( edgPos, EdgId(pit) );

    this->rc_inserted += 1;
  }

  return;
}

bool RamContainer::remove_by_condor_id( const string &condorId )
{
  bool                        error = false;
  vector<EdgId>::iterator     edgPos;
  vector<CondorId>::iterator  condorPos;
  list<PointerId>::iterator   position;
  string                      edgId;

  condorPos = lower_bound( this->rc_condors.begin(), this->rc_condors.end(), condorId, Compare() );

  error = ((condorPos == this->rc_condors.end()) || (condorId != condorPos->condor_id()));

  if( !error ) {
    edgId.assign( condorPos->edg_id() );
    edgPos = lower_bound( this->rc_edgs.begin(), this->rc_edgs.end(), edgId, Compare() );

    error = ((edgPos == this->rc_edgs.end()) || (edgId != edgPos->edg_id()));

    if( !error ) {
      position = edgPos->position();

      this->rc_pointers.erase( position );
      this->rc_edgs.erase( edgPos ); this->rc_condors.erase( condorPos );

      this->rc_inserted -= 1;
    }
  }

  return error;
}

} // namespace jccommon
} JOBCONTROL_NAMESPACE_END
