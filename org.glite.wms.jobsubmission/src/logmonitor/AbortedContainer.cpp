#include <iostream>

#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/utilities/FileList.h"

#include "../jobcontrol_namespace.h"
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
    }
    catch( utilities::FileContainerError &err ) {
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
    }
    catch( utilities::FileContainerError &err ) {
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

}; // Namespace logmonitor

} JOBCONTROL_NAMESPACE_END;
