
/*
 * common.h
 * 
 * Copyright (C) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#ifndef _GLITE_WMS_MANAGER_NS_CLIENT_COMMON_H_
#define _GLITE_WMS_MANAGER_NS_CLIENT_COMMON_H_


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <classad_distribution.h>

#include "glite/wms/common/utilities/edgstrstream.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace client {

extern "C++"
{
  void get_quoted_values(const std::string&, std::vector<std::string>&);
  std::ostream& operator <<(std::ostream& o, classad::ClassAd& ad);
  std::fstream& operator <<(std::fstream& f, classad::ClassAd& ad);
  void replace(std::string& , const std::string& , const std::string& ); 
  bool hostname_to_ip(const std::string& hostname, std::string& ip);
  bool resolve_host(const std::string& hostname, std::string& resolved_name);
  bool set_expression(classad::ClassAd* ad, const std::string& what, const std::string& with);
  bool getVectorValue(classad::ClassAd* from, const std::string& what, std::vector<std::string>&l);
  bool setVectorValue(classad::ClassAd* to, const std::string& name, const std::vector<std::string>& values);
  bool getListValue(classad::ClassAd* from, const std::string& what, std::list<std::string>&l);
  bool setListValue(classad::ClassAd* to, const std::string& name, const std::list<std::string>& values);
}

template <class T> std::string as_string(T a) {
 glite::wms::common::utilities::edgstrstream s;
 s << a << std::ends;
 std::string k(s.str());
 return k;
}

} // namespace client
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif

