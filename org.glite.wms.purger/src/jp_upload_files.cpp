#include <string>
#include <vector>

#include "glite/jp/jp_client.h"
#include "glite/jp/jpimporter.h"
#include "jp_upload_files.h"
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

namespace glite {
namespace wms {
namespace purger {

namespace {

struct delete_array
{
  delete_array::delete_array(size_t n) : length(n) {}
  void delete_array::operator()(const char** a) {
    while(length) { delete a[--length]; }
    delete[] a;
  }
  size_t length;
};

void delete_jp_context(glite_jpcl_context_t* ctx) 
{ 
 glite_jpcl_FreeContext(*ctx); 
 delete(ctx);
}

} // anonymous namespace closure

jp_upload_files::jp_upload_files(std::string const& jobid,
    std::string const& proxy,
    std::string const& lbmd,
    std::string const& jpps)
{
}

void jp_upload_files::operator()(std::vector<std::string> const&v) const 
  throw(init_context_exception, importer_upload_exception)
{
 boost::shared_ptr<glite_jpcl_context_t> jpcl_context(new glite_jpcl_context_t(), delete_jp_context);
 boost::shared_array<const char*> files(new const char*[v.size()], delete_array(v.size()));
 
 for(int i=0; i< v.size(); ++i) 
   files[i] = strdup(v[i].c_str());
 
 if ( glite_jpcl_InitContext(jpcl_context.get()) ) throw init_context_exception();

 if ( !m_lb_mail_dir.empty() ) glite_jpcl_SetParam(*jpcl_context, GLITE_JPCL_PARAM_LBMAILDIR, m_lb_mail_dir.c_str());
 if ( !m_jp_contact.empty() )  glite_jpcl_SetParam(*jpcl_context, GLITE_JPCL_PARAM_JPPS, m_jp_contact.c_str());

 if ( glite_jpimporter_upload_files(*jpcl_context, m_jobid.c_str(), files.get(), m_jp_proxy_file.c_str()) ) {
   char *errt, *errd;
   glite_jpcl_Error(*jpcl_context, &errt, &errd);
   throw importer_upload_exception(errt, errd);
 }
}

}
}
}
