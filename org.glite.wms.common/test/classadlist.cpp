#include <iostream>
#include <vector>
#include <string>

#include <boost/lexical_cast.hpp>
#include <classad_distribution.h>

#include "utilities/FileList.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

int main( int argc, char* argv[] )
{
  int                                       n;
  utilities::FileList<classad::ClassAd>    *flist;
  classad::ClassAd                         *adptr, ad;
  string                                    strAd;
  classad::ClassAdParser                    parser;
  utilities::FileList<classad::ClassAd>::iterator   flIt;

  if ( argc < 2 )
  {
	  cerr << "you must specify the output file!" << endl;
	  exit(2);
  }
  flist = new utilities::FileList<classad::ClassAd>( argv[1] );

  for( n = 0; n < 10; n++ ) {
    cout << "Pushing n. " << n << endl;

    strAd.assign( "[ cosa = \"ClassAd\"; numero = \"" );
    strAd.append( boost::lexical_cast<string>(n) );
    strAd.append( "\"; Tubo = \"Tubolare " );
    strAd.append( boost::lexical_cast<string>(n) );
    strAd.append( "\"; ];" );

    adptr = parser.ParseClassAd( strAd.c_str() );

    flist->push_back( *adptr );
  }

  cout << "Closing FileList..." << endl;
  delete flist;

  cout << "Reopening FileList..." << endl;
  flist = new utilities::FileList<classad::ClassAd>( argv[1] );

  for( flIt = flist->begin(); flIt != flist->end(); ++flIt ) {
//      ad.Clear(); ad.Update( *flIt );
    cout << "Extracted classad:\n{" << *flIt << "}" << endl;
  }

  delete flist;

  return 0;
}
