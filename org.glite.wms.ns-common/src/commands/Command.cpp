/* 
 * Command.cpp
 * 
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html.
 */

// $Id$
 
#include <string>
#include <classad_distribution.h>
#include "Command.h"
#include "CommandState.h"
#include "logging.h"
#include "glite/wmsutils/tls/socket++/GSISocketAgent.h"
#include "glite/wmsutils/classads/classad_utils.h"

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

namespace logger    = glite::wms::common::logger;
namespace socket_pp = glite::wmsutils::tls::socket_pp;
namespace utils = glite::wmsutils::classads;

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

Command::Command()
{
  ctx.reset( new edg_wll_Context() );
  jobId.reset( new edg_wlc_JobId() );
  ad = NULL;
  fsm = NULL;
}

Command::~Command()
{
 edg_wll_FreeContext(*ctx);
 edg_wlc_JobIdFree(*jobId);
 if (fsm != NULL) delete fsm;
 if (ad != NULL) delete ad;
}

classad::ClassAd& Command::asClassAd() const 
{ 
  assert( ad != NULL );
  return (*ad);
}

bool Command::serialize(socket_pp::SocketAgent *sck)
{
  this -> sck = sck;
  return serializeImpl(sck, this);
}

bool Command::execute()
{  
  if( this -> fsm -> empty() ) { return false; }

  ns::fsm::CommandState::shared_ptr state(fsm -> front());
  this -> fsm -> pop();

#ifdef DEBUG
  edglog_fn("Command");
  edglog(veryugly) << utils::asString(*ad) << std::endl; 
#endif
  return state -> execute(this);
}

bool Command::isDone() const
{
  return this -> fsm -> empty();
}

const fsm::CommandState& Command::state()
{
  assert( !this -> fsm -> empty() );
  return *this -> fsm -> front();
}

socket_pp::SocketAgent& Command::agent() const 
{
  assert( sck );
  return *sck;
}

std::string Command::name()
{
  std::string nm;
  assert(ad && ad -> EvaluateAttrString("Command",nm));
  return nm;
}

std::string Command::version()
{
  std::string ver;
  assert(ad && ad -> EvaluateAttrString("Version",ver));
  return ver;
}

bool Command::getParam(const std::string& name, bool& b)
{
  classad::ExprTree* et = ad -> Lookup("Arguments");
  return(utils::is_classad(et) && static_cast<classad::ClassAd*>(et)->EvaluateAttrBool(name, b));
}

bool Command::getParam(const std::string& name, int& i)
{
  classad::ExprTree* et = ad -> Lookup("Arguments");
  return(utils::is_classad(et) && static_cast<classad::ClassAd*>(et)->EvaluateAttrInt(name, i));
}

bool Command::getParam(const std::string& name, double& d)
{
  classad::ExprTree* et = ad -> Lookup("Arguments");
  return(utils::is_classad(et) && static_cast<classad::ClassAd*>(et)->EvaluateAttrReal(name, d));
}

bool Command::getParam(const std::string& name, std::string& s)
{ 
  classad::ExprTree* et = ad -> Lookup("Arguments");
  return(utils::is_classad(et) && static_cast<classad::ClassAd*>(et)->EvaluateAttrString(name, s));
}

bool Command::getParam(const std::string& name, std::vector<std::string>& v)
{
  classad::ExprTree* et = ad -> Lookup("Arguments");
  if(utils::is_classad(et)){
    classad::ClassAd* a = static_cast<classad::ClassAd*>(et);
    return(utils::EvaluateAttrList(*a,name, v));
  }
  return false;
}

bool Command::setParam( const std::string &name, bool b )
{ 
  classad::ExprTree* et = ad -> Lookup("Arguments");
  return(utils::is_classad(et) && static_cast<classad::ClassAd*>(et)->InsertAttr(name, b));
}

bool Command::setParam( const std::string &name, int i )
{ 
  classad::ExprTree* et = ad -> Lookup("Arguments");
  return(utils::is_classad(et) && static_cast<classad::ClassAd*>(et)->InsertAttr(name, i));
}

bool Command::setParam( const std::string &name, double d )
{ 
  classad::ExprTree* et = ad -> Lookup("Arguments");
  return(utils::is_classad(et) && static_cast<classad::ClassAd*>(et)->InsertAttr(name, d));
}

bool Command::setParam( const std::string &name, const std::string &s )
{ 

  classad::ExprTree* et = ad -> Lookup("Arguments");
  return(utils::is_classad(et) && static_cast<classad::ClassAd*>(et)->InsertAttr(name, s));
}

bool Command::setParam( const std::string &name, classad::ClassAd *pad )
{
  classad::ExprTree* et = ad -> Lookup("Arguments");
  return(utils::is_classad(et) && static_cast<classad::ClassAd*>(et)->Insert(name, pad->Copy()));
}

bool Command::setParam( const std::string &name, const std::vector<std::string> &v )
{ 
  classad::ExprTree* et = ad -> Lookup("Arguments");
  if(utils::is_classad(et)){
    classad::ClassAd* a = static_cast<classad::ClassAd*>(et);
    return(utils::InsertAttrList(*a,name, v));
  }
  return false;
}

edg_wll_Context* Command::getLogContext() {
  return ctx.get();
}

edg_wlc_JobId* Command::getLogJobId() {
  return jobId.get();
}

} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite
