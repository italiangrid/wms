dnl Usage:
dnl AC_GLITE_WMS_HELPER
dnl - GLITE_WMS_HELPER_LIBS
dnl - GLITE_WMS_HELPER_JOBADAPTER_LIBS
dnl - GLITE_WMS_HELPER_BROKER_LIBS
dnl - GLITE_WMS_HELPER_BROKER_ISM_LIBS
AC_DEFUN(AC_GLITE_WMS_HELPER,
[
    ac_glite_wms_helper_prefix=$GLITE_LOCATION

    if test -n "ac_glite_wms_helper_prefix" ; then
	dnl
	dnl 
	dnl
        ac_glite_wms_helper_lib="-L$ac_glite_wms_helper_prefix/lib"
	GLITE_WMS_HELPER_LIBS="$ac_glite_wms_helper_lib -lglite_wms_helper"
        GLITE_WMS_HELPER_JOBADAPTER_LIBS="$ac_glite_wms_helper_lib -lglite_wms_helper_jobadapter"
        GLITE_WMS_HELPER_BROKER_LIBS="$ac_glite_wms_helper_lib -lglite_wms_helper_broker_ii"
	GLITE_WMS_HELPER_BROKER_ISM_LIBS="$ac_glite_wms_helper_ism_lib -lglite_wms_helper_broker_ism"
	ifelse([$2], , :, [$2])
    else
	GLITE_WMS_HELPER_LIBS=""
	GLITE_WMS_HELPER_JOBADAPTER_LIBS=""
	GLITE_WMS_HELPER_BROKER_LIBS=""
	GLITE_WMS_HELPER_BROKER_ISM_LIBS=""
	ifelse([$3], , :, [$3])
    fi

    AC_SUBST(GLITE_WMS_HELPER_LIBS)
    AC_SUBST(GLITE_WMS_HELPER_JOBADAPTER_LIBS)
    AC_SUBST(GLITE_WMS_HELPER_BROKER_LIBS)
    AC_SUBST(GLITE_WMS_HELPER_BROKER_ISM_LIBS)
])

