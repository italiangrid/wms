#include <unistd.h>

#include <string>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include "utilities/FileList.h"
#include "utilities/FileListLock.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

int main( void )
{
  int                                    res = 0, n;
  string                                 data;
  utilities::FileList<string>            flist( "locktest.dat" );
  utilities::FileListMutex               flmut( flist );

  try {
    for( n = 0; n < 10; ++n ) {
      cout << "Sleeping 1 second... " << endl;
      sleep( 1 );

      cout << "Start file locking..." << flush;
      utilities::FileListLock    locker( flmut );
      cout << " Locked!" << endl;

      data.assign( "Roba numero: " );
      data.append( boost::lexical_cast<string>(n) );

      flist.push_back( data );
      cout << "Roba \"" << data << "\" messa nella coda..." << endl;

      cout << "Sleeping 10 seconds..." << endl;
      sleep( 10 );
      cout << "Start file unlocking (by destructor)..." << endl;
    }
  }
  catch( utilities::FileContainerError &error ) {
    cout << "FileContainerError = " << error.string_error() << endl;

    res = 1;
  }

  return res;
}
