PROJECT(Infinit C CXX)

set(INFINIT_VERSION_MAJOR "0")
set(INFINIT_VERSION_MINOR "1")
set(INFINIT_VERSION "${INFINIT_VERSION_MAJOR}.${INFINIT_VERSION_MINOR}")

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

find_package(Qt4 4.8.0 REQUIRED QtCore QtNetwork)
set(QT_DONT_USE_QTGUI true)
include(${QT_USE_FILE})
include_directories(${QT_INCLUDES})

find_package(OpenSSL REQUIRED)

include_directories(. ${OPENSSL_INCLUDE_DIR})

#
# configurations
#
IF(APPLE)
  SET(INFINIT_MACOSX true)
ELSEIF(WIN32)
  SET(INFINIT_WINDOWS true)
ELSEIF(UNIX)
  SET(INFINIT_LINUX true)
ENDIF()

#
# MSVC CXXFLAGS
#
# XXX is is still relevant?
#
IF(MSVC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
ENDIF(MSVC)

#
# MACOSX FLAGS
#
IF(INFINIT_MACOSX)
  INCLUDE_DIRECTORIES(/usr/local/include /opt/local/include /usr/local/include/osxfuse)
  LINK_DIRECTORIES(/usr/local/lib /opt/local/lib)

  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
ENDIF(INFINIT_MACOSX)

add_subdirectory(comet)
add_subdirectory(elle)
add_subdirectory(nucleus)
add_subdirectory(etoile)
add_subdirectory(agent)
add_subdirectory(lune)
add_subdirectory(horizon)
add_subdirectory(hole)
add_subdirectory(satellites)
add_subdirectory(plasma)

# just to make files appears on qtcreator
file(GLOB_RECURSE HEADERS *.hh *.hxx)

add_library(infinit
  Infinit.cc
  ${HEADERS}
)

ADD_EXECUTABLE(8infinit
  _infinit.cc
)

TARGET_LINK_LIBRARIES(8infinit infinit etoile nucleus lune agent hole horizon)

add_subdirectory(release)

# XXX
#ADD_EXECUTABLE(XXX
#  XXX/Porcupine.cc
#  XXX/XXX.cc
#  XXX/Nest.cc
#)

#TARGET_LINK_LIBRARIES(XXX infinit elle nucleus lune infinit)

# configuration
CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/config.hh.cmake" "${CMAKE_SOURCE_DIR}/config.hh")
