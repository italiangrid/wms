#include <cstdlib>
#include <ctime>

#include <iostream>
#include <vector>

#include "glite/wms/common/utilities/FileList.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

typedef  utilities::FileList<int>::iterator   Iterator;

int main( void )
{
  int                                  n, val;
  time_t                               now = time( NULL );
  utilities::FileList<int>             flist( "compact.dat" );
  utilities::FileList<int>::iterator   flIt;
  vector<Iterator>                     vec;
  vector<Iterator>::iterator           vecIt;

  srand( (unsigned int) now );

  try {
    for( n = 0; n < 10; ++n ) {
      val = rand();
      cout << "Inserting " << val << "..." << endl;
      flist.push_back( val );
    }

    cout << "\nReading..." << endl;
    for( flIt = flist.begin(); flIt != flist.end(); ++flIt )
      cout << "Numero = " << *flIt << endl;

    cout << "\nRemoving some of them..." << endl;
    for( flIt = flist.begin(); flIt != flist.end(); ++flIt ) {
      val = rand() % 5;
      if( (val == 0) || (val == 1) ) {
	cout << "Adding " << *flIt << " for removal..." << endl;
	vec.push_back( flIt );
      }
    }

    for( vecIt = vec.begin(); vecIt != vec.end(); ++vecIt )
      flist.erase( *vecIt );

    cout << "\nReading..." << endl;
    for( flIt = flist.begin(); flIt != flist.end(); ++flIt )
      cout << "Numero = " << *flIt << endl;

    cout << "\nCompacting..." << endl;
    flist.compact();

    cout << "\nReading..." << endl;
    for( flIt = flist.begin(); flIt != flist.end(); ++flIt )
      cout << "Numero = " << *flIt << endl;
  }
  catch( utilities::FileContainerError &err ) {
    cerr << "Catched an error: " << err.string_error() << endl;
  }

  return 0;
}
