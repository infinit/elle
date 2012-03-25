PROJECT(MACOSX CXX)

INCLUDE(CheckFunctionExists)

CHECK_FUNCTION_EXISTS(setxattr HAVE_SETXATTR)

ADD_DEFINITIONS(-D__FreeBSD__=10 -D_FILE_OFFSET_BITS=64 -D_DARWIN_USE_64_BIT_INODE)

QT4_WRAP_CPP(macosx_MOCS
  Broker.hh
)

ADD_LIBRARY(macosx
  Broker.cc
  Crib.cc
  Crux.cc
  FUker.cc
  FUSE.cc
  Handle.cc
  Janitor.cc
  MacOSX.cc

  ${macosx_MOCS}
)

# XXX[is pthread necessary?]
TARGET_LINK_LIBRARIES(macosx etoile fuse4x)
