dnl Usage:
dnl AC_DATA(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for rm, and defines
dnl - DATA_CFLAGS (compiler flags)
dnl - DATA_LIBS (linker flags, stripping and path)
dnl prerequisites:

AC_DEFUN(AC_DATA,
[
    AC_ARG_WITH(data_catalog_storageindex_api_c_prefix, 
	[  --with-data-catalog-storageindex-api-c-prefix=PFX   prefix where 'replica manager' is installed.],
	[], 
	with_data_catalog_storageindex_api_c_prefix=${GLITE_LOCATION:-/opt/glite})

    AC_MSG_CHECKING([for DATA CATALOG STORAGEINDEX API C installation at ${with_data_catalog_storageindex_api_c_prefix}])

    ac_data=yes

    if test -n "$with_data_catalog_storageindex_api_c_prefix" -a "$with_data_catalog_storageindex_api_c_prefix" != "/usr" ; then
	DATA_CFLAGS="-I$with_data_catalog_storageindex_api_c_prefix/include"
        DATA_LIBS="-L$with_data_catalog_storageindex_api_c_prefix/lib"
    else
	DATA_CFLAGS=""
        DATA_LIBS=""
	ac_data=no
    fi
  
    DATA_LIBS="$REPLICA_MANAGER_LIBS libglite_data_catalog_storageindex_api_c.a"

    if test x$ac_data = xyes; then
	ifelse([$2], , :, [$2])
    else
	DATA_CFLAGS=""
	DATA_LIBS=""
	ifelse([$3], , :, [$3])
    fi

    AC_SUBST(DATA_CFLAGS)
    AC_SUBST(DATA_LIBS)
])

