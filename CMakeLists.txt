PROJECT(Infinit C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(Qt4 4.5 REQUIRED QtCore QtNetwork)

INCLUDE_DIRECTORIES(. ${QT_INCLUDES})

MESSAGE("UNIX: ${UNIX}")
MESSAGE("WIN32: ${WIN32}")
MESSAGE("CMAKE_SYSTEM: ${CMAKE_SYSTEM}")
MESSAGE("CMAKE_HOST_SYSTEM: ${CMAKE_HOST_SYSTEM}")

#
# for C++0x
#
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
IF(${UNIX})
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -std=gnu++0x -Wcast-align -Wold-style-cast")
ENDIF()

#
# for FUSE
#
ADD_DEFINITIONS(-D_LARGE_FILE -D_FILE_OFFSET_BITS=64 -DWINVER=0x0600)

ADD_SUBDIRECTORY(comet)
ADD_SUBDIRECTORY(elle)
ADD_SUBDIRECTORY(nucleus)
ADD_SUBDIRECTORY(etoile)
ADD_SUBDIRECTORY(agent)
ADD_SUBDIRECTORY(lune)

IF(${UNIX})
  ADD_SUBDIRECTORY(pig)
ENDIF()

IF(${WIN32})
  ADD_SUBDIRECTORY(iig)
ENDIF()

ADD_SUBDIRECTORY(hole)
ADD_SUBDIRECTORY(applications)

# just to make files appears on qtcreator
FILE(GLOB_RECURSE HEADERS *.hh *.hxx)

ADD_LIBRARY(infinit
  Infinit.cc
  ${HEADERS}
)

ADD_EXECUTABLE(Infinit
  _infinit.cc
)

TARGET_LINK_LIBRARIES(Infinit infinit elle nucleus lune agent etoile hole)

IF(${UNIX})
  TARGET_LINK_LIBRARIES(Infinit pig)
ENDIF()

IF(${WIN32})
  TARGET_LINK_LIBRARIES(Infinit iig)
ENDIF()

TARGET_LINK_LIBRARIES(Infinit ${QT_QTCORE_LIBRARIES}
                              ${QT_QTNETWORK_LIBRARIES}
                              ${QT_QTCORE_LIBRARIES})

# XXX
ADD_EXECUTABLE(XXX
  XXX/XXX.cc
)

TARGET_LINK_LIBRARIES(XXX infinit elle nucleus lune infinit
                              ${QT_QTCORE_LIBRARIES}
                              ${QT_QTNETWORK_LIBRARIES}
                              ${QT_QTCORE_LIBRARIES})
