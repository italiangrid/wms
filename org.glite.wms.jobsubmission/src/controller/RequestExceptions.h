#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_REQUESTEXCEPTIONS_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_REQUESTEXCEPTIONS_H

#include <exception>

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class RequestException : public std::exception {
public:
  RequestException( void );
  virtual ~RequestException( void ) throw();

  virtual const char *what( void ) const throw() = 0;

protected:
  mutable std::string   re_what;
};

class MalformedRequest : public RequestException {
public:
  MalformedRequest( const classad::ClassAd &ad );
  virtual ~MalformedRequest( void ) throw();

  virtual const char *what( void ) const throw();

  inline const classad::ClassAd &classad( void ) const { return *this->mr_ad; }

private:
  classad::ClassAd     *mr_ad;
};

class UninitializedRequest : public RequestException {
public:
  UninitializedRequest( void );
  virtual ~UninitializedRequest( void ) throw();

  virtual const char *what( void ) const throw();
};

class MismatchedProtocol : public RequestException {
public:
  MismatchedProtocol( const std::string &def, const std::string &current );
  virtual ~MismatchedProtocol( void ) throw();

  virtual const char *what( void ) const throw();

  inline const std::string &default_protocol( void ) const { return this->mp_default; }
  inline const std::string &current_protocol( void ) const { return this->mp_current; }

private:
  std::string   mp_default, mp_current;
};

}; // Namespace controller

} JOBCONTROL_NAMESPACE_END;

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_REQUESTEXCEPTIONS_H */

// Local Variables:
// mode: c++
// End:
