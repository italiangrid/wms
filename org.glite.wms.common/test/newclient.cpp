#include <unistd.h>

#include <ctime>

#include <iostream>

#include "utilities/FileList.h"
#include "utilities/FileListLock.h"

#include "queuecommon.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

int main( void )
{
  int                                 ret = 0;
  time_t                              epoch;
  string                              buffer1, buffer2;
  utilities::FileList<string>         mylist( queuefile );
  utilities::FileListDescriptorMutex  listmutex( mylist );
  utilities::FileListLock             locker( listmutex, false );

  while( true ) {
    epoch = time( NULL );
    buffer1.assign( asctime(localtime(&epoch)) );
    buffer2.assign( buffer1.begin(), buffer1.end() - 1 );

    cout << "Pushing \"" << buffer2 << "\"... " << flush;
    locker.lock(); mylist.push_back( buffer2 ); locker.unlock();
    cout << "Done !!!" << endl;

    sleep( 10 );
  }

  return ret;
}
