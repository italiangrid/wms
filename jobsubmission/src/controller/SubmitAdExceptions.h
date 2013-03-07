/* Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITADEXCEPTIONS_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITADEXCEPTIONS_H

#include <exception>
#include <string>

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

class SubmitAdException : public std::exception {
public:
  SubmitAdException( void );
  virtual ~SubmitAdException( void ) throw();

  virtual const char *what() const throw() = 0;

  inline std::string error( void ) const { return this->sae_what; }

protected:
  mutable std::string   sae_what;
};

class CannotOpenStatusFile : public SubmitAdException {
public:
  CannotOpenStatusFile( const std::string &path, int mode = 0 );
  virtual ~CannotOpenStatusFile( void ) throw();

  inline int mode( void ) const { return this->cosf_mode; }
  inline const std::string &path( void ) const { return this->cosf_path; }

  virtual const char *what( void ) const throw();

private:
  int           cosf_mode;
  std::string   cosf_path;
};

class FileSystemError : public SubmitAdException {
public:
  FileSystemError( const char *error );
  virtual ~FileSystemError( void ) throw();

  inline const std::string &filesystem_error( void ) { return this->fse_error; }

  virtual const char *what( void ) const throw();

private:
  std::string   fse_error;
};

class CannotCreateDirectory: public SubmitAdException {
public:
  CannotCreateDirectory( const char *dirType, const std::string &path, const char *reason );
  ~CannotCreateDirectory( void ) throw();

  inline std::string &path( void ) { return this->ccd_path; }
  inline std::string &directory_owner( void ) { return this->ccd_dirType; }
  inline std::string &reason( void ) { return this->ccd_reason; }

  virtual const char *what( void ) const throw();

private:
  std::string   ccd_path, ccd_dirType, ccd_reason;
};

} // Namespace controller

} JOBCONTROL_NAMESPACE_END

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_SUBMITADEXCEPTIONS_H */

// Local Variables:
// mode: c++
// End:
