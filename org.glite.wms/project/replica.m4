dnl Usage:
dnl AC_REPLICA(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for rm, and defines
dnl - REPLICA_MANAGER_CFLAGS (compiler flags)
dnl - REPLICA_MANAGER_LIBS (linker flags, stripping and path)
dnl prerequisites:

AC_DEFUN(AC_REPLICA,
[
    AC_ARG_WITH(replica_manager_prefix, 
	[  --with-replica-manager-prefix=PFX   prefix where 'replica manager' is installed.],
	[], 
	with_replica_manager_prefix=${GLITE_LOCATION:-/opt/glite})

    AC_MSG_CHECKING([for RESOURCE MANAGER installation at ${with_replica_manager_prefix}])

    if test -n "$with_replica_manager_prefix" -a "$with_replica_manager_prefix" != "/usr" ; then
	REPLICA_MANAGER_CFLAGS="-I$with_replica_manager_prefix/include"
        REPLICA_MANAGER_LIBS="-L$with_replica_manager_prefix/lib"
    else
	REPLICA_MANAGER_CFLAGS=""
        REPLICA_MANAGER_LIBS=""
    fi
  
    REPLICA_MANAGER_LIBS="$REPLICA_MANAGER_LIBS -ledg_replica_manager_client_gcc3_2_2"

    AC_ARG_WITH(replica_optimization_prefix,
        [  --with-replica-optimization-prefix=PFX   prefix where 'replica optimization' is installed.],
        [],
        with_replica_optimization_prefix=${GLITE_LOCATION:-/opt/glite})

    AC_MSG_CHECKING([for RESOURCE OPTIMIZATION installation at ${with_replica_optimization_prefix}])

    if test -n "$with_replica_optimization_prefix" -a "$with_replica_optimization_prefix" != "/usr" ; then
        REPLICA_OPTIMIZATION_CFLAGS="-I$with_replica_optimization_prefix/include"
        REPLICA_OPTIMIZATION_LIBS="-L$with_replica_optimization_prefix/lib"
    else
        REPLICA_OPTIMIZATION_CFLAGS=""
        REPLICA_OPTIMIZATION_LIBS=""
    fi

    REPLICA_OPTIMIZATION_LIBS="$REPLICA_OPTIMIZATION_LIBS -ledg_replica_optimization_client_gcc3_2_2"


 -ledg_local_replica_catalog_client_gcc3_2_2 -ledg_replica_metadata_catalog_client_gcc3_2_2 -ledg_replica_optimization_client_gcc3_2_2 -ledg_gsoap_base_gcc3_2_2"
	

    if test x$ac_cv_replica_valid = xyes ; then
	ifelse([$2], , :, [$2])
    else
	REPLICA_MANAGER_CFLAGS=""
	REPLICA_MANAGER_LIBS=""
	REPLICA_OPTIMIZATION_CFLAGS=""
        REPLICA_OPTIMIZATION_LIBS=""
	ifelse([$3], , :, [$3])
    fi

    AC_SUBST(REPLICA_MANAGER_CFLAGS)
    AC_SUBST(REPLICA_MANAGER_LIBS)
    AC_SUBST(REPLICA_OPTIMIZATION_CFLAGS)
    AC_SUBST(REPLICA_OPTIMIZATION_LIBS)
])

