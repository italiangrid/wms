#include <iostream>

#include "utilities/FileList.h"
#include "utilities/Extractor.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

typedef  utilities::FileList<string>             FileList;
typedef  utilities::ForwardExtractor<FileList>   Extractor;

int main( int argc, char *argv[] )
{
  int                     arg, ret = 0;
  FileList                flist;
  Extractor::iterator     exIt;

  if( argc < 2 ) {
    cerr << "Usage: " << argv[0] << " <file1> [file2...]" << endl;
    ret = 1;
  }
  else {
    try {
      for( arg = 1; arg < argc; ++arg ) {
	cout << "Opening file: " << argv[arg] << endl;

	flist.open( argv[arg] );
	Extractor     extractor( flist );

	cout << "\tRead size: " << extractor->size() << " (" << extractor->size() << ")" << endl;

	while( (exIt = extractor.get_next()) != extractor->end() ) {
	  cout << "Read something: [" << *exIt << "]" << endl
	       << "\tGoing to remove, size = " << extractor->size() << endl;

	  extractor.erase( exIt );

	  cout << "\tRemoved, size = " << extractor->size() << endl;
	}
      }
    }
    catch( utilities::FileContainerError &error ) {
      cerr << "Error saw: " << error.string_error() << endl;

      ret = 1;
    }
  }

  return ret;
}
