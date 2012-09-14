/*
 * Copyright (c) Members of the EGEE Collaboration. 2004. 
 * See http://www.eu-egee.org/partners/ for details on the copyright
 * holders.  
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at 
 *
 *     http://www.apache.org/licenses/LICENSE-2.0 
 *
 *     Unless required by applicable law or agreed to in writing, software 
 *     distributed under the License is distributed on an "AS IS" BASIS, 
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 *     See the License for the specific language governing permissions and 
 *     limitations under the License.
 *     */

// File: filelist_reader.h
// Author: Francesco Giacomini
//
// $Id$

#ifndef GLITE_WMS_JOBSUBMISSION_COMMON_FILELISTREADER_H
#define GLITE_WMS_JOBSUBMISSION_COMMON_FILELISTREADER_H

#include <boost/shared_ptr.hpp>

#include "common/FLExtractor.h"

#include "input_reader.h"
#include "jobcontrol_namespace.h"

JOBCONTROL_NAMESPACE_BEGIN {
namespace jccommon {

class FileListItem: public InputItem
{
   class Impl;
   boost::shared_ptr<Impl> m_impl;
   typedef FLExtractor<std::string> Extractor;
   typedef boost::shared_ptr<Extractor> ExtractorPtr;
   typedef Extractor::iterator extractor_iterator;
public:
   FileListItem(ExtractorPtr extractor, extractor_iterator it);
   std::string value() const;
   void remove_from_input();
};

class FileListReader: public InputReader
{
   class Impl;
   boost::shared_ptr<Impl> m_impl;
public:
   FileListReader(std::string const& source);
   std::string name() const;
   std::string source() const;
   InputItems read();
};


} // namespace jccommon
} JOBCONTROL_NAMESPACE_END

#endif

