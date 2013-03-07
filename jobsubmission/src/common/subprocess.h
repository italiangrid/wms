#ifndef GLITE_WMS_COMMON_PROCESS_SUBPROCESS_H
#define GLITE_WMS_COMMON_PROCESS_SUBPROCESS_H

#include <sys/types.h>

namespace glite {
namespace wms {
namespace common {
namespace process {

class Functor;
class Process;

class Subprocess {
  friend class Process;

public:
  Subprocess( Functor &func );
  ~Subprocess( void );

  inline bool is_ended( void ) { return this->s_ended; }
  inline bool is_signaled( void ) { return this->s_signaled; }
  inline int exit_code( void ) { return this->s_exit; }
  inline int signal_code( void ) { return this->s_signal; }
  inline pid_t pid( void ) { return this->s_pid; }

  int signal( int sig );

private:
  Subprocess( pid_t pid );

  void set_status( int status );

  Subprocess( const Subprocess &proc ); // Not implemented
  Subprocess &operator=( const Subprocess &proc ); // Not implemented

  bool    s_ended, s_signaled;
  int     s_exit, s_signal;
  pid_t   s_pid;
};

} // Namespace process
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_PROCESS_SUBPROCESS_H */

// Local Variables:
// mode: c++
// End:
