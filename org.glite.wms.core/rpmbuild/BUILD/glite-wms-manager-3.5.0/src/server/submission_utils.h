// File: submission_utils.h
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#ifndef GLITE_WMS_MANAGER_SERVER_SUBMISSION_UTILS_H
#define GLITE_WMS_MANAGER_SERVER_SUBMISSION_UTILS_H

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

#include <openssl/pem.h>
#include <openssl/x509.h>

namespace classad {
class ClassAd;
}

namespace glite {

namespace wmsutils {
namespace jobid {
class JobId;
}}

namespace wms {
namespace manager {
namespace server {

class HitMaxRetryCount
{
  int m_n;
public:
  HitMaxRetryCount(int n): m_n(n) {}
  int count() const { return m_n; }
};

class HitJobRetryCount
{
  int m_n;
public:
  HitJobRetryCount(int n): m_n(n) {}
  int count() const { return m_n; }
};

class HitMaxShallowCount
{
  int m_n;
public:
  HitMaxShallowCount(int n): m_n(n) {}
  int count() const { return m_n; }
};

class HitJobShallowCount
{
  int m_n;
public:
  HitJobShallowCount(int n): m_n(n) {}
  int count() const { return m_n; }
};

class MissingProxy
{
};

class InvalidProxy
{
};

class CannotRetrieveJDL
{
};

boost::filesystem::path
get_reallyrunning_token(glite::wmsutils::jobid::JobId const& id);
void check_shallow_count(classad::ClassAd const& jdl, int count);
void check_deep_count(classad::ClassAd const& jdl, int count);
bool shallow_resubmission_is_enabled(classad::ClassAd const& jdl);
void create_token(boost::filesystem::path const& p);
boost::shared_ptr<X509> read_proxy(std::string const& proxy_file);
bool is_proxy_expired(glite::wmsutils::jobid::JobId const& id);
bool is_proxy_expired(
  boost::shared_ptr<X509>& cert,
  glite::wmsutils::jobid::JobId const& id
);

}}}} // glite::wms::manager::server

#endif
