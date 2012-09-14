// File: exceptions.h
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2003 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#ifndef GLITE_WMS_MATCHMAKING_EXCEPTIONS_H
#define GLITE_WMS_MATCHMAKING_EXCEPTIONS_H

#include <exception>
#include <boost/shared_ptr.hpp>

namespace glite {
namespace wms {
namespace matchmaking {

struct MatchMakingError: std::exception
{
  MatchMakingError() {}
  ~MatchMakingError() throw() {}
  char const* what() const throw()
  {
	  return "MatchMakingError";
  }
};

class InformationServiceError : public MatchMakingError
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;
public:
  InformationServiceError(const std::string& hostname, int port, 
		  const std::string& dn, const std::string& filter);
  ~InformationServiceError() throw();
  std::string host() const ;
  std::string filter() const ;
  std::string dn() const ;	  
  char const* what() const throw();
  int port() const ;
};

struct ISQueryError : InformationServiceError
{
	ISQueryError::ISQueryError(const std::string& hostname, 
			int port, const std::string& dn, const std::string& filter);		
};

struct ISConnectionError : InformationServiceError
{
        ISConnectionError::ISConnectionError(const std::string& hostname, 
			int port, const std::string& dn);
};

struct ISClusterQueryError : InformationServiceError
{
	ISClusterQueryError::ISClusterQueryError(const std::string& hostname,
			int port, const std::string& dn);
};

struct ISNoResultError : InformationServiceError
{
	ISNoResultError::ISNoResultError(const std::string& hostname,
			int port, const std::string& dn, const std::string& filter);
};

struct RankingError : MatchMakingError
{
	RankingError::RankingError() {}
	const char* what() const throw()
	{
		return "Problems during rank evaluation (e.g. GRISes down, wrong JDL rank expression, etc.)";
	}
};


}}} // glite::wms::matchmaking

#endif

// Local Variables:
// mode: c++
// End:
