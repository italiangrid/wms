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
#include <iostream>

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "common/filelist.h"

#include "jobcontrol_namespace.h"
#include "AbortedContainer.h"

using namespace std;
USING_COMMON_NAMESPACE;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {
namespace logmonitor {

void AbortedContainer::onConstruct( void )
{
  FileList::iterator    flIt, end;

  this->ac_filelist.compact();
  end = this->ac_filelist.end();

  for( flIt = this->ac_filelist.begin(); flIt != end; ++flIt )
    this->ac_pointers.insert( Map::value_type(*flIt, flIt) );
}

AbortedContainer::AbortedContainer( const char *filename ) : ac_inserted( 0 ), ac_filelist( filename ), ac_pointers()
{
  this->onConstruct();
}

AbortedContainer::AbortedContainer( const string &filename ) : ac_inserted( 0 ), ac_filelist( filename ), ac_pointers()
{
  this->onConstruct();
}

AbortedContainer::~AbortedContainer( void )
{}

bool AbortedContainer::insert( const string &condorId )
{
  bool                answer = false;
  Map::iterator       mIt;
  FileList::iterator  end;

  mIt = this->ac_pointers.find( condorId );

  if( mIt == this->ac_pointers.end() ) { // Allow multiple (fake) insertions...
    try {
      this->ac_filelist.push_back( condorId );

      end = this->ac_filelist.end();
      this->ac_pointers.insert( Map::value_type(condorId, --end) );
      this->ac_inserted += 1;
    } catch(glite::wms::jobsubmission::jccommon::FileContainerError const& err) {
      logger::StatePusher    pusher( elog::cedglog, "AbortedContainer::insert()" );

      elog::cedglog << logger::setlevel( logger::null )
		    << "Cannot push new condor id inside aborted list." << endl
		    << "Reason: " << err.string_error() << endl;

      answer = true;
    }
  }

  return answer;
}

bool AbortedContainer::remove( const string &condorId )
{
  bool           answer;
  Map::iterator  mIt;

  mIt = this->ac_pointers.find( condorId );

  if( !(answer = (mIt == this->ac_pointers.end())) ) {
    try {
      this->ac_filelist.erase( mIt->second );
      this->ac_pointers.erase( mIt );
    } catch(glite::wms::jobsubmission::jccommon::FileContainerError const& err) {
      logger::StatePusher     pusher( elog::cedglog, "AbortedContainer::insert()" );

      elog::cedglog << logger::setlevel( logger::null )
		    << "Cannot remove old condor id from aborted list." << endl
		    << "Reason: " << err.string_error() << endl;

      answer = true;
    }
  }

  return answer;
}

void AbortedContainer::compact( void )
{
  this->ac_pointers.clear();
  this->onConstruct();

  this->ac_inserted = 0;

  return;
}

} // namespace logmonitor
} JOBCONTROL_NAMESPACE_END
