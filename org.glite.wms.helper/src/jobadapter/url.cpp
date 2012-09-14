/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

//**************************************************************************
//  Filename  : url.cpp
//  Authors   : Elisabetta Ronchieri
//              Francesco Giacomini
//              Marco Cecchi
//**************************************************************************

//Formalization of URI in BNF according to RFC #3986 Jan 2005
// (restricted to our own needs)

//URI           = scheme ":" hier-part
//hier-part     = "//" authority path-abempty
//               / path-absolute
//               / path-rootless
//               / path-empty
//scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
//authority     = host [ ":" port ]
//host          = IP-literal / IPv4address / reg-name
//port          = *DIGIT
//path-abempty  = *( "/" segment )
//path-absolute = "/" [ segment-nz *( "/" segment ) ]
//path-noscheme = segment-nz-nc *( "/" segment )
//path-rootless = segment-nz *( "/" segment )
//path-empty    = 0<pchar>
//segment       = *pchar
//segment-nz    = 1*pchar
//segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
//; non-zero-length segment without any colon ":"
//pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
//query         = *( pchar / "/" / "?" )
//fragment      = *( pchar / "/" / "?" )
//pct-encoded   = "%" HEXDIG HEXDIG
//unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
//sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
//
//Some examples:
//"http://www.cnaf.infn.it"                   = Valid
//"http://www.cnaf.infn.it:"                  = Valid (port = *DIGIT)
//"h ttp://www.cnaf.infn.it"                  = Not valid
//"http://www.c naf.infn.it"                  = Not valid
//"http://www.ics.uci.edu:8080/pub/ietf/uri/" = Valid
//"http://www.ics.uci.edu:8080/pub/ietf/uri"  = Valid
//"http://www.ics.uci.edu:8080"               = Valid
//"http://www.ics.uci.edu:8080:16"            = Not valid
//"http://www.ics.uci.edu:8080/pub/i:etf/uri" = Not valid
//"http://www.ics.uci.edu:8080/pub/i%20f/uri" = Valid
//"http://www.ics.uci.edu:8080/pub/i%2tf/uri" = Not valid
//""                                          = Not valid

#include <string>
#include <iostream>
#include <boost/regex.hpp>

#include "url.h"

namespace glite {
namespace wms {
namespace helper {
namespace jobadapter {

InvalidURL::InvalidURL(std::string const& url)
  : m_message("Invalid URL: " + url)
{
}

InvalidURL::~InvalidURL() throw()
{
}

char const* InvalidURL::what() const throw()
{
  return m_message.c_str();
}

URL::URL(std::string const& url)
{

  static const boost::regex valid_url(
    "([[:alpha:]][[:alnum:]+.-]*)" // scheme
    "://"
    "(([[:alnum:]_.~!$&'()-]|%[[:xdigit:]]{2})+)" // host
    "(:([[:digit:]]*))?" // port
    "((/([[:alpha:][:digit:]_.~!$&'()-]|%[[:xdigit:]]{2})+)*)/?" // path
  );

  try {
    boost::smatch pieces;
    if (boost::regex_match(url, pieces, valid_url)) {
      m_protocol.assign(pieces[1].first, pieces[1].second);
      m_host.assign(pieces[2].first, pieces[2].second);
      m_port.assign(pieces[5].first, pieces[5].second);
      m_path.assign(pieces[6].first, pieces[6].second);
    } else {
      throw InvalidURL(url);
    }
  } catch (std::runtime_error& e) {
    throw InvalidURL(url);
  }
}

std::string
URL::protocol() const
{
  return m_protocol;
}

std::string
URL::host() const
{
  return m_host;
}

std::string
URL::port() const
{
  return m_port;
}

std::string
URL::path() const
{
  return m_path;
}

std::string
URL::as_string() const
{
  return m_protocol
    + "://"
    + m_host
    + (!m_port.empty() ? ":" + m_port : "")
    + m_path;
}

}}}} // namespace glite::wms::helper::jobadapter
