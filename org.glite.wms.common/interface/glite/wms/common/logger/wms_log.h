#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>

namespace glite{
namespace wms{
namespace common{
namespace logger{

class wms_log
{
public:
   enum mode{ STDERR = 0, SYSLOG };
private:
   static boost::scoped_ptr<wms_log> wms_log_instance;
   static boost::mutex mx;

   mode m_mode;
   wms_log();

public:
   static wms_log* get_instance();

   void init(mode m);
 
   void debug(std::string str);
   void info(std::string str);
   void warning(std::string str);
   void error(std::string str);
   void sever(std::string str);
   void critical(std::string str);
   void fatal(std::string str);
};

}
}
}
}

