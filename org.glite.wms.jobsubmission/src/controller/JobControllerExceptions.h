#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLEREXCEPTIONS_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLEREXCEPTIONS_H

#include <exception>
#include <string>

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class ControllerError : public std::exception {
public:
  ControllerError( void );
  virtual ~ControllerError( void ) throw();

  virtual const std::string &reason( void ) const = 0;
  virtual const char *what( void ) const throw();

private:
  mutable std::string   ce_what;
};

class CannotCreate : public ControllerError {
public:
  CannotCreate( const std::string &reason );
  virtual ~CannotCreate( void ) throw();

  virtual const std::string &reason( void ) const;

private:
  std::string    cc_reason;
};

class CannotExecute : public ControllerError {
public:
  CannotExecute( const std::string &reason );
  virtual ~CannotExecute( void ) throw();

  virtual const std::string &reason( void ) const;

private:
  std::string   ce_reason;
};

}; // namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_JOBCONTROLLEREXCEPTIONS_H */

// Local Variables:
// mode: c++
// End:
