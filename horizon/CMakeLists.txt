PROJECT(pig CXX)

ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)

ADD_EXECUTABLE(pig
  main.cc
  PIG.cc
)

TARGET_LINK_LIBRARIES(pig pthread rt fuse elle ssl crypto)
