dnl Usage:
dnl AC_REPLICA(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for rm, and defines
dnl - REPLICA_CFLAGS (compiler flags)
dnl - REPLICA_LIBS (linker flags, stripping and path)
dnl prerequisites:

AC_DEFUN(AC_RM,
[
    AC_ARG_WITH(replica_prefix, 
	[  --with-replica-prefix=PFX   prefix where 'replica' is installed.],
	[], 
	with_replica_prefix=${GLITE_LOCATION:-/opt/glite})

    AC_MSG_CHECKING([for RESOURCE MANAGER installation at ${with_replica_prefix}])
    
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    ac_save_cppflags=$CPPFLAGS
    ac_save_LIBS=$LIBS
    if test -n "$with_replica_prefix" -a "$with_replica_prefix" != "/usr" ; then
	REPLICA_CFLAGS="-I$with_replica_prefix/include"
	REPLICA_LIBS="-L$with_replica_prefix/lib"
    else
	REPLICA_CFLAGS=""
	REPLICA_LIBS=""
    fi

    REPLICA_LIBS="$REPLICA_LIBS -ledg_replica_manager_client_gcc3_2_2 -ledg_local_replica_catalog_client_gcc3_2_2 -ledg_replica_metadata_catalog_client_gcc3_2_2 -ledg_replica_optimization_client_gcc3_2_2 -ledg_gsoap_base_gcc3_2_2"
	
    CPPFLAGS="$REPLICA_CFLAGS $CPPFLAGS"
    LIBS="$REPLICA_LIBS $LIBS"
    ac_cv_replica_valid=yes
    AC_TRY_COMPILE([ #include "EdgReplicaManager/ReplicaManagerImpl.h" 
    		     #include "EdgReplicaOptimization/ReplicaManagerException.h" ],
    		   [ ] 
    		   [ ac_cv_replica_valid=yes ], [ ac_cv_replica_valid=no ])

    CPPFLAGS=$ac_save_cppflags
    LIBS=$ac_save_LIBS	
    AC_LANG_RESTORE
    AC_MSG_RESULT([$ac_cv_replica_valid])

    if test x$ac_cv_replica_valid = xyes ; then
	ifelse([$2], , :, [$2])
	REPLICA_INSTALL_PATH=$with_replica_prefix
    else
	REPLICA_CFLAGS=""
	REPLICA_LIBS=""
        REPLICA_INSTALL_PATH=""
	ifelse([$3], , :, [$3])
    fi

    AC_SUBST(REPLICA_CFLAGS)
    AC_SUBST(REPLICA_LIBS)
    AC_SUBST(REPLICA_INSTALL_PATH)
])

