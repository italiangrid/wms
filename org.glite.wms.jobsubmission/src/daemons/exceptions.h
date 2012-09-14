#ifndef EDG_WORKLOAD_JOBCONTROL_DAEMONS_EXCEPTIONS_H
#define EDG_WORKLOAD_JOBCONTROL_DAEMONS_EXCEPTIONS_H

#include <exception>

JOBCONTROL_NAMESPACE_BEGIN {

namespace daemons {

class DaemonError : public std::exception {
public:
  DaemonError( const std::string &reason );
  virtual ~DaemonError( void ) throw();

  inline const std::string &reason( void ) const { return this->de_reason; }
  virtual const char *what( void ) const throw();

private:
  std::string   de_reason;
};

class CannotStart : public DaemonError {
public:
  CannotStart( const std::string &reason );
  virtual ~CannotStart( void ) throw();
};

class CannotExecute : public DaemonError {
public:
  CannotExecute( const std::string &reason );
  virtual ~CannotExecute( void ) throw();
};

}; // Namespace daemons

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_DAEMONS_EXCEPTIONS_H */

// Local Variables:
// mode: c++
// End:
