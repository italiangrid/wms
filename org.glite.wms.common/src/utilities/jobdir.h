// File: jobdir.h
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$
 
#ifndef GLITE_WMS_COMMON_UTILITIES_JOBDIR_H
#define GLITE_WMS_COMMON_UTILITIES_JOBDIR_H

#include <string>
#include <exception>
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

namespace glite {
namespace wms {
namespace common {
namespace utilities {

class JobDirError: public std::exception
{
public:
  ~JobDirError() throw();
  char const* what() const throw();
};

class JobDir
{
  class Impl;
  boost::shared_ptr<Impl> m_impl;

public:
  JobDir(boost::filesystem::path const& base_dir);
  boost::filesystem::path const& base_dir() const;

  boost::filesystem::path deliver(
    std::string const& contents,
    std::string const& tag = std::string()
  );
  boost::filesystem::path set_old(boost::filesystem::path const& file);

  boost::filesystem::directory_iterator new_entries();
  boost::filesystem::directory_iterator old_entries();

public:
  static bool create(boost::filesystem::path const& base_dir);
};

}}}} // glite::wms::common::utilities

#endif

