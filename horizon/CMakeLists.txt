PROJECT(pig CXX)

ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)

ADD_EXECUTABLE(pig
  main.cc

  system/fuse/System.cc
  system/fuse/Interface.cc

  cache/Cache.cc

  util/Path.cc
  util/File.cc)

TARGET_LINK_LIBRARIES(pig pthread rt fuse elle etoile)
