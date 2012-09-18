/* Copyright (c) Members of the EGEE Collaboration. 2004.
 * See http://www.eu-egee.org/partners/ for details on the copyright
 * holders.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License. */
#include <iostream>
#include <memory>
#include <string>
#include <algorithm>

#include <classad_distribution.h>

#include "id_container.h"
#include "IdCompare.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

void IdContainer::onConstruct( void )
{
  glite::wms::jobsubmission::jccommon::FileList<classad::ClassAd>::iterator position, end = this->ic_container.end();
  list<PointerId>::iterator pushed;

  for( position = this->ic_container.begin(); position != end; ++position ) {
    this->ic_pointers.push_back( position );

    pushed = this->ic_pointers.end(); --pushed;

    this->ic_condors.push_back( CondorId(pushed) ); this->ic_edgs.push_back( EdgId(pushed) );
  }

  sort( this->ic_condors.begin(), this->ic_condors.end(), Compare() );
  sort( this->ic_edgs.begin(), this->ic_edgs.end(), Compare() );

  return;
}

bool IdContainer::removePointers( vector<EdgId>::iterator &edgPos, vector<CondorId>::iterator &condorPos )
{
  bool                        error = false;
  list<PointerId>::iterator   position;
  FileList::iterator          list_position;
  ListLock                    lock( this->ic_mutex );

  position = edgPos->position();

  try {
    position = edgPos->position();
    list_position = position->position();

    this->ic_container.erase( list_position );
    this->ic_pointers.erase( position );
    this->ic_edgs.erase( edgPos ); this->ic_condors.erase( condorPos );
  } catch(glite::wms::jobsubmission::jccommon::FileContainerError const&) {
    error = true;
  }

  return error;
}

IdContainer::IdContainer( const char *filename ) : ic_inserted( 0 ),
						   ic_container( filename ), ic_mutex( ic_container ),
						   ic_pointers(), ic_edgs(), ic_condors(),
						   ic_last()
{ 
  ListLock    lock( this->ic_mutex );

  if( !this->ic_container.empty() ) this->onConstruct();
}

IdContainer::IdContainer( const string &filename ) : ic_inserted( 0 ),
						     ic_container( filename ), ic_mutex( ic_container ),
						     ic_pointers(), ic_edgs(), ic_condors(),
						     ic_last()
{
  ListLock    lock( this->ic_mutex );

  if( !this->ic_container.empty() ) this->onConstruct();
}

IdContainer::~IdContainer( void ) {}

string IdContainer::condor_id( const string &edgId )
{
  string                   condorId;
  vector<EdgId>::iterator  position;

  position = lower_bound( this->ic_edgs.begin(), this->ic_edgs.end(), edgId, Compare() );
  if( (position != this->ic_edgs.end()) && (position->edg_id() == edgId) ) condorId.assign( position->condor_id() );

  return condorId;
}

string IdContainer::edg_id( const string &condorId )
{
  string                      edgId;
  vector<CondorId>::iterator  position;

  position = lower_bound( this->ic_condors.begin(), this->ic_condors.end(), condorId, Compare() );
  if( (position != this->ic_condors.end()) && (position->condor_id() == condorId) )
    edgId.assign( position->edg_id() );

  return edgId;
}

list<PointerId>::iterator IdContainer::position_by_condor_id( const string &condorId )
{
  list<PointerId>::iterator     position = this->ic_pointers.end();
  vector<CondorId>::iterator    cit;

  cit = lower_bound( this->ic_condors.begin(), this->ic_condors.end(), condorId, Compare() );
  if( (cit != this->ic_condors.end()) && (cit->condor_id() == condorId) )
    position = cit->position();

  return position;
}

list<PointerId>::iterator IdContainer::position_by_edg_id( const string &edgId )
{
  list<PointerId>::iterator    position = this->ic_pointers.end();
  vector<EdgId>::iterator      eit;

  eit = lower_bound( this->ic_edgs.begin(), this->ic_edgs.end(), edgId, Compare() );
  if( (eit != this->ic_edgs.end()) && (eit->edg_id() == edgId) )
    position = eit->position();

  return position;
}

bool IdContainer::insert( const string &edgId, const string &condorId, const string &sequenceCode, int status )
{
  bool                               error = false;
  classad::ClassAd                   value;
  vector<CondorId>::iterator         condorPos;
  vector<EdgId>::iterator            edgPos;
  FileList::iterator                 last;
  ListLock                           lock( this->ic_mutex );

  condorPos = lower_bound( this->ic_condors.begin(), this->ic_condors.end(), condorId, Compare() );
  edgPos = lower_bound( this->ic_edgs.begin(), this->ic_edgs.end(), edgId, Compare() );

  error = (((edgPos != this->ic_edgs.end()) && (edgId == edgPos->edg_id())) ||
	   ((condorPos != this->ic_condors.end()) && (condorId == condorPos->condor_id())));

  if( !error ) {
    value.InsertAttr( PointerId::condorId(), condorId );
    value.InsertAttr( PointerId::edgId(), edgId );
    value.InsertAttr( PointerId::sequenceCode(), sequenceCode );
    value.InsertAttr( PointerId::condorStatus(), status );

    this->ic_container.push_back( value );
    last = this->ic_container.end(); --last;

    this->ic_pointers.push_back( PointerId(last, condorId, edgId, sequenceCode) );
    this->ic_last = this->ic_pointers.end(); --this->ic_last;

    this->ic_condors.insert( condorPos, CondorId(this->ic_last) );
    this->ic_edgs.insert( edgPos, EdgId(this->ic_last) );

    this->ic_inserted += 1;
  }

  return error;
}

bool IdContainer::update_pointer( iterator position, const string &seqcode, int status, int laststatus )
{
  bool                         error = false;
  auto_ptr<classad::ClassAd>   modified(static_cast<classad::ClassAd*>(position->position()->Copy()));
  FileList::iterator           last;
  ListLock                     lock( this->ic_mutex );

  modified->InsertAttr( PointerId::sequenceCode(), seqcode );
  modified->InsertAttr( PointerId::condorStatus(), status );
  if( laststatus != static_cast<int>(undefined_status) ) modified->InsertAttr( PointerId::lastStatus(), laststatus );

  this->ic_container.erase( position->position() );
  this->ic_container.push_back( *modified );
  last = this->ic_container.end(); --last;

  position->reset( last ); //, seqcode );

  return error;
}

bool IdContainer::increment_pointer_retry_count( iterator position )
{
  bool                         error = false;
  int                          count = position->retry_count();
  auto_ptr<classad::ClassAd>   modified(static_cast<classad::ClassAd*>(position->position()->Copy()));
  FileList::iterator           last;
  ListLock                     lock( this->ic_mutex );

  modified->InsertAttr( PointerId::retryCount(), (count + 1) );

  this->ic_container.erase( position->position() );
  this->ic_container.push_back( *modified );
  last = this->ic_container.end(); --last;

  position->reset( last );

  return error;
}

bool IdContainer::remove_by_edg_id( const string &edgId )
{
  bool                          error = false;
  vector<EdgId>::iterator       edgPos;
  vector<CondorId>::iterator    condorPos;
  string                        condorId;

  edgPos = lower_bound( this->ic_edgs.begin(), this->ic_edgs.end(), edgId, Compare() );

  error = ((edgPos == this->ic_edgs.end()) || (edgId != edgPos->edg_id()));

  if( !error ) {
    condorId.assign( edgPos->condor_id() );
    condorPos = lower_bound( this->ic_condors.begin(), this->ic_condors.end(), condorId, Compare() );

    error = ((condorPos == this->ic_condors.end()) || (condorId != condorPos->condor_id()));

    if( !error ) error = this->removePointers( edgPos, condorPos );
  }

  return error;
}

bool IdContainer::remove_by_condor_id( const string &condorId )
{
  bool                          error = false;
  vector<EdgId>::iterator       edgPos;
  vector<CondorId>::iterator    condorPos;
  string                        edgId;

  condorPos = lower_bound( this->ic_condors.begin(), this->ic_condors.end(), condorId, Compare() );

  error = ((condorPos == this->ic_condors.end()) || (condorId != condorPos->condor_id()));

  if( !error ) {
    edgId.assign( condorPos->edg_id() );
    edgPos = lower_bound( this->ic_edgs.begin(), this->ic_edgs.end(), edgId, Compare() );

    error = ((edgPos == this->ic_edgs.end()) || (edgId != edgPos->edg_id()));

    if( !error ) error = this->removePointers( edgPos, condorPos );
  }

  return error;
}

void IdContainer::refresh( void )
{
  ListLock    lock( this->ic_mutex );

  if( this->ic_container.modified() ) {
    this->ic_container.sync();

    this->ic_pointers.clear();
    this->ic_edgs.clear(); this->ic_condors.clear();
    this->onConstruct();
  }

  return;
}

void IdContainer::compact( void )
{
  ListLock     lock( this->ic_mutex );

  this->ic_container.compact();

  this->ic_pointers.clear();
  this->ic_edgs.clear(); this->ic_condors.clear();
  this->onConstruct();

  this->ic_inserted = 0;

  return;
}

void IdContainer::clear( void )
{
  ListLock     lock( this->ic_mutex );

  this->ic_container.clear();
  this->ic_pointers.clear();
  this->ic_edgs.clear();
  this->ic_condors.clear();

  return;
}

} // namespace jccommon
} JOBCONTROL_NAMESPACE_END
