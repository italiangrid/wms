#include <iostream>

#include "utilities/FileList.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

int main( int argc, char *argv[] )
{
  int       arg;
  utilities::FileList<string>            flist;
  utilities::FileList<string>::iterator  flIt;

  try {
    for( arg = 1; arg < argc; arg++ ) {
      cout << "Opening file: " << argv[arg] << endl;

      flist.open( argv[arg] );
      cout << "\tRead size: " << flist.size() << " (" << flist.size() << ")" << endl;

      for( flIt = flist.begin(); flIt != flist.end(); ++flIt )
	cout << "\tRead: (" << *flIt << ")" << endl;

      //      cout << "End = " << *flIt << endl;

      flist.close();
      cout << "File " << argv[arg] << " closed..." << endl;
    }
  }
  catch( utilities::FileContainerError &error ) {
    cerr << "Error saw: " << error.string_error() << endl;
    return (1);
  }

  return( 0 );
}
