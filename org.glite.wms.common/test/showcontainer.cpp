#include <cstdlib>
#include <cstring>

#include <iostream>

#include "utilities/filecontainer.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

void check_status( utilities::FileContainerError::iostatus_t stat )
{
  utilities::FileContainerError    error( stat );

  if( stat != utilities::FileContainerError::all_good ) {
    cerr << "Error !!! " << error.string_error() << " (" << (int) stat << ")" << endl;

    exit( 1 );
  }

  return;
}

int main( int argc, char *argv[] )
{
  bool         fileorder = false;
  int          arg, nn = 0;
  streamoff    begin, end;
  string       data;
  utilities::FileContainer                    container;
  utilities::FileContainerError::iostatus_t   status;
  utilities::FileIterator                     fileIt;

  for( arg = 1; arg < argc; arg++ ) {
    if( !strcmp(argv[arg], "-f") ) fileorder = true;
    else {
      cout << "Opening file " << ++nn << ": " << argv[arg] << endl;
      if( fileorder ) cout << "\tReading in file order..." << endl;

      status = container.open( argv[arg] ); check_status( status );

      if( fileorder ) 
  	status = container.get_fileorder_iterator( container.file_begin(), fileIt );
      else {
	status = container.read_begin( begin );
	status = container.get_iterator( begin, fileIt );
      }

      check_status( status );

      do {
  	if( fileorder )
  	  status = container.read_fileorder_data( data, fileIt );
  	else
	  status = container.read_data( data, fileIt );

	check_status( status );

	cout << "Read (" << data << ")" << endl;

  	if( fileorder )
  	  status = container.increment_fileorder_iterator( fileIt );
  	else
	  status = container.increment_iterator( fileIt );

	check_status( status );

	if( fileorder ) 
	  status = container.read_file_end( end );
	else
	  status = container.read_end( end );

	check_status( status );
      } while( (fileorder && (fileIt.position() != end)) || (fileIt.position() != end) );
//        } while( fileIt.position() != container.end() );

      cout << "Closing file " << argv[arg] << endl;

      container.close();

      fileorder = false;
    }
  }

  return( 1 );
}
