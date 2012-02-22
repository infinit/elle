PROJECT(Infinit C CXX)

set(INFINIT_VERSION_MAJOR "0")
set(INFINIT_VERSION_MINOR "1")
set(INFINIT_VERSION "${INFINIT_VERSION_MAJOR}.${INFINIT_VERSION_MINOR}")

set(INFINIT_LIBRARIES_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/libraries/build")
set(INFINIT_OPENSSL_STATIC_LIBRARIES
	${INFINIT_LIBRARIES_PREFIX}/lib/libssl.a
	${INFINIT_LIBRARIES_PREFIX}/lib/libcrypto.a
)
set(INFINIT_OPENSSL_SHARED_LIBRARIES
	${INFINIT_LIBRARIES_PREFIX}/lib/libssl.so
	${INFINIT_LIBRARIES_PREFIX}/lib/libcrypto.so
)
message("External librairies directory: ${INFINIT_LIBRARIES_DIR}")

cmake_minimum_required(VERSION 2.6)

# follow symlink when copying
cmake_policy(SET CMP0009 NEW)


find_package(Qt4 4.8.0 REQUIRED QtCore QtNetwork)
set(QT_DONT_USE_QTGUI true)
include(${QT_USE_FILE})
include_directories(${QT_INCLUDES})

# tell cmake to not build shared libraries by default
set(BUILD_SHARED_LIBS OFF)

include_directories(BEFORE ${INFINIT_LIBRARIES_PREFIX}/include .)


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
add_subdirectory(oracle)

# just to make files appears on qtcreator
file(GLOB_RECURSE HEADERS *.hh *.hxx)

add_library(infinit
  Infinit.cc
  ${HEADERS}
)

add_executable(8infinit
  _infinit.cc
)

target_link_libraries(8infinit
	infinit elle etoile nucleus lune agent hole horizon
)

# XXX
#ADD_EXECUTABLE(XXX
#  XXX/Porcupine.cc
#  XXX/XXX.cc
#  XXX/Nest.cc
#)

#TARGET_LINK_LIBRARIES(XXX infinit elle nucleus lune infinit)

# configuration
configure_file("${CMAKE_SOURCE_DIR}/config.hh.cmake" "${CMAKE_SOURCE_DIR}/config.hh")
