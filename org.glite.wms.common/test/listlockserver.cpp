#include <unistd.h>

#include <iostream>

#include "glite/wms/common/utilities/FileList.h"
#include "glite/wms/common/utilities/FileListLock.h"
#include "glite/wms/common/utilities/Extractor.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

typedef  utilities::FileList<string>    queue_type;

int main( void )
{
  int                                     res = 0;
  string                                  data;
  utilities::FileList<string>             flist( "locktest.dat" );
  utilities::FileListDescriptorMutex      flmutex( flist );
  utilities::FileListLock                 locker( flmutex, false );
  utilities::ForwardExtractor<queue_type> extractor;
  utilities::FileList<string>::iterator   current;

  extractor.reset( flist );

  try {
    do {
      while( true ) {
	cout << "Going to lock the mutex..." << flush;
	locker.lock();
	cout << " Locked !" << endl;

	cout << "Extracting something..." << endl;
	current = extractor.get_next();
	cout << "...extraction ended..." << endl;

	if( current != extractor->end() ) break;
	else {
	  cout << "Going to unlock the mutex..." << endl;
	  locker.unlock();

	  cout << "Sleeping 2 seconds..." << endl;
	  sleep( 2 );
	}
      }

      cout << "Got new data \"" << *current << "\"" << endl;
      cout << "Going to unlock the mutex..." << endl;
      locker.unlock();

      cout << "Sleeping 1 second..." << endl;
      sleep( 1 );

      cout << "Going to lock the mutex..." << flush;
      locker.lock();
      cout << " Locked !" << endl;

      cout << "Removing the iterator..." << endl;
      extractor.erase( current );

//      cout << "Sleeping 10 seconds..." << endl;
//      sleep( 10 );

      cout << "Going to unlock the mutex..." << endl;
      locker.unlock();
    } while( true );
  }
  catch( utilities::FileContainerError &error ) {
    cout << "FileContainerError = " << error.string_error() << endl;

    res = 1;
  }
    
  return res;
}
