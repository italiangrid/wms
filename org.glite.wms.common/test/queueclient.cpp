#include <unistd.h>
#include <sys/types.h>

#include <string>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include "utilities/FileList.h"
#include "utilities/FileListLock.h"

#include "queuecommon.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

string &roba( const char *cosa )
{
  static string    wow, pids;

  if( pids.size() == 0 )
    pids.assign( boost::lexical_cast<string>(getpid()) );

  wow.assign( pids ); wow.append( ": " ); wow.append( cosa );

  return wow;
}

int main( void )
{
  size_t        i, n, max;
  char         *robe1[] = { "Roba uno", "Roba due", "Roba tre", "Roba quattro", "Roba cinque",
			    "Roba sei", "Roba sette", "Roba otto", "Roba nove", "Roba dieci" };
  char          binary[] = { 123, 124, 125, 126, 127, 95, 64, 0 };
  char         *robe2[] = { "Cosa numero uno", "Cosa numero due", "Altra cosa con dentro\nun newline",
			    "Diciamo\ntutto cio`\nche diciamo", binary, "Ultima roba..." };

  utilities::FileList<string>             mylist( queuefile );
  utilities::FileList<string>::iterator   position;
  utilities::FileListDescriptorMutex      listmutex( mylist );
  utilities::FileListLock                 locker( listmutex, false );

  try {
    max = sizeof( robe2 ) / sizeof( char * );

    locker.lock();
    for( i = 0; i < max; i++ ) {
      cout << "Adding (" << robe2[i] << ") at end..." << endl;
      mylist.push_back( roba(robe2[i]) );

      if( i == 4 ) {
	position = mylist.end();
	--position;
      }
    }

    cout << "Adding thing in the middle..." << endl;
    mylist.insert( position, roba("Per lo mezzo") );

    cout << "Adding thing at end..." << endl;
    mylist.push_back( roba("Alla fine") );
    locker.unlock();

    cout << "Waiting 5 seconds..." << endl;
    sleep( 5 );

    max = sizeof( robe1 ) / sizeof( char * );
    for( i = 0; i < max; i++ ) {
      cout << "Adding (" << robe1[i] << ") at beginning..." << endl;

      locker.lock(); mylist.push_front( roba(robe1[i]) ); sleep( 1 ); locker.unlock();
    }
  }
  catch( utilities::FileContainerError &error ) {
    cerr << "Error catch: " << error.string_error() << endl;
  }

  return( 0 );
}
