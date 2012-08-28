cmake_minimum_required(VERSION 2.8)

# ---------- options ----------------------------------------------------------
set(HAS_XCODE TRUE CACHE BOOL
    "Disable HAS_XCODE if your machine does not have it")

# follow symlink when copying
cmake_policy(SET CMP0009 NEW)

project(Infinit C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake/Modules")

set(INFINIT_VERSION_MAJOR "0")
set(INFINIT_VERSION_MINOR "1")
set(INFINIT_VERSION "${INFINIT_VERSION_MAJOR}.${INFINIT_VERSION_MINOR}")
set(CMAKE_BUILD_TYPE Debug)


# Platform ####################################################################
if(APPLE)
  set(INFINIT_MACOSX TRUE)
  set(INFINIT_PLATFORM macosx)
elseif(WIN32)
  set(INFINIT_WINDOWS TRUE)
  set(INFINIT_PLATFORM windows)
elseif(UNIX)
  set(INFINIT_LINUX TRUE)
  set(INFINIT_PLATFORM linux)
endif()

# Architecture ################################################################
if(CMAKE_SIZEOF_VOID_P MATCHES "8")
    set(INFINIT_ARCHITECTURE "64")
else()
    set(INFINIT_ARCHITECTURE "32")
endif()

set(INFINIT_PLATFORM "${INFINIT_PLATFORM}${INFINIT_ARCHITECTURE}")

# Compiler ####################################################################

# XXX[problem with distcc, fuck you very much CMake]
#if(CMAKE_COMPILER_IS_GNUCXX)
#	exec_program("${CMAKE_CXX_COMPILER}"
#		ARGS --version OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION
#	)
#
#	if(NOT(CMAKE_CXX_COMPILER_VERSION MATCHES ".*4\\.7.*"))
#		message(FATAL_ERROR "gcc version has to be 4.7 or greater")
#	endif()
#endif()

# Compilation flags ###########################################################
set(CUSTOM_CXX_FLAGS)
set(CUSTOM_C_FLAGS)
if (UNIX)
    list(APPEND CUSTOM_CXX_FLAGS
        -D_LARGE_FILE
        -D_FILE_OFFSET_BITS=64
        -m${INFINIT_ARCHITECTURE}
        -std=c++0x
        -Wall
        -Wextra
        -Wpointer-arith
        -Wcast-align
        -Wno-overloaded-virtual
        -Wno-empty-body  # for ELLE_LOG_TRACE macro
        -DQT_NO_KEYWORDS
    )
    set(CMAKE_CXX_FLAGS_DEBUG "-g3 -DDEBUG -UNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-g3 -UDEBUG -DNDEBUG")

    #set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc")
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc")
endif()

foreach(flag ${CUSTOM_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
endforeach()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CXX flags: ${CMAKE_CXX_FLAGS}")

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
  include_directories(/usr/local/include /opt/local/include)
  link_directories(/usr/local/lib /opt/local/lib)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif(INFINIT_MACOSX)

# Workaround to remove '-rdynamic' flag
# fix scheduled for 2.8.1
# http://www.itk.org/Bug/view.php?id=9985
# http://www.itk.org/Bug/view.php?id=5833
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

message("--- building infinit-${INFINIT_VERSION} for ${INFINIT_PLATFORM}")

set(INFINIT_LIBRARIES_PREFIX
	"${CMAKE_CURRENT_SOURCE_DIR}/dependencies/platforms/${INFINIT_PLATFORM}/work")

set(INFINIT_OPENSSL_STATIC_LIBRARIES
  ${INFINIT_LIBRARIES_PREFIX}/lib/libssl${CMAKE_STATIC_LIBRARY_SUFFIX}
  ${INFINIT_LIBRARIES_PREFIX}/lib/libcrypto${CMAKE_STATIC_LIBRARY_SUFFIX}
)

set(INFINIT_OPENSSL_SHARED_LIBRARIES
  ${INFINIT_LIBRARIES_PREFIX}/lib/libssl${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${INFINIT_LIBRARIES_PREFIX}/lib/libcrypto${CMAKE_SHARED_LIBRARY_SUFFIX}
)

set(INFINIT_QJSON_SHARED_LIBRARIES
  libqjson${CMAKE_SHARED_LIBRARY_SUFFIX}
)


set(INFINIT_EXTERNAL_LIBRARIES
  ${INFINIT_OPENSSL_STATIC_LIBRARIES}
  ${INFINIT_OPENSSL_SHARED_LIBRARIES}
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
find_package(Boost COMPONENTS system thread signals filesystem unit_test_framework REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
message("-- Boost libraries: ${Boost_LIBRARIES}")



find_package(Qt4 4.6.0 REQUIRED QtCore QtNetwork)
set(QT_DONT_USE_QTGUI true)
include(${QT_USE_FILE})
include_directories(${QT_INCLUDES})

# tell cmake to not build shared libraries by default
set(BUILD_SHARED_LIBS OFF)

include_directories(BEFORE SYSTEM ${INFINIT_LIBRARIES_PREFIX}/include .)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/elle/reactor/src)


add_subdirectory(comet)
add_subdirectory(elle)
add_subdirectory(common)
add_subdirectory(nucleus)
add_subdirectory(protocol)
add_subdirectory(etoile)
add_subdirectory(agent)
add_subdirectory(lune)
add_subdirectory(horizon)
add_subdirectory(hole)
add_subdirectory(satellites)
add_subdirectory(plasma)

# XXX[temporarily here so as to bypass surface's compilation failure]
add_subdirectory(tests)

add_subdirectory(surface)

#if("${INFINIT_PLATFORM}" MATCHES "linux*")
add_subdirectory(oracle)
#endif()

# just to make files appears on qtcreator
file(GLOB_RECURSE HEADERS *.hh *.hxx)

add_definitions(-fPIC)
add_library(infinit
  Infinit.cc
  ${HEADERS}
)
add_dependencies(infinit dependencies)

add_executable(8infinit
  _infinit.cc
)
add_dependencies(8infinit dependencies)
target_link_libraries(8infinit
  infinit
  ${Boost_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  elle etoile nucleus lune agent  hole metal horizon nucleus protocol
)

# configuration
configure_file("${CMAKE_SOURCE_DIR}/config.hh.cmake" "${CMAKE_SOURCE_DIR}/config.hh")
