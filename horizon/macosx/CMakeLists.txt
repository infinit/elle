PROJECT(MACOSX CXX)

ADD_DEFINITIONS(-D__FreeBSD__=10 -D_FILE_OFFSET_BITS=64)

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
