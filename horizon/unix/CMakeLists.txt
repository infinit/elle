PROJECT(UNIX CXX)

ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)

ADD_LIBRARY(unix
  Crux.cc
  FUSE.cc
  Handle.cc
  InterlacedFUker.cc
  Janitor.cc
  ParallelFUker.cc
  UNIX.cc
  SequentialFUker.cc
)

TARGET_LINK_LIBRARIES(unix etoile fuse)
