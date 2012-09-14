dnl AM_PATH_SQLITE3([ ])
dnl define SQLITE3_CFLAGS and SQLITE3_LIBS
dnl
AC_DEFUN([AM_PATH_SQLITE3],
[
	# Set SQLITE3 Properties
	AC_ARG_WITH(sqlite3_prefix,
		[  --with-sqlite3-prefix=PFX     prefix where sqlite3 is installed. (/opt/sqlite3)],
		[],
	        with_sqlite3_prefix=${SQLITE3_PATH:-/opt/sqlite3})
	        
	SQLITE3_PATH="$with_sqlite3_prefix"
	SQLITE3_CFLAGS="-I$with_sqlite3_prefix/include"
	ac_sqlite3_ldlib="$with_sqlite3_prefix/lib"
	SQLITE3_LIBS="$ac_sqlite3_ldlib/libsqlite3.a"
	    
	AC_SUBST(SQLITE3_PATH)
	AC_SUBST(SQLITE3_CFLAGS)
	AC_SUBST(SQLITE3_LIBS)
])
