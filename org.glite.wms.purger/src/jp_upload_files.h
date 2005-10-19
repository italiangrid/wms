#ifndef GLITE_WMS_PURGER_JP_UPLOAD_FILES
#define GLITE_WMS_PURGER_JP_UPLOAD_FILES

#include <string>
#include <vector>

namespace glite {
namespace wms {
namespace purger {

class jp_upload_files
{
public:
  struct init_context_exception {
  };
  class  importer_upload_exception {
    std::string m_errt, m_errd;
  public:
    importer_upload_exception(const std::string& errt, const std::string& errd) : 
      m_errt(errt), m_errd(errd) {}
    
    std::string what() const throw() {
     return std::string("Error calling glite_jpimporter_upload_files() " + 
       m_errt + std::string(": ") + m_errd);
    }
  };
  
  jp_upload_files(std::string const& jobid, std::string const& proxy, 
    std::string const& lbmd, std::string const& jpps);
  void operator()(std::vector<std::string> const&) const
    throw(init_context_exception,
          importer_upload_exception
    );

private:
  std::string m_jp_proxy_file;
  std::string m_lb_mail_dir;
  std::string m_jp_contact;
  std::string m_jobid;
};

}
}
}

#endif
