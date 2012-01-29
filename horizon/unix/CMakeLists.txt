PROJECT(UNIX CXX)

IF(APPLE)
  ADD_DEFINITIONS(-D__FreeBSD__=10 -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26)
ELSEIF(UNIX)
  ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)
ENDIF()

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

IF(APPLE)
  TARGET_LINK_LIBRARIES(unix etoile osxfuse)
ELSEIF(UNIX)
  TARGET_LINK_LIBRARIES(unix etoile fuse)
ENDIF()
