dnl Usage:
dnl AC_GLITE_WMS_MATCHMAING
dnl - GLITE_WMS_MATCHMAKING_CFLAGS
dnl - GLITE_WMS_MATHCMAKING_LIBS

AC_DEFUN(AC_GLITE_WMS_MATCHMAKING,
[
    ac_glite_wms_match_prefix=$GLITE_LOCATION

    if test -n "ac_glite_wms_match_prefix" ; then
    	GLITE_WMS_MATCHMAKING_CFLAGS="-I$ac_glite_wms_match_prefix/include/glite/wms/matchmaking"
	dnl
	dnl 
	dnl
        ac_glite_wms_match_lib="-L$ac_glite_wms_match_prefix/lib"
	GLITE_WMS_MATCHMAKING_LIBS="$ac_glite_wms_match_lib -lglite_wms_matchmaking"
	ifelse([$2], , :, [$2])
    else
	GLITE_WMS_MATCHMAKING_CFLAGS=""
	GLITE_WMS_MATCHMAKING_LIBS=""
	ifelse([$3], , :, [$3])
    fi

    AC_SUBST(GLITE_WMS_MATCHMAKING_CFLAGS)
    AC_SUBST(GLITE_WMS_MATCHMAKING_LIBS)
])

