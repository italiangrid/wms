#include <iostream>
#include <algorithm>

#include "glite/wms/common/utilities/FileList.h"

using namespace std;
using glite::wms::common;
namespace utilities = glite::wms::common::utilities;

template <class Type>
void stampa_coda( utilities::FileList<Type> &filequeue )
{
  typename utilities::FileList<Type>::iterator   fileIt;

  cout << "<---------------------------------------------------------------------------->" << endl;
  cout << "La dimensione sembrerebbe essere: " << filequeue.size() << endl;
  cout << "<---------------------------------------------------------------------------->" << endl;
  for( fileIt = filequeue.begin(); fileIt != filequeue.end(); ++fileIt ) {
    cout << "Oggetto = (" << *fileIt << ')' << endl;
  }
  cout << "<---------------------------------------------------------------------------->" << endl;

  return;
}

int main( void )
{
  int                         n, ret = 0;
  utilities::FileList<int>    codaintera( "interi.queue" );

  try {
    for( n = 0; n < 10; n++ ) codaintera.push_back( n );

    stampa_coda( codaintera );
  }
  catch( utilities::FileContainerError &stat ) {
    cout << "Cacciato uno stato: " << stat.string_error() << endl;

    ret = 1;
  }

  return ret;
}
