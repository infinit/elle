project(Infinit C CXX)

set(INFINIT_VERSION_MAJOR "0")
set(INFINIT_VERSION_MINOR "1")
set(INFINIT_VERSION "${INFINIT_VERSION_MAJOR}.${INFINIT_VERSION_MINOR}")

#
# configurations
#
if(APPLE)
  set(INFINIT_MACOSX true)
elseif(WIN32)
  set(INFINIT_WINDOWS true)
elseiF(UNIX)
  set(INFINIT_LINUX true)
endif()

message("--- building infinit-${INFINIT_VERSION} for ${INFINIT_PLATFORM}")

set(INFINIT_LIBRARIES_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/platforms/${INFINIT_PLATFORM}/work")

set(INFINIT_OPENSSL_STATIC_LIBRARIES
  ${INFINIT_LIBRARIES_PREFIX}/lib/libssl${CMAKE_STATIC_LIBRARY_SUFFIX}
  ${INFINIT_LIBRARIES_PREFIX}/lib/libcrypto${CMAKE_STATIC_LIBRARY_SUFFIX}
)

set(INFINIT_OPENSSL_SHARED_LIBRARIES
  ${INFINIT_LIBRARIES_PREFIX}/lib/libssl${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${INFINIT_LIBRARIES_PREFIX}/lib/libcrypto${CMAKE_SHARED_LIBRARY_SUFFIX}
)

set(INFINIT_MSGPACK_STATIC_LIBRARIES
  ${INFINIT_LIBRARIES_PREFIX}/lib/libmsgpack${CMAKE_STATIC_LIBRARY_SUFFIX}
)

set(INFINIT_MSGPACK_SHARED_LIBRARIES
  ${INFINIT_LIBRARIES_PREFIX}/lib/libmsgpack${CMAKE_SHARED_LIBRARY_SUFFIX}
)

set(INFINIT_JSONCPP_STATIC_LIBRARIES
  ${INFINIT_LIBRARIES_PREFIX}/lib/libjsoncpp${CMAKE_STATIC_LIBRARY_SUFFIX}
)

set(INFINIT_JSONCPP_SHARED_LIBRARIES
  ${INFINIT_LIBRARIES_PREFIX}/lib/libjsoncpp${CMAKE_SHARED_LIBRARY_SUFFIX}
)

set(INFINIT_CURL_STATIC_LIBRARIES
  ${INFINIT_LIBRARIES_PREFIX}/lib/libcurl${CMAKE_STATIC_LIBRARY_SUFFIX}
)

set(INFINIT_CURL_SHARED_LIBRARIES
  ${INFINIT_LIBRARIES_PREFIX}/lib/libcurl${CMAKE_SHARED_LIBRARY_SUFFIX}
)

set(INFINIT_QJSON_SHARED_LIBRARIES
  libqjson${CMAKE_SHARED_LIBRARY_SUFFIX}
)

set(INFINIT_EXTERNAL_LIBRARIES
  ${INFINIT_OPENSSL_STATIC_LIBRARIES}
  ${INFINIT_OPENSSL_SHARED_LIBRARIES}
  ${INFINIT_MSGPACK_STATIC_LIBRARIES}
  ${INFINIT_MSGPACK_SHARED_LIBRARIES}
  ${INFINIT_JSONCPP_STATIC_LIBRARIES}
  ${INFINIT_JSONCPP_SHARED_LIBRARIES}
  ${INFINIT_CURL_STATIC_LIBRARIES}
  ${INFINIT_CURL_SHARED_LIBRARIES}
  ${INFINIT_QJSON_SHARED_LIBRARIES}
)
message("-- External librairies directory: ${INFINIT_LIBRARIES_PREFIX}")
message("-- External libraries: ${INFINIT_EXTERNAL_LIBRARIES}")

##
## Dependencies target.
##
add_custom_target(dependencies ALL)
add_custom_command(
  TARGET
    dependencies
  PRE_BUILD
  COMMAND
    ./build.sh ${INFINIT_PLATFORM}
  WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies
)

##
## Loading boost
##
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS system thread signals filesystem REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
message("-- Boost libraries: ${Boost_LIBRARIES}")




cmake_minimum_required(VERSION 2.6)

# follow symlink when copying
cmake_policy(SET CMP0009 NEW)


find_package(Qt4 4.6.0 REQUIRED QtCore QtNetwork)
set(QT_DONT_USE_QTGUI true)
include(${QT_USE_FILE})
include_directories(${QT_INCLUDES})

# tell cmake to not build shared libraries by default
set(BUILD_SHARED_LIBS OFF)

include_directories(BEFORE SYSTEM ${INFINIT_LIBRARIES_PREFIX}/include .)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/elle/reactor/src)

set(CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} -DQT_NO_KEYWORDS --include ${CMAKE_CURRENT_SOURCE_DIR}/elle/extensions.hh"
)

#
# MSVC CXXFLAGS
#
# XXX is is still relevant?
#
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
endiF(MSVC)

#
# MACOSX FLAGS
#
if(INFINIT_MACOSX)
  include_directories(/usr/local/include /opt/local/include /usr/local/include/osxfuse)
  link_directories(/usr/local/lib /opt/local/lib)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif(INFINIT_MACOSX)

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
add_subdirectory(surface)

if("${INFINIT_PLATFORM}" MATCHES "linux*")
  add_subdirectory(oracle)
endif()

# just to make files appears on qtcreator
file(GLOB_RECURSE HEADERS *.hh *.hxx)

add_definitions(-fPIC)
add_library(infinit
  Infinit.cc
  ${HEADERS}
)

add_executable(8infinit
  _infinit.cc
)
add_dependencies(8infinit dependencies)
add_dependencies(infinit dependencies)
target_link_libraries(8infinit
  infinit elle etoile nucleus lune agent hole horizon ${Boost_LIBRARIES}
)

# XXX
#ADD_EXECUTABLE(XXX
#  XXX/Porcupine.cc
#  XXX/XXX.cc
#  XXX/Handle.cc
#  XXX/Placement.cc
#)
#TARGET_LINK_LIBRARIES(XXX infinit elle etoile nucleus lune infinit)
# XXX

# configuration
configure_file("${CMAKE_SOURCE_DIR}/config.hh.cmake" "${CMAKE_SOURCE_DIR}/config.hh")

# tests
add_subdirectory(tests)
