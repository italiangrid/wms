#include <iostream>
#include <algorithm>
#include <list>

#include "glite/wms/common/utilities/FileList.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

class NotLessThan {
public:
  NotLessThan( int i ) : nlt_i( i ) {}

  bool operator()( int k ) { return( !(k < this->nlt_i) ); }

private:
  int   nlt_i;
};

void print( int el ) { cout << el << " "; }

int main( int argc, char* argv[] )
try {
  int                         ivec[] = { 8, 10, 7, 4, 6, 3, 2, 0, 1, 9 };
  list<int>                   lvec;
  list<int>::iterator         lit;

  if (argc < 2)
  {
	  cerr << "You need to specify the output file!" << endl;
	  exit(2);
  }
  
  utilities::FileList<int>    flvec( argv[1] );
  utilities::FileList<int>::iterator   flit;

  flvec.insert( flvec.begin(), ivec, ivec + (sizeof(ivec) / sizeof(int)) );
  lvec.insert( lvec.begin(), ivec, ivec + (sizeof(ivec) / sizeof(int)) );

//    for_each( lvec.begin(), lvec.end(), print ); cout << endl;
//    lvec.sort();
//    for_each( lvec.begin(), lvec.end(), print ); cout << endl;

//    for( lit = lvec.begin(); lit != lvec.end(); ++lit )
//      flvec.push_back( *lit );
//    for_each( flvec.begin(), flvec.end(), print ); cout << endl;

//    lit = find_if( lvec.begin(), lvec.end(), NotLessThan(5) );
//    flit = find_if( flvec.begin(), flvec.end(), NotLessThan(5) );

//    cout << "Dentro alla lista: " << *lit << ", dentro al file: " << *flit << endl;

  lvec.insert( lvec.begin(), ivec, ivec + (sizeof(ivec) / sizeof(int)) );
  flvec.insert( flvec.begin(), ivec, ivec + (sizeof(ivec) / sizeof(int)) );

  lvec.insert( lvec.end(), ivec, ivec + (sizeof(ivec) / sizeof(int)) );
  flvec.insert( flvec.end(), ivec, ivec + (sizeof(ivec) / sizeof(int)) );

  for_each( lvec.begin(), lvec.end(), print ); cout << endl;
  for_each( flvec.begin(), flvec.end(), print ); cout << endl;

  return 0;
}
catch( utilities::FileContainerError &error ) {
  cerr << error.string_error() << endl;

  return 1;
}
