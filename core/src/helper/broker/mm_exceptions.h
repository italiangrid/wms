/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the
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

// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2003 EU DataGrid.

// $Id: mm_exceptions.h,v 1.1.2.2 2012/09/12 10:02:12 mcecchi Exp $

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
	ISQueryError(
    const std::string& hostname, 
    int port,
    const std::string& dn,
    const std::string& filter
  );
};

struct ISConnectionError : InformationServiceError
{
  ISConnectionError(
    const std::string& hostname, 
		int port,
    const std::string& dn
  );
};

struct ISClusterQueryError : InformationServiceError
{
	ISClusterQueryError(const std::string& hostname,
			int port, const std::string& dn);
};

struct ISNoResultError : InformationServiceError
{
	ISNoResultError(const std::string& hostname,
			int port, const std::string& dn, const std::string& filter);
};

struct RankingError : MatchMakingError
{
	RankingError() { }
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
