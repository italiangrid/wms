dnl Usage:
dnl AC_GLITE_WMS_ISM
dnl - GLITE_WMS_ISM_LIBS

AC_DEFUN(AC_GLITE_WMS_ISM,
[
    ac_glite_wms_ism_prefix=$GLITE_LOCATION

    if test -n "ac_glite_wms_ism_prefix" ; then
	dnl
	dnl 
	dnl
        ac_glite_wms_ism_lib="-L$ac_glite_wms_ism_prefix/lib"
	GLITE_WMS_ISM_LIBS="$ac_glite_wms_ism_lib -lglite_wms_ism"
	ifelse([$2], , :, [$2])
    else
	GLITE_WMS_ISM_LIBS=""
	ifelse([$3], , :, [$3])
    fi

    AC_SUBST(GLITE_WMS_ISM_LIBS)
])

