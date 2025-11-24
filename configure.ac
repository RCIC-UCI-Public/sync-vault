# Copyright (C) 2006-2018 Free Software Foundation, Inc.
# This configure.ac script is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

AC_INIT([sync-vault], [1.0], [ppapadop@uci.edu])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
])
# Checks for libraries.
# FIXME: Replace `main' with a function in `-ltirpc':
AC_CHECK_LIB([tirpc], [svc_run])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h memory.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h syslog.h unistd.h sys/inotify.h sys/stat.h errno.h ])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gethostbyname inet_ntoa memset socket])

# Check for pkg-config
PKG_PROG_PKG_CONFIG

# Check for libtirpc and get its include path
PKG_CHECK_MODULES([LIBTIRPC], [libtirpc], [
    CPPFLAGS="$CPPFLAGS $LIBTIRPC_CFLAGS"
], [
    AC_MSG_ERROR([libtirpc not found])
])

# Now check for rpc.h in the updated include path
AC_CHECK_HEADERS([rpc/rpc.h], [], [
    AC_MSG_ERROR([rpc/rpc.h not found. Please install libtirpc-devel.])
])

AC_OUTPUT
