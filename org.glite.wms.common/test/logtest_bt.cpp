#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <fstream>

#include <boost/thread/thread.hpp>

#include "logger/logstream_ts.h"
#include "logger/manipulators.h"
#include "logger/edglog.h"

using glite::wms::common;
namespace logger = glite::wms::common::logger;
namespace ts = glite::wms::common::logger::threadsafe;

using namespace std;

class counter_c {
public:
  counter_c( int start, int end );
  ~counter_c( void );

  void operator()( void );

private:
  int    t_start, t_end;
};

class multiplier_c {
public:
  multiplier_c( double start, double mult, int steps );
  ~multiplier_c( void );

  void operator()( void );

private:
  int    t_steps;
  double t_start, t_inc;
};

counter_c::counter_c( int start, int end ) : t_start( start ), t_end( end )
{}

counter_c::~counter_c( void )
{}

void counter_c::operator()( void )
{
  int          n;
  pid_t        pt = getpid();

  ts::edglog << logger::setfunction( "counter_c::operator()" ) << logger::setlevel( logger::null )
	     << "Starting count thread in pid " << pt << endl;

  for( n = this->t_start; n < this->t_end; ++n )
    ts::edglog << logger::setlevel( /*(n % 2) ? */ logger::null /* : logger::ugly*/ )
	       << "Counting n. " << n << " (" << pt << ")" << endl;

  return;
}

multiplier_c::multiplier_c( double start, double mulp, int steps ) : t_steps( steps ), t_start( start ), t_inc( mulp )
{}

multiplier_c::~multiplier_c( void )
{}

void multiplier_c::operator()( void )
{
  int         n;
  pid_t       pt = getpid();
  double      val;

  ts::edglog << logger::setfunction( "multiplier_c::operator()" ) << logger::setlevel( logger::null )
	     << "Starting multiplexing thread in pid " << pt << endl;

  for( n = 0, val = this->t_start; n < t_steps; ++n, val *= this->t_inc )
    ts::edglog << logger::setlevel( /* (n % 2) ? */ logger::null /* : logger::ugly */ )
	       << "Multiplex n. " << n << " = " << val << " (" << pt << ")" << endl;

  return;
}

void start_all( void )
{
  boost::thread     t1( counter_c(0, 1000) ), t3( multiplier_c(1, 2, 10000) );
  boost::thread     t2( counter_c(5000, 6000) ), t4( multiplier_c(2, 3, 500) );

  ts::edglog << logger::setfunction( "::start_all()" );

  t1.join(); ts::edglog << "Joined thread 1..." << endl;
  t2.join(); ts::edglog << "Joined thread 2..." << endl;
  t3.join(); ts::edglog << "Joined thread 3..." << endl;
  t4.join(); ts::edglog << "Joined thread 4..." << endl;

  ts::edglog << "Exiting..." << endl;

  return;
}

int main( void )
{
  ofstream    ofs( "edglog.log", ios::out );

  ts::edglog.open( ofs, logger::medium );

  ts::edglog << logger::setfunction( "::main()" ) << logger::setlevel( logger::null )
	     << "Starting..." << endl;

  start_all();

  return 0;
}
