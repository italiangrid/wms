# Copyright (c) Members of the EGEE Collaboration. 2004.
# See http://www.eu-egee.org/partners/ for details on the copyright
# holders.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

noinst_HEADERS = \
	EventFactory.h \
	EventInterface.h \
	EventSubmit.h \
	EventGlobusSubmit.h \
	EventGridSubmit.h \
	EventExecute.h \
	EventTerminated.h \
	EventPostTerminated.h \
	EventAborted.h \
	EventGlobusSubmitFailed.h \
	EventGlobusResourceDown.h \
	EventGridResourceDown.h \
	EventGlobusResourceUp.h \
	EventGridResourceUp.h \
	EventJobHeld.h \
	EventJobReleased.h \
	EventGeneric.h \
	EventUnhandled.h \
	MonitorData.h \
	SubmitReader.h \
	JobResubmitter.h

EXTRA_DIST = EventFactory.cpp \
	EventInterface.cpp \
	EventUnhandled.cpp \
	EventSubmit.cpp \
	EventGlobusSubmit.cpp \
	EventGridSubmit.cpp \
	EventExecute.cpp \
	EventTerminated.cpp \
	EventPostTerminated.cpp \
	EventAborted.cpp \
	EventGlobusSubmitFailed.cpp \
	EventGlobusResourceDown.cpp \
	EventGridResourceDown.cpp \
	EventGlobusResourceUp.cpp \
	EventGridResourceUp.cpp \
	EventJobHeld.cpp \
	EventJobReleased.cpp \
	EventGeneric.cpp \
	MonitorData.cpp \
	SubmitReader.cpp \
	JobResubmitter.cpp

AM_CPPFLAGS = -I/usr/include

MAINTAINERCLEANFILES = Makefile.in *~
