#include <string>
#include <iostream>

#include <classad_distribution.h>

#include "utilities/FileList.h"
#include "utilities/FileListLock.h"

#include "queuecommon.h"

using namespace std;

using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

//template <classad::ClassAd, utilities::StdConverter<classad::ClassAd> >
template utilities::StdConverter<classad::ClassAd> 
utilities::FLIterator<classad::ClassAd, utilities::StdConverter<classad::ClassAd> >::fli_s_converter;

int main( void )
{
  const int                                          max = 10;
  int                                                n, res = 0;
  string                                             rawad( "[ Bambolina = \"guee\"; ];" );
  classad::ClassAd                                  *ad;
  classad::ClassAdParser                             parser;
  utilities::FileList<classad::ClassAd>              cllist( queuefile );
  utilities::FileList<classad::ClassAd>::iterator    position;
  utilities::FileListDescriptorMutex                 listmutex( cllist );
  utilities::FileListLock                            locker( listmutex, false );

  for( n = 0; n < max; n++ ) {
    ad = parser.ParseClassAd( rawad );
    ad->InsertAttr( "Numero", n );

    locker.lock(); cllist.push_back( *ad ); locker.unlock();

    delete ad;
    sleep( 1 );
  }

  return res;
}
