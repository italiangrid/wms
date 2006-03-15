// File: edg-wl-eval_ad_expr.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
// Copyright (c) 2003 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include <iostream>
#include <fstream>
#include <cstring>
#include <boost/scoped_ptr.hpp>
#include <classad_distribution.h>
#include <value.h>

#include "glite/wmsutils/classads/classad_utils.h"

namespace utilities = glite::wmsutils::classads;

namespace {

std::string program_name;

void usage(std::ostream& os)
{
  os << "Usage: " << program_name << " classad_file expression\n";
}

} // {anonymous}

int
main(int argc, char* argv[])
try {
  program_name = argv[0];

  if (argc != 3) {
    usage(std::cerr);
    return EXIT_FAILURE;
  }

  std::string ad_file(argv[1]);
  std::string expression(argv[2]);

  std::ifstream is(ad_file.c_str());
  if (!is) {
    std::cerr << "Cannot open " << ad_file << "\n";
    return EXIT_FAILURE;
  }

  boost::scoped_ptr<classad::ClassAd> ad(utilities::parse_classad(is));
  classad::Value v;
  if (ad->EvaluateExpr(expression, v)) {
    std::cout << v << "\n";
  } else {
    std::cerr << "Evaluation failed\n";
    return EXIT_FAILURE;
  }

} catch (std::exception& e) {
  std::cerr << e.what() << "\n";
  return EXIT_FAILURE;
} catch (...) {
  std::cerr << "Caught generic exception\n";
  return EXIT_FAILURE;
}
