/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://project.eu-egee.org/index.php?id=115 for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef GLITE_WMS_COMMON_CONFIGURATION_EXCEPTIONS_H
#define GLITE_WMS_COMMON_CONFIGURATION_EXCEPTIONS_H

#include <string>
#include <vector>
#include <iostream>
#include <exception>

#include "ModuleType.h"

namespace glite {
namespace wms {
namespace common {
namespace configuration {

class CannotConfigure : public std::exception {
public:
  CannotConfigure( void );
  virtual ~CannotConfigure( void ) throw();

  virtual std::string reason( void ) const;
  virtual const char *what( void ) const throw();

private:
  mutable std::string   cc_what;
};

class OtherErrors : public CannotConfigure {
public:
  OtherErrors( const char *reason );
  virtual ~OtherErrors( void ) throw();

  virtual std::string reason( void ) const;

private:
  std::string    oe_error;
};

class InvalidExpression : public CannotConfigure {
public:
  InvalidExpression( const std::string &expr );
  virtual ~InvalidExpression( void ) throw();

  virtual std::string reason( void ) const;

  inline const std::string &expression( void ) const { return this->ie_expr; }

private:
  std::string    ie_expr;
};

class CannotReadFile : public CannotConfigure {
public:
  CannotReadFile( const std::string &file );
  virtual ~CannotReadFile( void ) throw();

  virtual std::string reason( void ) const;

  inline const std::string &filename( void ) const { return this->crf_file; }

private:
  std::string    crf_file;
};

class UndefinedParameter : public CannotConfigure {
public:
  UndefinedParameter( const char *name );
  virtual ~UndefinedParameter( void ) throw();

  virtual std::string reason( void ) const;

  inline const std::string &parameter( void ) const { return this->up_param; }

private:
  std::string    up_param;
};

class UndefinedVariable : public CannotConfigure {
public:
  UndefinedVariable( const std::string &name );
  virtual ~UndefinedVariable( void ) throw();

  virtual std::string reason( void ) const;

  inline const std::string &variable( void ) const { return this->uv_variable; }

private:
  std::string    uv_variable;
};

class CannotOpenFile : public CannotConfigure {
public:
  CannotOpenFile( const char *name );
  virtual ~CannotOpenFile( void ) throw();

  virtual std::string reason( void ) const;

  inline const std::string &file( void ) const { return this->cof_file; }

private:
  std::string   cof_file;
};

class CannotFindFile : public CannotConfigure {
public:
  CannotFindFile( const std::string &name, const std::vector<std::string> &paths );
  virtual ~CannotFindFile( void ) throw();

  virtual std::string reason( void ) const;

  inline const std::vector<std::string> &paths( void ) const { return( this->cff_paths ); }

private:
  std::string                cff_file;
  std::vector<std::string>   cff_paths;
};

class ModuleMismatch : public CannotConfigure {
public:
  ModuleMismatch( const ModuleType &type );
  virtual ~ModuleMismatch( void ) throw();

  virtual std::string reason( void ) const;

  inline ModuleType type( void ) { return( this->mm_etype ); }

private:
  ModuleType       mm_etype;
};

inline std::ostream &operator<<( std::ostream &os, const CannotConfigure &cc ) { os << cc.reason(); return os; }

} // configuration namespace end
} // common namespace end
} // wms namespace end
} // glite namespace end

#endif /* GLITE_WMS_COMMON_CONFIGURATION_EXCEPTIONS_H */

// Local Variables:
// mode: c++
// End:
