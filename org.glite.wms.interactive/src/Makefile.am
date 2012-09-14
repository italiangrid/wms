## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

#wlindocdir = $(datadir)/doc/edg-wl-interactive-@VERSION@
#wlindoc_DATA = LICENSE README

bin_PROGRAMS = \
	glite-wms-pipe-input \
	glite-wms-pipe-output \
	glite-wms-job-agent

glite_wms_pipe_input_SOURCES = glite_wms_pipe_input.c
glite_wms_pipe_input_LDFLAGS = -Xlinker --export-dynamic
glite_wms_pipe_input_LDADD = \
	$(GLITE_WMS_TP_BYPASS_LIBS) \
        $(GLITE_LB_NOTHR_CLIENT_LIBS) \
        $(GLITE_LB_NOTHR_COMMON_LIBS) \
        $(GLITE_WMSUTILS_JOBID_LIBS) \
        $(GLITE_WMSUTILS_CJOBID_LIBS) \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
        -lnsl -ldl -lstdc++

glite_wms_pipe_output_SOURCES = glite_wms_pipe_output.c
glite_wms_pipe_output_LDFLAGS = -Xlinker --export-dynamic
glite_wms_pipe_output_LDADD = \
	$(GLITE_WMS_TP_BYPASS_LIBS) \
        $(GLITE_LB_NOTHR_CLIENT_LIBS) \
        $(GLITE_LB_NOTHR_COMMON_LIBS) \
        $(GLITE_WMSUTILS_JOBID_LIBS) \
        $(GLITE_WMSUTILS_CJOBID_LIBS) \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
	-lnsl -ldl -lstdc++

glite_wms_job_agent_SOURCES = glite_wms_job_agent.c 
glite_wms_job_agent_LDFLAGS = -Xlinker --export-dynamic
glite_wms_job_agent_LDADD = \
        $(GLITE_WMS_TP_BYPASS_LIBS) \
        $(GLITE_LB_NOTHR_CLIENT_LIBS) \
        $(GLITE_LB_NOTHR_COMMON_LIBS) \
        $(GLITE_WMSUTILS_JOBID_LIBS) \
        $(GLITE_WMSUTILS_CJOBID_LIBS) \
        $(GLITE_WMSUTILS_EXCEPTION_LIBS) \
        -lnsl -ldl -lstdc++

AM_CPPFLAGS = -I$(top_srcdir)/src \
	$(GLITE_CFLAGS)

EXTRA_DIST = LICENSE README.in	

MAINTAINERCLEANFILES = Makefile.in *~
