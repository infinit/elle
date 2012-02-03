PROJECT(Linux CXX)

ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)

ADD_LIBRARY(linux
  Crux.cc
  FUSE.cc
  Handle.cc
  InterlacedFUker.cc
  Janitor.cc
  ParallelFUker.cc
  Linux.cc
  SequentialFUker.cc
)

TARGET_LINK_LIBRARIES(linux etoile fuse)
