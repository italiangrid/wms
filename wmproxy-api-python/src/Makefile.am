## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************


wmppyapilibdir = $(libdir)/python$(PYTHON_MAJOR).$(PYTHON_MINOR)/site-packages
wmppyapilib_SCRIPTS=WMPClient.py \
	wmproxymethods.py \
	WMPConnection.py 

install-data-hook:
	python -m compileall -l $(DESTDIR)/$(wmppyapilibdir)
	python -O -m compileall -l $(DESTDIR)/$(wmppyapilibdir)


MAINTAINERCLEANFILES = Makefile.in

