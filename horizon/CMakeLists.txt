PROJECT(PIG CXX)

ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)

ADD_LIBRARY(pig
  Crux.cc
  FUker.cc
  FUSE.cc
  PIG.cc
  InterlacedFUker.cc
  ParallelFUker.cc
  SequentialFUker.cc

  diary/Diary.cc
  diary/Live.cc
  diary/Record.cc
  diary/Replay.cc
  diary/Upcall.cc
)

TARGET_LINK_LIBRARIES(pig elle nucleus lune etoile fuse)
