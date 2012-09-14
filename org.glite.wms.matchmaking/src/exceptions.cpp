// File: exceptions.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Author: Salvatore Monforte  <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2003 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include "exceptions.h"
#include <string>

namespace glite {
namespace wms {
namespace matchmaking {

namespace {	
std::string const empty_string;
int null_port;
}

class InformationServiceError::Impl
{
public:
  std::string m_host;
  std::string m_filter;
  std::string m_dn;
  int m_port;
  std::string m_what;
};

InformationServiceError::InformationServiceError(const std::string& h,
		int p,const std::string& d, const std::string& f)
try
{
  m_impl.reset(new Impl);
  m_impl->m_host = h;
  m_impl->m_port = p;
  m_impl->m_dn = d;
  m_impl->m_filter = f;
} catch (...) {
  m_impl.reset();
}

InformationServiceError::~InformationServiceError() throw()
{
}

std::string InformationServiceError::host() const
{
  return m_impl ? m_impl->m_host : empty_string;
}

int InformationServiceError::port() const
{
  return m_impl ? m_impl->m_port : null_port;	
}

std::string InformationServiceError::filter() const
{
  return m_impl ? m_impl->m_filter : empty_string;
}

std::string InformationServiceError::dn() const
{
	  return m_impl ? m_impl->m_dn : empty_string;
}

const char*
InformationServiceError::what() const throw()
{
  if (m_impl) {
    if (m_impl->m_what.empty()) {
      m_impl->m_what = "Problems querying the information service " + m_impl->m_host;
    }
    return m_impl->m_what.c_str();
  } else {
    return "MatchMaking: InformationServiceError";
  }
}

ISQueryError::ISQueryError(const std::string& h, int p, const std::string& d, const std::string& f) 
	: InformationServiceError(h,p,d,f)

{
};

ISConnectionError::ISConnectionError(const std::string& h, int p, const std::string& d)
	: InformationServiceError(h,p,d, "")
{
};

ISClusterQueryError::ISClusterQueryError(const std::string& h, int p, const std::string& d)
        : InformationServiceError(h,p,d, "")
{
};

ISNoResultError::ISNoResultError(const std::string& h, int p, const std::string& d, const std::string& f)
	        : InformationServiceError(h,p,d,f)
{
};

}}} // glite::wms::matchmaking

