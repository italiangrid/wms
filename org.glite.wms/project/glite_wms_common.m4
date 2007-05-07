dnl Usage:
dnl AC_GLITE_WMS_COMMON
dnl - GLITE_WMS_COMMON_CONF_LIBS
dnl - GLITE_WMS_COMMON_CONF_WRAPPER_LIBS
dnl - GLITE_WMS_COMMON_CONFIG_LIBS
dnl - GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS
dnl - GLITE_WMS_COMMON_LOGGER_LIBS
dnl - GLITE_WMS_COMMON_PROCESS_LIBS
dnl - GLITE_WMS_COMMON_UT_UTIL_LIBS
dnl - GLITE_WMS_COMMON_UT_FTP_LIBS
dnl - GLITE_WMS_COMMON_UT_II_LIBS
dnl - GLITE_WMS_COMMON_SYSLOG_CFLAGS

AC_DEFUN(AC_GLITE_WMS_COMMON,
[
    ac_glite_wms_common_prefix=$GLITE_LOCATION

    if test -n "ac_glite_wms_common_prefix" ; then
	dnl
	dnl 
	dnl
        ac_glite_wms_common_lib="-L$ac_glite_wms_common_prefix/lib"
	GLITE_WMS_COMMON_CONF_LIBS="$ac_glite_wms_common_lib -lglite_wms_conf"
	GLITE_WMS_COMMON_CONF_WRAPPER_LIBS="$ac_glite_wms_common_lib -lglite_wms_conf_wrapper"
	GLITE_WMS_COMMON_CONFIG_LIBS="$GLITE_WMS_COMMON_CONF_LIBS -lglite_wms_conf_wrapper"
	GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS="$ac_glite_wms_common_lib -lglite_wms_LDIF2ClassAd"
	GLITE_WMS_COMMON_LOGGER_LIBS="$ac_glite_wms_common_lib -lglite_wms_logger"
	GLITE_WMS_COMMON_PROCESS_LIBS="$ac_glite_wms_common_lib -lglite_wms_process"
	GLITE_WMS_COMMON_UT_UTIL_LIBS="$ac_glite_wms_common_lib -lglite_wms_util"
	GLITE_WMS_COMMON_UT_FTP_LIBS="$ac_glite_wms_common_lib -lglite_wms_globus_ftp_util"
	GLITE_WMS_COMMON_UT_II_LIBS="$ac_glite_wms_common_lib -lglite_wms_iiattrutil"
	ifelse([$2], , :, [$2])
    else
	GLITE_WMS_COMMON_CONF_LIBS=""
	GLITE_WMS_COMMON_CONF_WRAPPER_LIBS=""
	GLITE_WMS_COMMON_CONFIG_LIBS=""
	GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS=""
	GLITE_WMS_COMMON_LOGGER_LIBS=""
	GLITE_WMS_COMMON_PROCESS_LIBS=""
	GLITE_WMS_COMMON_UT_UTIL_LIBS=""
	GLITE_WMS_COMMON_UT_FTP_LIBS=""
	GLITE_WMS_COMMON_UT_II_LIBS=""
	ifelse([$3], , :, [$3])
    fi

    AC_SUBST(GLITE_WMS_COMMON_CONF_LIBS)
    AC_SUBST(GLITE_WMS_COMMON_CONF_WRAPPER_LIBS)
    AC_SUBST(GLITE_WMS_COMMON_CONFIG_LIBS)
    AC_SUBST(GLITE_WMS_COMMON_LDIF2CLASSADS_LIBS)
    AC_SUBST(GLITE_WMS_COMMON_LOGGER_LIBS)
    AC_SUBST(GLITE_WMS_COMMON_PROCESS_LIBS)
    AC_SUBST(GLITE_WMS_COMMON_UT_UTIL_LIBS)
    AC_SUBST(GLITE_WMS_COMMON_UT_FTP_LIBS)
    AC_SUBST(GLITE_WMS_COMMON_UT_II_LIBS)

    AC_MSG_CHECKING([whether SYSLOG logging is enabled, or not])

    AC_ARG_ENABLE(syslog-logging,
        [  --enable-sysylog-logging=<option> Default is no],
        syslogopt="$enableval",
        syslogopt="no"
    )

    if test "x$syslogopt" = "xyes" ; then
        AC_MSG_RESULT([$syslogopt])
    else
        AC_MSG_RESULT(no)
    fi

    if test "x$syslogopt" = "xyes" ; then
       GLITE_WMS_COMMON_SYSLOG_CFLAGS="-DGLITE_WMS_HAVE_SYSLOG_LOGGING"
    else
       GLITE_WMS_COMMON_SYSLOG_CFLAGS=""
    fi
    AC_SUBST(GLITE_WMS_COMMON_SYSLOG_CFLAGS)


])

