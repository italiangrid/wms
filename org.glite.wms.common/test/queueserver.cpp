#include <ctime>
#include <unistd.h>

#include <string>
#include <iostream>

#include "utilities/FileList.h"
#include "utilities/FileListLock.h"

#include "queuecommon.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

int main( void )
{
  size_t                              size;
  string                              robo;
  utilities::FileList<string>         flist( queuefile );
  utilities::FileListDescriptorMutex  listmutex( flist );
  utilities::FileListLock             locker( listmutex, false );

  try {
    while( true ) {
      cout << "Locking locker (" << time(NULL) << ")... " << flush;
      locker.lock(); // flist.sync();
      cout << "Locked (" << time(NULL) << ") !!" << endl;

      if( flist.empty() ) cout << "Nothing written..." << endl;
      else {
	cout << "FileList contains: " << flist.size() << " objects..." << endl;

	do {
	  robo = flist.back();
	  cout << "Read (" << robo << ")" << endl;
	  flist.pop_back();
	} while( !flist.empty() );
      }

//        cout << "Sleep (" << time(NULL) << ")... " << flush;
//        sleep( 1 );
      cout << "Unlocking locker (" << time(NULL) << ")..." << flush;
      locker.unlock();
      cout << "Unlocked (" << time(NULL) << ")!!" << endl;
      sleep( 2 );
    }
  }
  catch( utilities::FileContainerError &error ) {
    cerr << "Got error: " << error.string_error() << endl;
  }

  return( 0 );
}
