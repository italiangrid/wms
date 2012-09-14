#
#       Copyright (c) 2004 on behalf of the EU EGEE Project:
#       The European Organization for Nuclear Research (CERN),
#       Istituto Nazionale di Fisica Nucleare (INFN), Italy
#       Datamat Spa, Italy
#       Centre National de la Recherche Scientifique (CNRS), France
#       CS Systeme d'Information (CSSI), France
#       Royal Institute of Technology, Center for Parallel Computers (KTH-PDC), Sweden
#       Universiteit van Amsterdam (UvA), Netherlands
#       University of Helsinki (UH.HIP), Finland
#       University of Bergen (UiB), Norway
#       Council for the Central Laboratory of the Research Councils (CCLRC), United Kingdom
#
#       Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
#       Version info: $Id$
#       Release: $Name$
#
#       Revision history:
#       $Log$
#       Revision 1.1.34.1.2.2  2012/02/10 16:17:05  mcecchi
#       filelist in isolated JC/LM locations. possible future redesign
#
#       Revision 1.1.34.1.2.1  2012/02/07 14:36:29  mcecchi
#       removed support for filelist - work in progress
#
#       Revision 1.1.34.1  2011/01/05 11:10:57  pandreet
#       Pkg-config support for jss
#
#       Revision 1.1  2004/07/06 10:09:03  pandreet
#       Insert jobcontrol_namespace and Makefile
#

SUBDIRS = common controller logmonitor daemons

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wms-jss-controller-adapter.pc \
		wms-jss-controller-wrapper.pc \
		wms-jss-common.pc \
		wms-jss-controller.pc \
		wms-jss-logmonitor.pc

MAINTAINERCLEANFILES = Makefile.in
