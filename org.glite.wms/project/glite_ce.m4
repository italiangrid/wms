dnl Usage:
dnl AC_GLITE_CE
dnl - GLITE_CE_MONITOR_CLIENT_API_LIBS

AC_DEFUN(AC_GLITE_CE,
[
    ac_glite_ce_prefix=$GLITE_LOCATION

    if test -n "ac_glite_ce_prefix" ; then
	dnl
	dnl 
	dnl
        ac_glite_ce_lib="-L$ac_glite_cd_prefix/lib"
	GLITE_CE_MONITOR_CLIENT_API_LIBS="$ac_glite_ce_lib -lglite_ce_monitor_client"
	ifelse([$2], , :, [$2])
    else
	GLITE_CE_MONITOR_CLIENT_API_LIBS=""
	ifelse([$3], , :, [$3])
    fi

    AC_SUBST(GLITE_CE_MONITOR_CLIENT_API_LIBS)
])

