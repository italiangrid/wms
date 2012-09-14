#ifndef GLITE_WMS_COMMON_PROCESS_USER_H
#define GLITE_WMS_COMMON_PROCESS_USER_H

#include <pwd.h>

namespace glite {
namespace wms {
namespace common {
namespace process {

class User : private passwd {
public:
  // Constructors/Destructors
  User( void );
  User( const char *name );
  User( uid_t uid );
  User( const User &u );

  ~User( void );

  // Extractors
  inline const char *name( void )   { return this->pw_name; }
  inline const char *passwd( void ) { return this->pw_passwd; }
  inline const char *gecos( void )  { return this->pw_gecos; }
  inline const char *dir( void )    { return this->pw_dir; }
  inline const char *shell( void )  { return this->pw_shell; }

  // Constant extractors
  inline bool good( void ) const { return this->u_good; }
  inline uid_t uid( void ) const { return this->pw_uid; }
  inline gid_t gid( void ) const { return this->pw_gid; }
  inline const char *name( void ) const   { return this->pw_name; }
  inline const char *passwd( void ) const { return this->pw_passwd; }
  inline const char *gecos( void ) const  { return this->pw_gecos; }
  inline const char *dir( void ) const    { return this->pw_dir; }
  inline const char *shell( void ) const  { return this->pw_shell; }

  // Setters
  User &uid( uid_t uid );
  User &name( const char *name );

  // Operators
  inline User &operator=( const User &U ) { if( this != &U ) this->zero()->copy( &U ); return( *this ); }
  inline bool operator!( void ) { return( !(this->u_good) ); }
  inline operator bool( void ) { return( this->u_good ); }

private:
  // Private methods
  void copy( const struct passwd *pwd );
  void copy( char *&dest, const char *source );
  User *remove( void );
  User *zero( void );

  // Object data
  bool      u_good;
};

} // Namespace process
} // common namespace
} // wms namespace
} // glite namespace

#endif /*  GLITE_WMS_COMMON_PROCESS_USER_H */

// Local Variables:
// mode: c++
// End:
