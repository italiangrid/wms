
check_PROGRAMS = t_jobid_to_filename

t_jobid_to_filename_SOURCES = t_jobid_to_filename.cpp
t_jobid_to_filename_LDFLAGS = $(GLITE_LDFLAGS)
t_jobid_to_filename_LDADD = \
	$(GLITE_JOBID_LIBS) \
	$(BOOST_FS_LIBS) \
	$(CPPUNIT_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/interface \
        $(GLOBUS_CFLAGS) \
        $(BOOST_CFLAGS) \
        $(GLITE_CPPFLAGS) \
        $(CPPUNIT_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~

