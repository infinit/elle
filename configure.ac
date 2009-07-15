#
# ---------- header -----------------------------------------------------------
#
# project       infinite
#
# license       GPL
#
# file          /home/mycure/infinit/configure.ac
#
# created       julien quintard   [tue nov 13 21:10:52 2007]
# updated       julien quintard   [thu mar 20 10:39:50 2008]
#

# prerequisite
AC_PREREQ(2.61)

# initialise
AC_INIT([The Infinit File System],[0.0.1],[],[infinit])

# auxiliary directory
AC_CONFIG_AUX_DIR(tools)

# automake initilisation
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# copyright
AC_COPYRIGHT([GPL])

# basic check
AC_CONFIG_SRCDIR([elle/misc/Status.hh])

# type checks
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# c++ compiler
AC_LANG([C++])
AC_PROG_CXX

# programs
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_CPP

# typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

# endianness detection
AC_C_BIGENDIAN

# library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC

# makefiles generation
AC_CONFIG_FILES([
  Makefile
  elle/Makefile
  infinit/Makefile
])

# c++ flags
CXXFLAGS="								\
  -pipe									\
  -Wall -W -Wcast-align -Wformat -Wwrite-strings			\
  -fexceptions								\
"

# cpp flags
# XXX[remove $HOME stuff]
CPPFLAGS="								\
  -I. -I$HOME/local-ssldev/include/ -I$HOME/local/include		\
"

# ld flags
# XXX[remove $HOME stuff]
LDFLAGS="								\
      -L$HOME/local-ssldev/lib/ -L$HOME/local/lib			\
"

# output
AC_OUTPUT
