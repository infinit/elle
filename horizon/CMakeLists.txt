PROJECT(PIG CXX)

ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)

ADD_LIBRARY(pig
  Crux.cc
  FUSE.cc
  Handle.cc
  InterlacedFUker.cc
  Janitor.cc
  ParallelFUker.cc
  PIG.cc
  SequentialFUker.cc
)

TARGET_LINK_LIBRARIES(pig etoile fuse)
