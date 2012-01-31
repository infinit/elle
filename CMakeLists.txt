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

message("INFINIT_VERSION: ${INFINIT_VERSION}")
message("UNIX: ${UNIX}")
message("WIN32: ${WIN32}")
message("CMAKE_SYSTEM: ${CMAKE_SYSTEM}")
message("CMAKE_HOST_SYSTEM: ${CMAKE_HOST_SYSTEM}")

#
# MSVC CXXFLAGS
#
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
endif()

#
# MACOSX FLAGS
#
if(APPLE)
  include_directories(/usr/local/include /opt/local/include /usr/local/include/osxfuse)
  link_directories(/usr/local/lib /opt/local/lib)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

add_subdirectory(comet)
add_subdirectory(elle)
add_subdirectory(nucleus)
add_subdirectory(etoile)
add_subdirectory(agent)
add_subdirectory(lune)
add_subdirectory(facade)

add_subdirectory(hole)
add_subdirectory(applications)
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

TARGET_LINK_LIBRARIES(8infinit infinit etoile nucleus lune agent hole facade)

add_subdirectory(release)
