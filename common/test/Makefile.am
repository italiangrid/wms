
bin_PROGRAMS = glite-ftp-get glite-ftp-put

glite_ftp_get_SOURCES = glite-ftp-get.cpp
glite_ftp_get_LDADD   = \
    $(top_srcdir)/../stage/lib/libglite_wms_globus_ftp_util.la \
    $(top_srcdir)/../stage/lib/libglite_wms_logger.la \
    $(GLOBUS_FTP_CLIENT_NOTHR_LIBS) \
    $(BOOST_LIBS)

glite_ftp_put_SOURCES = glite-ftp-put.cpp
glite_ftp_put_LDADD   = \
    $(top_srcdir)/../stage/lib/libglite_wms_globus_ftp_util.la \
    $(top_srcdir)/../stage/lib/libglite_wms_logger.la \
    $(GLOBUS_FTP_CLIENT_NOTHR_LIBS) \
    $(BOOST_LIBS)


AM_CPPFLAGS = -I$(top_srcdir)/interface \
        -I$(top_srcdir)/src/utilities \
        -I$(top_srcidr)/test \
        $(GLOBUS_CFLAGS) \
        $(BOOST_CFLAGS) \
        $(CPPUNIT_CFLAGS)

MAINTAINERCLEANFILES = Makefile.in *~

