#ifndef GLITE_WMS_COMMON_UTILITIES_STREAMDESCRIPTOR_H
#define GLITE_WMS_COMMON_UTILITIES_STREAMDESCRIPTOR_H

#include <fstream>
#include <iostream>


#if (__GNUC__>=3)
#include <ext/stdio_filebuf.h>
#endif

namespace glite {
namespace wms {
namespace common {
namespace utilities {

#if (__GNUC__>=3)

inline int streamdescriptor( const std::fstream &fs )
{
  return static_cast<__gnu_cxx::stdio_filebuf<char> *>( fs.rdbuf() )->fd();
}

inline int streamdescriptor( const std::ifstream &ifs )
{
  return static_cast<__gnu_cxx::stdio_filebuf<char> *>( ifs.rdbuf() )->fd();
}

inline int streamdescriptor( const std::ofstream &ofs )
{
  return static_cast<__gnu_cxx::stdio_filebuf<char> *>( ofs.rdbuf() )->fd();
}

inline int bufferdescriptor( const std::filebuf &fb )
{
  return static_cast<__gnu_cxx::stdio_filebuf<char> *>( const_cast<std::filebuf *>(&fb) )->fd();
}

inline bool create_file( const char *name )
{
  if( !std::fstream(name , std::ios::in).good() ) return (!std::ofstream( name, std::ios::out ).good())  ;
  else return false;
}

#else

inline int streamdescriptor( const std::fstream &fs )
{
  return fs.rdbuf()->fd();
}

inline int streamdescriptor( const std::ifstream &ifs )
{
  return ifs.rdbuf()->fd();
}

inline int streamdescriptor( const std::ofstream &ofs )
{
  return ofs.rdbuf()->fd();
}

inline int bufferdescriptor( const std::filebuf &fb )
{
  return fb.fd();
}

inline bool create_file( const char *name ) { return false; }

#endif

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_UTILITIES_STREAMDESCRIPTOR_H */

// Local Variables:
// mode: c++
// End:
