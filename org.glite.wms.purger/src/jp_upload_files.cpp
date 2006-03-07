#ifndef GLITE_WMS_DONT_HAVE_JP

#include <vector>
#include <string>

#include "glite/jp/jp_client.h"
#include "glite/jp/jpimporter.h"
#include "jp_upload_files.h"
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

namespace glite {
namespace wms {
namespace purger {

namespace {

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
 std::vector<char const*> files;

 for(size_t i=0; i<v.size(); ++i) {
  files[i] = v[i].c_str();
 }

 if ( glite_jpcl_InitContext(&(*jpcl_context)) ) throw init_context_exception();

 if ( !m_lb_mail_dir.empty() ) glite_jpcl_SetParam(*jpcl_context, GLITE_JPCL_PARAM_LBMAILDIR, m_lb_mail_dir.c_str());
 if ( !m_jp_contact.empty() )  glite_jpcl_SetParam(*jpcl_context, GLITE_JPCL_PARAM_JPPS, m_jp_contact.c_str());

 if ( glite_jpimporter_upload_files(*jpcl_context, m_jobid.c_str(), &files[0], m_jp_proxy_file.c_str()) ) {
   char *errt, *errd;
   glite_jpcl_Error(*jpcl_context, &errt, &errd);
   throw importer_upload_exception(errt, errd);
 }
}

}
}
}
#endif
