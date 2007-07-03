#include <algorithm>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstring>

#include <signal.h>

#include <boost/lexical_cast.hpp>

#include "jobcontrol_namespace.h"
#include "common/SignalChecker.h"

#include "handler.h"

using namespace std;

JOBCONTROL_NAMESPACE_BEGIN {

namespace jccommon {

SignalChecker   *SignalChecker::sh_s_instance = NULL;

SignalChecker *SignalChecker::instance( void )
{
  if( sh_s_instance == NULL ) sh_s_instance = new SignalChecker;

  return sh_s_instance;
}

SignalChecker::SignalChecker( void )
{}

SignalChecker::~SignalChecker( void )
{}

void SignalChecker::deactivate_signal_trapping( void )
{
  list<int>::iterator   sigIt;

  for( sigIt = this->sh_signals.begin(); sigIt != this->sh_signals.end(); ++sigIt )
    signal( *sigIt, SIG_IGN );

  return;
}

void SignalChecker::reactivate_signal_trapping( void )
{
  list<int>::iterator  sigIt;

  for( sigIt = this->sh_signals.begin(); sigIt != this->sh_signals.end(); ++sigIt )
    signal( *sigIt, edg_wl_jobcontrol_common_SignalHandler );

  return;
}

void SignalChecker::reset_all_signals( void )
{
  list<int>::iterator  sigIt;

  for( sigIt = this->sh_signals.begin(); sigIt != this->sh_signals.end(); ++sigIt )
    this->reset_signal( *sigIt );

  this->sh_signals.clear();

  return;
}

void SignalChecker::throw_on_signal( void )
{
  int signum = static_cast<int>( edg_wl_jobcontrol_common_received_signal );

  if( signum != 0 ) throw Exception( signum );

  return;
}

bool SignalChecker::add_signal( int signum )
{
  bool                  good;
  list<int>::iterator   position;

  if( (signum == SIGKILL) || (signum == SIGSTOP) )
    good = false;
  else {
    good = (signal(signum, edg_wl_jobcontrol_common_SignalHandler) != SIG_ERR);

    position = find( this->sh_signals.begin(), this->sh_signals.end(), signum );
    if( position == this->sh_signals.end() )
      this->sh_signals.push_back( signum );
  }

  return good;
}

vector<bool> SignalChecker::add_signals( const vector<int> &signums )
{
  vector<int>::const_iterator    sigIt;
  vector<bool>                   results;

  for( sigIt = signums.begin(); sigIt != signums.end(); ++sigIt )
    results.push_back( this->add_signal(*sigIt) );

  return results;
}

bool SignalChecker::reset_signal( int signum )
{
  bool     good;

  if( (signum == SIGKILL) || (signum == SIGSTOP) )
    good = false;
  else {
    good = (signal(signum, SIG_DFL) != SIG_ERR);

    this->sh_signals.remove( signum );
  }

  return good;
}

bool SignalChecker::ignore_signal( int signum )
{
  bool    good;

  if( (signum == SIGKILL) || (signum == SIGSTOP) )
    good = false;
  else
    good = (signal(signum, SIG_IGN) != SIG_ERR);

  return good;
}

int SignalChecker::check_signal( void )
{
  int signum = static_cast<int>( edg_wl_jobcontrol_common_received_signal );

  edg_wl_jobcontrol_common_received_signal = 0;

  return signum;
}

SignalChecker::Exception::Exception( int signal ) : exception(), e_signal( signal ), e_reason( new string("Received signal n. ") )
{
  this->e_reason->append( boost::lexical_cast<string>(signal) );

#ifdef HAVE_STRSIGNAL
  this->e_reason->append( 1, '(' );
  this->e_reason->append( strsignal(signal) );
  this->e_reason->append( 1, ')' );
#endif
}

SignalChecker::Exception::~Exception( void ) throw()
{}

const char *SignalChecker::Exception::what( void ) const throw()
{
  return this->e_reason->c_str();
}

}; // Namespace jccommon

} JOBCONTROL_NAMESPACE_END;
