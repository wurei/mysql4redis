AC_INIT([libmysql4readis], [1.0.0])

AC_SUBST(PACKAGE_NAME)
AC_SUBST(PACKAGE_VERSION)

AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_INTLTOOL

AC_CONFIG_MACRO_DIR([m4])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_CONFIG_FILES([
libmysql4redis.pc
Makefile
src/Makefile
po/Makefile.in
])

AC_OUTPUT

