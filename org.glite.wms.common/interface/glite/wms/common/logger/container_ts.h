#ifndef GLITE_WMS_COMMON_LOGGER_CONTAINER_TS_H
#define GLITE_WMS_COMMON_LOGGER_CONTAINER_TS_H

#include <boost/thread/tss.hpp>

#include "glite/wms/common/logger/common.h"

namespace glite {
namespace wms {
namespace common {
namespace logger {

class DataContainerMulti : public DataContainerImpl {
public:
  DataContainerMulti( const char *format );
  ~DataContainerMulti( void );

  // Setters
  virtual void date( bool d );
  virtual void multiline( bool d, const char *prefix );
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
  struct data_s {
    data_s( const DataContainerSingle &dcs );

    bool         d_date, d_multiline;
    level_t      d_next;
    std::string  d_format, d_function, d_multiprefix;
  };

  inline void createData( void ) const
  { if( this->dcm_data.get() == NULL ) this->dcm_data.reset( new data_s(this->dcm_single) ); return; }

  mutable boost::thread_specific_ptr<data_s>       dcm_data;
  DataContainerSingle                              dcm_single;
};

} // logger namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_LOGGER_CONTAINER_TS_H */

// Local Variables:
// mode: c++
// End:
