PROJECT(Facade CXX)

ADD_LIBRARY(facade
  Facade.cc
)

IF(${UNIX})
  ADD_SUBDIRECTORY(unix)

  TARGET_LINK_LIBRARIES(facade unix)
ENDIF()

IF(${WIN32})
  ADD_SUBDIRECTORY(windows)

  TARGET_LINK_LIBRARIES(facade windows)
ENDIF()
