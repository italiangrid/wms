#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/pool/detail/singleton.hpp>

namespace glite{
namespace wms{
namespace common{
namespace logger{

class wms_log
{
public:
   enum mode{ STDERR = 0, STDOUT, SYSLOG };
   enum level{ FATAL = 0, CRITICAL, SEVER, ERROR, WARNING, INFO, DEBUG };
private:

   mode m_mode;
   int m_init_flag;
   level m_init_level;

public:
   wms_log();

   void init(mode m, level l);
 
   void debug(const std::string& str);
   void info(const std::string& str);
   void warning(const std::string& str);
   void error(const std::string& str);
   void sever(const std::string& str);
   void critical(const std::string& str);
   void fatal(const std::string& str);
};

}
}
}
}

