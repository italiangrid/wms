#include <iostream>

#include "utilities/FileList.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

//  int main( int argc, char *argv[] )
//  {
//    int           arg, i, max;
//    char          binary[] = { 123, 124, 125, 126, 127, 95, 64, 0 };
//    char         *robe[] = { "Cosa numero uno", "Cosa numero due", "Altra cosa con dentro\nun newline",
//  			   "Diciamo\ntutto cio`\nche diciamo", binary, "Ultima roba..." };
//    utilities::FileList<string>            flist;

//    for( arg = 1; arg < argc; arg++ ) {
//      try {
//        flist.open( argv[arg] );

//        max = boost::lexical_cast<int>( argv[++arg] );
//        for( i = 0; i < max; i++ ) flist.push_back( string(robe[i]) );

//        flist.close();
//      }
//      catch( utilities::FileContainerError &error ) {
//        cerr << "Error: " << error.string_error() << endl;
//      }
//    }

//    return( 0 );
//  }

int main( void )
{
  int           i, max;
  char          binary[] = { 123, 124, 125, 126, 127, 95, 64, 0 };
  char         *robe[] = { "Cosa numero uno", "Cosa numero due", "Altra cosa con dentro\nun newline",
			   "Diciamo\ntutto cio`\nche diciamo", binary, "Ultima roba..." };
  string        data;
  utilities::FileList<string>             flist( "list.dat" );
  utilities::FileList<string>::iterator   flIt, position;

  max = sizeof( robe ) / sizeof( char * );

  try {
    for( i = 0; i < max; i++ ) {
      cout << "Adding (" << robe[i] << ") at end..." << endl;
      flist.push_back( string(robe[i]) );

      if( i == 4 ) {
	position = flist.end();
	--position;
      }
    }

    cout << "Adding thing in the middle, at (" << *position << ")..." << endl;
    flist.insert( position, string("Per lo mezzo") );

    cout << "Adding thing at end..." << endl;
    flist.push_back( string("Alla fine") );

    cout << "Removing data in the middle (" << *position << ")..." << endl;
    flist.erase( position );

    for( i = 0; i < max; i++ ) {
      cout << "Adding (" << robe[i] << ") at beginning..." << endl;
      flist.push_front( string(robe[i]) );
    }
  }
  catch( utilities::FileContainerError &error ) {
    cerr << "Error: " << error.string_error() << endl;
  }

  return( 0 );
}
