#include <iostream>

#include "glite/wms/common/utilities/filecontainer.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

int main( void )
{
  int           i, max;
  size_t        number;
  streampos     position;
  streamoff     begin, end;
  char          binary[] = { 123, 124, 125, 126, 127, 95, 64, 0 };
  char         *robe[] = { "Cosa numero uno", "Cosa numero due", "Altra cosa con dentro\nun newline",
			   "Diciamo\ntutto cio`\nche diciamo", binary, "Ultima roba..." };
  string        data;
  utilities::FileContainerError::iostatus_t   status;
  utilities::FileContainer                    container( "test.cont" );
  utilities::FileContainerError               error;
  utilities::FileIterator                     iter;

  max = sizeof(robe) / sizeof(char *);

  status = container.read_begin( begin );
  status = container.read_end( end );

  cout << "Begin = " << begin << ", End = " << end << endl;

  for( i = 0; i < max; i++ ) {
    cout << "Adding (" << robe[i] << ") at end..." << endl;
    container.read_end( end );
    container.add_data( string(robe[i]), end, iter );

    if( i == 4 ) position = iter.position();

    cout << "\titer.position = " << iter.position() << ", (" << iter.get_prev() << ", " << iter.get_next() << ")" << endl;
  }

  container.read_size( number );
  cout << "L'oggetto contiene: " << number << " cose..." << endl;

  cout << "Adding thing for the middle (" << position << ")..." << endl;
  container.add_data( string("Per lo mezzo"), position, iter );

  cout << "Adding thing at end (" << hex << end << dec << ")..." << endl;
  container.read_end( end );
  container.add_data( string("Alla fine"), end, iter );

  container.read_size( number );
  cout << "L'oggetto contiene: " << number << " cose..." << endl;

  for( i = 0; i < max; i++ ) {
    cout << "Adding (" << robe[i] << ") at beginning..." << endl;
    container.read_begin( begin );
    container.add_data( string(robe[i]), begin, iter );
    cout << "\titer.position = " << iter.position() << ", (" << iter.get_prev() << ", " << iter.get_next() << ")" << endl;
  }

  container.read_size( number );
  cout << "L'oggetto contiene: " << number << " cose..." << endl;

  // Start the reading of the data...

  status = container.read_begin( begin );
  status = container.get_iterator( begin, iter );
  if( status == utilities::FileContainerError::all_good )
    do {
      status = container.read_data( data, iter );
      if( status != utilities::FileContainerError::all_good ) {
	cerr << "Error status: " << status << " (" << error.set_code( status ).string_error() << ")" << endl;
	break;
      }

      cout << "Read: (" << data << ")" << endl;

      status = container.increment_iterator( iter );
      if( status != utilities::FileContainerError::all_good ) {
	cerr << "Error status: " << status << " (" << error.set_code( status ).string_error() << ")" << endl;
	break;
      }

      container.read_end( end );
    } while( iter.position() != end );
  else cerr << "Error status: " << status << " (" << error.set_code( status ).string_error() << ")" << endl;

  return( 0 );
}
