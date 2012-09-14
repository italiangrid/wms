
/**
 * File: Version.cpp
 * Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
 * Copyright (c) 200e EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$
 
#include <string>
#include <boost/regex.hpp>
#include "Version.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace versions{

  Version::Version() {
    major = 1;
    minor = 0;
    revision = 0;
  }

  Version::Version(const std::string& ver) {

    static boost::regex  expression( "([0-9].*).([0-9].*).([0-9].*)" );
    boost::smatch        pieces;
    std::string          sminor, smajor, srevision;
 
    if( boost::regex_match(ver, pieces, expression) ) {
 
      sminor.assign     (pieces[1].first, pieces[1].second);
      smajor.assign     (pieces[2].first, pieces[2].second);
      srevision.assign  (pieces[3].first, pieces[3].second);
    }

    minor    = std::atoi(sminor.c_str());
    major    = std::atoi(smajor.c_str());
    revision = std::atoi(srevision.c_str());
  }  

  Version::Version(int maj, int min, int rev) {
    major = maj;
    minor = min;
    revision = rev;
  }

  std::string Version::asString() {
    char buf[32];
    sprintf(buf, "%d.%d.%d", this->major, this->minor, this->revision);
    return std::string(buf);
  }

  void Version::getVersion(std::string& v) {
    v.append(this->asString());
  }
  
  bool Version::operator== (const Version& v) {
    return (this->major==v.major)&&(this->minor==v.minor)&&(this->revision==v.revision);
  }

  bool Version::operator>  (const Version& v) {
    return (this->major > v.major) 
      || 
      ((this->major == v.major)&&(this->minor > v.minor))
      ||
      ((this->major == v.major)&&(this->minor == v.minor)&&(this -> revision > v.revision));
  }

  bool Version::operator<  (const Version& v) {
    return (this->major < v.major) 
      || 
      ((this->major == v.major)&&(this->minor < v.minor))
      ||
      ((this->major == v.major)&&(this->minor == v.minor)&&(this -> revision < v.revision));
  }


}}}}} // namespace glite::wms::manager::ns::versions
