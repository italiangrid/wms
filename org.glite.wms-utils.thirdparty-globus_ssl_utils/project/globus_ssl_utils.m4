dnl Usage:
dnl AC_GLOBUS_SSL_UTILS

AC_DEFUN(AC_GLOBUS_SSL_UTILS,
[

    AC_MSG_RESULT([GLOBUS_SSL_UTILS under $GLITE_LOCATION])
    ssl_utils="$GLITE_LOCATION" 

    # On solaris openssl needs the socket and nsl library

    AC_CHECK_HEADERS(malloc.h sys/time.h unistd.h)

    have_gsu=no

    AC_MSG_CHECKING(checking for time_t timezone in <time.h>)

    AC_TRY_COMPILE([ #include <time.h> ],
	    [ struct tm y;
	      time_t offset = 3;
	      time_t x = mktime(&y) + offset*60*60 - timezone; ],
	    [answer=yes], [answer=no])

    if test "x$answer" = "xyes" ; then
       have_gsu=yes
       AC_MSG_RESULT(yes)
       AC_DEFINE(HAVE_TIME_T_TIMEZONE, 1, [Define if you have time_t timezone in <time.h>.])
    else
       dnl
       dnl only place this should occur is on CYGWIN B20, which has an
       dnl integer _timezone defined instead
       dnl
       AC_MSG_CHECKING(checking for time_t _timezone in <time.h>)
       AC_TRY_COMPILE([ #include <time.h> ],
	    [ struct tm y;
	      time_t offset = 3;
	      time_t x = mktime(&y) + offset*60*60 - _timezone; ],
	    [answer=yes], [answer=no])

       if test "x$answer" = "xyes" ; then
	   have_gsu=yes
	   AC_MSG_RESULT(yes)
	   AC_DEFINE(HAVE_TIME_T__TIMEZONE, 1, [Define if you have time_t _timezone in <time.h>.])
       else
	   have_gsu=no
	   AC_MSG_RESULT(no)
       fi
    fi

    if test "x$answer" = "xyes" ; then
	ifelse([$2], , :, [$2])
    else
	ifelse([$3], , :, [$3])
    fi

])

