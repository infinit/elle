PROJECT(Linux CXX)

INCLUDE(CheckFunctionExists)

CHECK_FUNCTION_EXISTS(setxattr HAVE_SETXATTR)

ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)

ADD_LIBRARY(linux
  Crib.cc
  Crux.cc
  Event.cc
  FUker.cc
  FUSE.cc
  Handle.cc
  Janitor.cc
  Linux.cc

  ${linux_MOCS}
)

TARGET_LINK_LIBRARIES(linux etoile fuse pthread ${Boost_LIBRARIES})
