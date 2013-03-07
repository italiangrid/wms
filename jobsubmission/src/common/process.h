#ifndef GLITE_WMS_COMMON_PROCESS_PROCESS_H
#define GLITE_WMS_COMMON_PROCESS_PROCESS_H

#include <sys/types.h>

#include <list>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace glite {
namespace wms {
namespace common {
namespace process {

class Subprocess; // Forward declaration

class Functor {
public:
  Functor( void ) {}

  virtual ~Functor( void ) {}

  virtual int run( void ) = 0;
};

class Process {
public:
  Process( void );
  ~Process( void );

  inline bool is_son( void ) { return this->p_son; }
  inline bool is_daemon( void ) { return this->p_daemon; }
  inline bool have_stdstreams( void ) { return this->p_havestd; }
  inline pid_t pid( void ) { return this->p_pid; }

  void remove( Subprocess *proc );
  void wait_one( Subprocess *proc );
  int make_daemon( bool chdir = false, bool close = true );
  int drop_privileges_forever( const char *username );
  int drop_privileges( const char *username );
  int regain_privileges( void );
  pid_t parent( void );
  Subprocess *wait_first( void );
  Subprocess *fork( Functor &runner );

  inline static Process *self( void )
  { if( p_s_instance == NULL ) p_s_instance = new Process; return p_s_instance; }

private:
  typedef boost::shared_ptr<Subprocess>   ProcPtr;

  bool                p_son, p_daemon, p_havestd;
  pid_t               p_pid;
  std::list<ProcPtr>  p_list;

  static Process  *p_s_instance;
};

} // Namespace process
} // common namespace
} // wms namespace
} // glite namespace


#endif /* GLITE_WMS_COMMON_PROCESS_PROCESS_H */

// Local Variables:
// mode: c++
// End:
