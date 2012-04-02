PROJECT(MACOSX CXX)

INCLUDE(CheckFunctionExists)

CHECK_FUNCTION_EXISTS(setxattr HAVE_SETXATTR)

ADD_DEFINITIONS(-D__FreeBSD__=10 -D_FILE_OFFSET_BITS=64 -D_DARWIN_USE_64_BIT_INODE)

ADD_LIBRARY(macosx
  Crib.cc
  Crux.cc
  Event.cc
  FUker.cc
  FUSE.cc
  Handle.cc
  Janitor.cc
  MacOSX.cc

  ${macosx_MOCS}
)

TARGET_LINK_LIBRARIES(macosx etoile fuse4x pthread)
