#include <cstring>

#include <iostream>

#include "process/user.h"
#include "process/process.h"

using namespace std;
using glite::wms::common;
namespace process = glite::wms::common::process;

int main( void )
{
  int             res;
  process::User   cur1;

  cout << "Running as user: " << cur1.name() << ", trying to drop..." << endl;
  res = process::Process::self()->drop_privileges( "dguser" );

  if( res )
    cout << "Privilege dropping failed (" << strerror(res) << ")" << endl;
  else {
    process::User   cur2;

    cout << "Running as user: " << cur2.name() << ", trying to regain..." << endl;
    res = process::Process::self()->regain_privileges();

    if( res )
      cout << "Privilege regaining failed (" << strerror(res) << ")" << endl;
    else {
      process::User   cur3;

      cout << "Now running as user: " << cur3.name() << ", exiting..." << endl;
    }
  }

  return res;
}
