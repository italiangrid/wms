#include <unistd.h>

#include <string>
#include <iostream>

#include "utilities/FileList.h"
#include "utilities/FileListLock.h"
#include "utilities/Extractor.h"

#include "queuecommon.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

int main( void )
{
  size_t      size;
  string      robo;
  utilities::FileList<string>         flist( queuefile );
  utilities::FileListDescriptorMutex  listmutex( flist );
  utilities::FileListLock             locker( listmutex, false );
  utilities::FileList<string>::iterator                 flIt;
  utilities::ForwardExtractor<utilities::FileList<string> >    extractor( flist );

  try {
    while( true ) {
      locker.lock();

      if( extractor->empty() ) cout << "Nothing written..." << endl;
      else {
	do {
	  flIt = extractor.get_next();
	  cout << "Read (" << *flIt << ")" << endl;
	  extractor.erase( flIt );
	} while( !extractor->empty() );
      }

      locker.unlock();
      sleep( 2 );
    }
  }
  catch( utilities::FileContainerError &error ) {
    cerr << "Got error: " << error.string_error() << endl;
  }

  return( 0 );
}
