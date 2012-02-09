PROJECT(MACOSX CXX)

INCLUDE(CheckFunctionExists)

CHECK_FUNCTION_EXISTS(setxattr HAVE_SETXATTR)

ADD_DEFINITIONS(-D__FreeBSD__=10 -D_FILE_OFFSET_BITS=64 -D_DARWIN_USE_64_BIT_INODE)

ADD_LIBRARY(macosx
  Crux.cc
  FUSE.cc
  Handle.cc
  InterlacedFUker.cc
  Janitor.cc
  ParallelFUker.cc
  MacOSX.cc
  SequentialFUker.cc
)

TARGET_LINK_LIBRARIES(macosx etoile fuse4x)
