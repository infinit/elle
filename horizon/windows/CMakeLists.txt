PROJECT(Windows CXX)

# XXX are all these necessary?
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_REENTRANT -D_THREAD_SAFE -D_UNICODE -DUNICODE -D_WIN32_IE=0x0300")

SET(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -static-libgcc -static-libstdc++ -mwindows -municode")

ADD_LIBRARY(windows
  Windows.cc
)

TARGET_LINK_LIBRARIES(windows cbfs setupapi version netapi32 ole32 comctl32)
