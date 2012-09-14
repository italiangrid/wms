dnl Usage:
dnl AC_LIBCLASSAD(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl - CLASSAD_CPPFLAGS (compiler flags)
dnl - CLASSAD_LIBS (linker flags, stripping and path)

AC_DEFUN([AC_LIBCLASSAD],
[
    AC_ARG_WITH(classads_prefix, 
	[  --with-classads-prefix=PFX    prefix where libclassad is installed (optional)],
	[],
	with_classads_prefix="/opt/classads"
    )

    if test "$host_cpu" = "x86_64" -o "$host_cpu" = "ia64" ; then
        ac_classads_lib_dir="lib64"
    else
        ac_classads_lib_dir="lib"
    fi
    AC_MSG_CHECKING([for libclassad])

    if test -n "$with_classads_prefix" -a "$with_classads_prefix" != "/usr" ; then
        ac_classads_prefix=$with_classads_prefix
        CLASSAD_CPPFLAGS="-I$with_classads_prefix/include -DWANT_NAMESPACES"
        CLASSAD_LIBS="-L$with_classads_prefix/$ac_classads_lib_dir -lclassad"
    else
        ac_classads_prefix="/usr"
        CLASSAD_LIBS="-lclassad"
    fi

    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS
    ac_save_cppflags=$CPPFLAGS
    ac_save_libs=$LIBS
    CPPFLAGS="$CLASSAD_CPPFLAGS $CPPFLAGS"
    LIBS="$CLASSAD_LIBS $LIBS"
    AC_LINK_IFELSE([
#include <classad_distribution.h>
#include <string>

int main()
{
  std::string const s("[key=\"value\"]");
  classad::ClassAd ad;
  classad::ClassAdParser parser;
  parser.ParseClassAd(s, ad);
}
    ], [ac_have_classads=yes], [ac_have_classads=no])

    CPPFLAGS=$ac_save_cppflags
    LIBS=$ac_save_libs
    AC_LANG_RESTORE

    if test "$ac_have_classads" = "yes"; then
        ifelse([$2], , [echo "yes (in $ac_classads_prefix)"], [$2])
    else
        ifelse([$3], , [echo no], [$3])
    fi
    AC_SUBST(CLASSAD_CPPFLAGS)
    AC_SUBST(CLASSAD_LIBS)
])

