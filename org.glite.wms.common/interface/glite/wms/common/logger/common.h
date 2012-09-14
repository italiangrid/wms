/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
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

#ifndef GLITE_WMS_COMMON_LOGGER_COMMON_H
#define GLITE_WMS_COMMON_LOGGER_COMMON_H

#include <cstdio>

#include <string>

#include <boost/shared_ptr.hpp>

namespace glite {
namespace wms {
namespace common {
namespace logger {

class StatePusher;

enum level_t {
  _first_level, fatal = _first_level,
  critical,
  severe,
  error,
  warning,
  info,
  debug,
  _last_positive, null = _last_positive,
  verylow,
  low,
  medium,
  high,
  ugly,
  veryugly,
  _last_level
};

class DataContainerImpl {
public:
  DataContainerImpl( void );
  virtual ~DataContainerImpl( void );

  // Setters
  virtual void date( bool b ) = 0;
  virtual void multiline( bool b, const char *prefix ) = 0;
  virtual void next_level( level_t lev ) = 0;
  virtual void time_format( const char *format ) = 0;
  virtual void function( const char *func ) = 0;
  virtual void clear_function( void ) = 0;

  // Constant extractors
  virtual bool date( void ) const = 0;
  virtual bool multiline( void ) const = 0;
  virtual level_t next_level( void ) const = 0;
  virtual const std::string &time_format( void ) const = 0;
  virtual const std::string &function( void ) const = 0;
  virtual const std::string &multiline_prefix( void ) const = 0;

  // Extractors
  virtual bool date( void ) = 0;
  virtual bool multiline( void ) = 0;
  virtual level_t next_level( void ) = 0;
  virtual const std::string &time_format( void ) = 0;
  virtual const std::string &function( void ) = 0;
  virtual const std::string &multiline_prefix( void ) = 0;

  void copy( const DataContainerImpl &dci );
};

class DataContainerSingle : public DataContainerImpl {
  friend class StatePusher;

public:
  DataContainerSingle( const char *format );
  ~DataContainerSingle( void );

  // Setters
  virtual void date( bool d );
  virtual void multiline( bool b, const char *prefix );
  virtual void next_level( level_t lev );
  virtual void time_format( const char *format );
  virtual void function( const char *func );
  virtual void clear_function( void );

  // Constant extractors
  virtual bool date( void ) const;
  virtual bool multiline( void ) const;
  virtual level_t next_level( void ) const;
  virtual const std::string &time_format( void ) const;
  virtual const std::string &function( void ) const;
  virtual const std::string &multiline_prefix( void ) const;

  // Extractors
  virtual bool date( void );
  virtual bool multiline( void );
  virtual level_t next_level( void );
  virtual const std::string &time_format( void );
  virtual const std::string &function( void );
  virtual const std::string &multiline_prefix( void );

private:
  DataContainerSingle( void ); // For the StatePusher

  bool               dcs_date, dcs_multiline;
  level_t            dcs_next;
  std::string        dcs_format, dcs_function, dcs_multiprefix;
};

class data_c {
public:
  data_c( void );
  data_c( const char *name, level_t lev, const char *format );

  ~data_c( void );

  // Setters
  inline void bad( bool b ) { this->bd_bad = b; }
  inline void date( bool d ) { this->bd_data->date( d ); }
  inline void show_severity( bool show ) { this->bd_showSeverity = show; }
  inline void multiline( bool d, const char *prefix ) { this->bd_data->multiline( d, prefix ); }
  inline void next_level( level_t lev ) { this->bd_data->next_level( lev ); }
  inline void buffer_level( level_t lev ) { this->bd_current = lev; }
  inline void max_size( size_t s ) { this->bd_maxSize = s; }
  inline void add_to_total( size_t add ) { this->bd_total += add; }
  inline void function( const char *func ) { this->bd_data->function( func ); }
  inline void time_format( const char *format ) { this->bd_data->time_format( format ); }
  inline void clear_function( void ) { this->bd_data->clear_function(); }

  // const Extractors
  inline bool bad( void ) const { return this->bd_bad; }
  inline bool date( void ) const { return this->bd_data->date(); }
  inline bool show_severity( void ) const { return this->bd_showSeverity; }
  inline bool multiline( void ) const { return this->bd_data->multiline(); }
  inline size_t max_size( void ) const { return this->bd_maxSize; }
  inline size_t total_size( void ) const { return this->bd_total; }
  inline size_t buffer_size( void ) const { return bd_s_bufsize; }
  inline level_t next_level( void ) const { return this->bd_data->next_level(); }
  inline level_t buffer_level( void ) const { return this->bd_current; }
  inline const char *buffer_base( void ) const { return this->bd_buffer; }
  inline const std::string &time_format( void ) const { return this->bd_data->time_format(); }
  inline const std::string &function( void ) const { return this->bd_data->function(); }
  inline const std::string &multiline_prefix( void ) const { return this->bd_data->multiline_prefix(); }

  // Extractors
  inline bool bad( void ) { return this->bd_bad; }
  inline bool date( void ) { return this->bd_data->date(); }
  inline bool show_severity( void ) { return this->bd_showSeverity; }
  inline bool multiline( void ) { return this->bd_data->multiline(); }
  inline size_t max_size( void ) { return this->bd_maxSize; }
  inline size_t total_size( void ) { return this->bd_total; }
  inline size_t buffer_size( void ) { return bd_s_bufsize; }
  inline level_t next_level( void ) { return this->bd_data->next_level(); }
  inline level_t buffer_level( void ) { return static_cast<level_t>( static_cast<int>(this->bd_current) % static_cast<int>(_last_positive) ); }
  inline char *buffer_base( void ) { return this->bd_buffer; }
  inline DataContainerImpl *container( void ) { return this->bd_data; }
  inline const std::string &time_format( void ) { return this->bd_data->time_format(); }
  inline const std::string &function( void ) { return this->bd_data->function(); }
  inline const std::string &multiline_prefix( void ) { return this->bd_data->multiline_prefix(); }

  void reset_container( DataContainerImpl *dc );
  void reset( const char *name, level_t lev, const char *format );
  void remove( void );

  static const char           *bd_s_timeFormat;

private:
  data_c( const data_c &bd ); // Not implemented
  data_c &operator=( const data_c &bd ); // Not implemented

  static const size_t          bd_s_bufsize = BUFSIZ, bd_s_maxSize = (1024 * 1024);

  bool                         bd_bad, bd_remove, bd_showSeverity;
  level_t                      bd_current;
  size_t                       bd_maxSize, bd_total;
  DataContainerImpl           *bd_data;
  std::string                  bd_filename;
  char                         bd_buffer[bd_s_bufsize];
};

} // Namespace logger
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_LOGGER_COMMON_H */

// Local Variables:
// mode: c++
// End:
