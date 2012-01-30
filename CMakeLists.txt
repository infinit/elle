PROJECT(Infinit C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(Qt4 REQUIRED QtCore QtNetwork)
FIND_PACKAGE(OpenSSL REQUIRED)

INCLUDE_DIRECTORIES(. ${QT_INCLUDES} ${OPENSSL_INCLUDE_DIR})

#
# MSVC CXXFLAGS
#
IF(MSVC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
ENDIF(MSVC)

#
# MACOSX FLAGS
#
IF(APPLE)
  INCLUDE_DIRECTORIES(/usr/local/include /opt/local/include /usr/local/include/osxfuse)
  LINK_DIRECTORIES(/usr/local/lib /opt/local/lib)

  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
ENDIF()

ADD_SUBDIRECTORY(comet)
ADD_SUBDIRECTORY(elle)
ADD_SUBDIRECTORY(nucleus)
ADD_SUBDIRECTORY(etoile)
ADD_SUBDIRECTORY(agent)
ADD_SUBDIRECTORY(lune)
ADD_SUBDIRECTORY(facade)

ADD_SUBDIRECTORY(hole)
ADD_SUBDIRECTORY(applications)
ADD_SUBDIRECTORY(plasma)

# just to make files appears on qtcreator
FILE(GLOB_RECURSE HEADERS *.hh *.hxx)

ADD_LIBRARY(infinit
  Infinit.cc
  ${HEADERS}
)

ADD_EXECUTABLE(8infinit
  _infinit.cc
)

TARGET_LINK_LIBRARIES(8infinit infinit etoile nucleus lune agent hole facade)

# XXX
#ADD_EXECUTABLE(XXX
#  XXX/Porcupine.cc
#  XXX/XXX.cc
#  XXX/Nest.cc
#)

#TARGET_LINK_LIBRARIES(XXX infinit elle nucleus lune infinit
#                              ${QT_QTNETWORK_LIBRARIES}
#                              ${QT_QTCORE_LIBRARIES})
