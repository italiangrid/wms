dnl Usage:
dnl AC_GLITE_WMS_BROKERINFO
dnl - GLITE_WMS_BROKERINFO_LIBS

AC_DEFUN(AC_GLITE_WMS_BROKERINFO,
[
    ac_glite_wms_brokerinfo_prefix=$GLITE_LOCATION

    if test -n "ac_glite_wms_bokerinfo_prefix" ; then
	dnl
	dnl 
	dnl
        ac_glite_wms_brokerinfo_lib="-L$ac_glite_wms_brokerinfo_prefix/lib"
	GLITE_WMS_BROKERINFO_LIBS="$ac_glite_wms_brokerinfo_lib -lglite_wms_brokerinfo"
	ifelse([$2], , :, [$2])
    else
	GLITE_WMS_BROKERINFO_LIBS=""
	ifelse([$3], , :, [$3])
    fi

    AC_SUBST(GLITE_WMS_BROKERINFO_LIBS)
])

