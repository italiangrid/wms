dnl Usage:
dnl AC_GLITE_WMS_WMPROXY
dnl - GLITE_WMS_WMPROXY_LIBS

AC_DEFUN(AC_GLITE_WMS_WMPROXY,
[
    ac_glite_wms_wmproxy_prefix=$GLITE_LOCATION

    if test -n "ac_glite_wms_wmproxy_prefix" ; then
	dnl
	dnl 
	dnl
        ac_glite_wms_wmproxy_lib="-L$ac_glite_wms_wmproxy_prefix/lib"
	GLITE_WMS_WMPROXY_LIBS="$ac_glite_wms_wmproxy_lib -lglite_wms_wmproxy_api_cpp"
	ifelse([$2], , :, [$2])
    else
	GLITE_WMS_WMPROXY_CFLAGS=""
	GLITE_WMS_WMPROXY_LIBS=""
	ifelse([$3], , :, [$3])
    fi

    AC_SUBST(GLITE_WMS_WMPROXY_LIBS)
])

