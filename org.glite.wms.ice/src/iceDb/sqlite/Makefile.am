#  Copyright (c) Members of the EGEE Collaboration. 2004. 
#  See http://www.eu-egee.org/partners/ for details on the copyright
#  holders.  
#  
#  Licensed under the Apache License, Version 2.0 (the "License"); 
#  you may not use this file except in compliance with the License. 
#  You may obtain a copy of the License at 
#  
#      http://www.apache.org/licenses/LICENSE-2.0 
#  
#  Unless required by applicable law or agreed to in writing, software 
#  distributed under the License is distributed on an "AS IS" BASIS, 
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
#  See the License for the specific language governing permissions and 
#  limitations under the License.

noinst_LIBRARIES = libsqlite3.a

libsqlite3_a_SOURCES = sqlite3.c

AM_CPPFLAGS = -Wall -O4 \
              -DSQLITE_THREADSAFE=1 \
              -D_FILE_OFFSET_BITS=64 \
              -DSQLITE_ENABLE_MEMORY_MANAGEMENT=1 \
              -DSQLITE_ENABLE_UPDATE_DELETE_LIMIT=1 \
              -DSTDC_HEADERS=1 \
              -DHAVE_SYS_TYPES_H=1 \
              -DHAVE_SYS_STAT_H=1 \
              -DHAVE_STDLIB_H=1 \
              -DHAVE_STRING_H=1 \
              -DHAVE_MEMORY_H=1 \
              -DHAVE_STRINGS_H=1 \
              -DHAVE_INTTYPES_H=1 \
              -DHAVE_STDINT_H=1 \
              -DHAVE_UNISTD_H=1 \
              -DHAVE_DLFCN_H=1 \
              -DHAVE_FDATASYNC=1 \
              -DHAVE_USLEEP=1 \
              -DHAVE_LOCALTIME_R=1 \
              -DHAVE_GMTIME_R=1 \
              -DHAVE_READLINE=1 \
              -I$(top_srcdir)/src/iceDb/sqlite

MAINTAINERCLEANFILES = Makefile.in
