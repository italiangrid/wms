#ifndef GLITE_WMS_COMMON_UTILITIES_UTILS_H
#define GLITE_WMS_COMMON_UTILITIES_UTILS_H
/*
 * utils.h
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 *
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STRINGSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <unistd.h>

#define  CHAR_BUFFER_SIZE 1024

namespace glite {
namespace wms {
namespace common {
namespace utilities {

/**
 * std:string useful JSUI common utilized methods
 *
 * @ingroup Common
 * @version 0.1
 * @date 15 April 2002
 * @author Alessandro Maraschini <alessandro.maraschini@datamat.it>
*/

/**
* Check if a string can be converted into an integer
* @ingroup Common
*/
int isInt (const std::string& str);
/**
*  Converts a string into an Integer value
* @ingroup Common
*/
int toInt (const std::string& str, int &sum);
/**
*  Converts a string into an Hexasdecimal value
* @ingroup Common
*/
int toHex (const std::string& str, int &sum);
/**
*  Converts an integer into a string
* @ingroup Common
*/
std::string inTo(int i);
/**
*  Counts number of sub-string occurrences  in a string
* @ingroup Common
*/
int count(const std::string& strMain, const std::string& sep);
/**
* Splits a string into a vector of substring
* @param   str - The string to be parsed
* @param   sep - The separator to be found
* @param   maxLength - max dimension of vector (the last item of the vector will be the lasting part)
* @param  trough -  if trough == 1 the separator will be copied at the end of the left-substring
* @ingroup Common
*/
std::vector<std::string> split(const std::string& str, const std::string& sep, int maxLength =1000, int trough = 0);
std::string sp(const std::string& separator);
int checkFormat (const std::string& format, const std::string& str);

void
replace(std::string& where, const std::string& what, const std::string& with);

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace
   
#endif
