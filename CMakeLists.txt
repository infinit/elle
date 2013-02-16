cmake_minimum_required(VERSION 2.8)

# ---------- options ----------------------------------------------------------
set(HAS_XCODE TRUE CACHE BOOL
    "Disable HAS_XCODE if your machine does not have it")

option(ENABLE_TEST_COMPILATION "Enable or not compilation of subdir tests" ON)
option(DISABLE_HORIZON "Enable fuse system mount points" OFF)
option(USE_STATIC_LIBS "Use static version of libraries" OFF)
option(USE_STATIC_RUNTIME "Link statically with libgcc and libg++" OFF)
option(PRODUCTION_BUILD "Make build production ready" OFF)

# follow symlink when copying
cmake_policy(SET CMP0009 NEW)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake/Modules")

project(Infinit C CXX)

# - Set target directories ----------------------------------------------------
#
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


# - Infinit version -----------------------------------------------------------
#
# Defines INFINIT_VERSION
#

set(INFINIT_VERSION_MAJOR "0")
set(INFINIT_VERSION_MINOR "1")
set(INFINIT_VERSION "${INFINIT_VERSION_MAJOR}.${INFINIT_VERSION_MINOR}")

add_definitions(-DINFINIT_VERSION_MAJOR=${INFINIT_VERSION_MAJOR})
add_definitions(-DINFINIT_VERSION_MINOR=${INFINIT_VERSION_MINOR})
add_definitions(-DINFINIT_VERSION=${INFINIT_VERSION})

if(PRODUCTION_BUILD)
	add_definitions(-DINFINIT_PRODUCTION_BUILD=1)
	message(STATUS "PRODUCTION build")
else()
	message(STATUS "DEVELOPMENT build")
endif()

# - Infinit platform ----------------------------------------------------------
#
# Defines INFINIT_PLATFORM
#

if(APPLE)
  set(INFINIT_MACOSX TRUE)
  set(INFINIT_PLATFORM_NO_ARCH macosx)
  add_definitions(-DHAVE_STATFS)
  add_definitions(-DINFINIT_MACOSX)
elseif(WIN32)
  set(INFINIT_WINDOWS TRUE)
  set(INFINIT_PLATFORM_NO_ARCH windows)
  add_definitions(-DINFINIT_WINDOWS)
elseif(UNIX)
  set(INFINIT_LINUX TRUE)
  set(INFINIT_PLATFORM_NO_ARCH linux)
  add_definitions(-DINFINIT_LINUX)
endif()

add_definitions(-DHAVE_SETXATTR)

# - Infinit architecture ------------------------------------------------------
#
# Defines INFINIT_ARCHITECTURE
#

if(CMAKE_SIZEOF_VOID_P MATCHES "8")
    set(INFINIT_ARCHITECTURE "64")
else()
    set(INFINIT_ARCHITECTURE "32")
endif()

set(INFINIT_PLATFORM "${INFINIT_PLATFORM_NO_ARCH}${INFINIT_ARCHITECTURE}")
add_definitions(-DBOOST_PP_VARIADICS)


# - Components configuration --------------------------------------------------

message("--- building infinit-${INFINIT_VERSION} for ${INFINIT_PLATFORM}")

# - Build type ----------------------------------------------------------------
#
# Handle cmake build type changes and set BUILD_TYPE_CHANGED when appropriate.
#

if(NOT DISABLE_HORIZON)
   add_definitions(-DINFINIT_HORIZON)
endif()

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE DEBUG CACHE STRING "Build type" FORCE)
	set(BUILD_TYPE DEBUG)
endif()

if(BUILD_TYPE)
	string(TOUPPER "${BUILD_TYPE}" BUILD_TYPE)
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)

if(BUILD_TYPE AND NOT(BUILD_TYPE STREQUAL CMAKE_BUILD_TYPE))
	set(CMAKE_BUILD_TYPE ${BUILD_TYPE} CACHE BOOL "Build type" FORCE)
	message(STATUS "Force build type: ${CMAKE_BUILD_TYPE}")
	set(BUILD_TYPE_CHANGED TRUE)
else()
	set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
	set(BUILD_TYPE_CHANGED FALSE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# - Compiler ------------------------------------------------------------------
message(STATUS "Compiler is: ${CMAKE_CXX_COMPILER}")
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

# - Compilation flags ---------------------------------------------------------

set(CUSTOM_CXX_FLAGS)
set(CUSTOM_C_FLAGS)
if (UNIX)
    list(APPEND CUSTOM_CXX_FLAGS
        -D_LARGE_FILE
        -D_FILE_OFFSET_BITS=64
        -m${INFINIT_ARCHITECTURE}
        -std=c++11
        -Wall
        -Wextra
        -Wpointer-arith
        -Wcast-align
        -Wno-overloaded-virtual
        -Wno-empty-body  # for ELLE_LOG_TRACE macro
        -Wno-non-template-friend
        -DQT_NO_KEYWORDS
        -fPIC
    )
    set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -DDEBUG -UNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-g3 -UDEBUG -DNDEBUG")

    #set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc")
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc")
endif()

if(INFINIT_MACOSX)
  include_directories(/usr/local/include /opt/local/include)
  link_directories(/usr/local/lib /opt/local/lib)
  list(APPEND CUSTOM_CXX_FLAGS "-Wno-deprecated-declarations")
endif(INFINIT_MACOSX)

if (USE_STATIC_RUNTIME)
	message(STATUS "Using static runtime linking")
	list(APPEND CUSTOM_CXX_FLAGS
		"-static-libgcc"
		#"-static-libstdc++"
	)
endif()

foreach(flag ${CUSTOM_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
endforeach()

message(STATUS "CXX flags: ${CMAKE_CXX_FLAGS}")

# - Libraries linking method --------------------------------------------------
#
# Defines USE_STATIS_LIBS to TRUE if build type is release.
# Defines LINK_METHOD to STATIC if build type is release, SHARED otherwise.
#

if (USE_STATIC_LIBS)
	set(CMAKE_FIND_LIBRARY_SUFFIXES
		"${CMAKE_STATIC_LIBRARY_SUFFIX};${CMAKE_SHARED_LIBRARY_SUFFIX}"
	)
endif()

#if(${CMAKE_BUILD_TYPE} MATCHES "[rR]elease|RELEASE")
#	set(USE_STATIC_LIBS ON)
	set(LINK_METHOD STATIC)
#else()
#	set(USE_STATIC_LIBS OFF)# CACHE BOOL "use static libraries for linking")
	set(LINK_METHOD STATIC)
#endif()

# tell cmake to not build shared libraries by default
set(BUILD_SHARED_LIBS OFF)

# Workaround to remove '-rdynamic' flag
# fix scheduled for 2.8.1
# http://www.itk.org/Bug/view.php?id=9985
# http://www.itk.org/Bug/view.php?id=5833
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")


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

# - Dependencies --------------------------------------------------------------

## Loading Basic systems dependencies

### Threads
find_package(Threads REQUIRED)

## Loading boost
set(Boost_USE_STATIC_LIBS ${USE_STATIC_LIBS})
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ${USE_STATIC_RUNTIME})
find_package(Boost
	COMPONENTS system thread signals filesystem unit_test_framework
	REQUIRED
)

include_directories(${Boost_INCLUDE_DIRS})
message(STATUS "Boost libraries:")
foreach(lib ${Boost_LIBRARIES})
	message("\t - ${lib}")
endforeach()
## Loading libz
unset(INFINIT_Z_LIBRARY)
find_library(INFINIT_Z_LIBRARY z PATHS /opt/local/lib NO_DEFAULT_PATH)
message(STATUS "Zip library: ${INFINIT_Z_LIBRARY}")

## Loading Qt
find_package(Qt4 4.6.0 REQUIRED QtCore)

#set(QT_DONT_USE_QTNETWORK true)
#set(QT_DONT_USE_QTGUI true)
#set(QT_DONT_USE_QTXML true)
include(${QT_USE_FILE})

include_directories(${QT_INCLUDES})
add_definitions(${QT_DEFINITIONS})

message(STATUS "Qt Libraries:")
foreach(lib ${QT_LIBRARIES})
	message("\t - ${lib}")
endforeach()

include_directories(BEFORE SYSTEM ${INFINIT_LIBRARIES_PREFIX}/include .)

# - Infinit internal dependencies ---------------------------------------------

set(INFINIT_LIBRARIES_PREFIX
	"${CMAKE_CURRENT_SOURCE_DIR}/dependencies/platforms/${INFINIT_PLATFORM}/work")

message("-- External librairies directory prefix: ${INFINIT_LIBRARIES_PREFIX}")
include_directories(BEFORE SYSTEM ${INFINIT_LIBRARIES_PREFIX}/include)

set(INFINIT_OPENSSL_STATIC_LIBRARIES
  ${INFINIT_LIBRARIES_PREFIX}/lib/libssl${CMAKE_STATIC_LIBRARY_SUFFIX}
  ${INFINIT_LIBRARIES_PREFIX}/lib/libcrypto${CMAKE_STATIC_LIBRARY_SUFFIX}
)

set(INFINIT_OPENSSL_SHARED_LIBRARIES
  ${INFINIT_LIBRARIES_PREFIX}/lib/libssl${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${INFINIT_LIBRARIES_PREFIX}/lib/libcrypto${CMAKE_SHARED_LIBRARY_SUFFIX}
)

set(INFINIT_EXTERNAL_LIBRARIES
  ${INFINIT_OPENSSL_STATIC_LIBRARIES}
  ${INFINIT_OPENSSL_SHARED_LIBRARIES}
)

if(USE_STATIC_LIBS)
	set(INFINIT_OPENSSL_LIBRARIES
		${INFINIT_OPENSSL_STATIC_LIBRARIES}
		${INFINIT_Z_LIBRARY}
	)
else()
	set(INFINIT_OPENSSL_LIBRARIES ${INFINIT_OPENSSL_SHARED_LIBRARIES})
endif()

message("-- OpenSSL libraries:")
foreach(lib  ${INFINIT_OPENSSL_LIBRARIES})
	message("\t - ${lib}")
endforeach()

# XXX[unfortunate we cannot put these in their own CMakeLists.txt file]
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/elle/reactor/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cryptography/sources)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/asio-udt/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/asio-udt/udt/include)

add_subdirectory(comet)
add_subdirectory(elle)
add_subdirectory(common)
add_subdirectory(cryptography)
add_subdirectory(nucleus)
add_subdirectory(etoile)
add_subdirectory(agent)
add_subdirectory(lune)
add_subdirectory(asio-udt)

if(NOT DISABLE_HORIZON)
  add_subdirectory(horizon)
  # circular dependency with ${INFINIT_PLATFORM_NO_ARCH} lib
  set(HORIZON_LIB horizon ${INFINIT_PLATFORM_NO_ARCH} horizon fuse)
endif()
add_subdirectory(hole)

add_library(infinit ${LINK_METHOD}
  Infinit.cc
  HoleFactory.cc
  ${HEADERS}
)
add_dependencies(infinit hole dependencies)


## ----------------------------------------------------------------------------
# The whole project is linked against these libraries

set(INFINIT_LIBRARIES
  ${HORIZON_LIB}
  etoile
  protocol
  hole
  metal
  agent
  lune
  infinit # XXX(descriptor which depends on Infinit)
  lune # XXX(infinit depends on lune)
  hole # XXX(descriptor depends on model)
  nucleus
  cryptography
  common
  elle
  cryptography # XXX(authority depends on crypto)
  comet
  asio-udt
  ${INFINIT_OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)

message(STATUS "Infinit libraries:")
foreach(lib ${INFINIT_LIBRARIES})
	message("\t - ${lib}")
endforeach()

## Executables ----------------------------------------------------------------

add_subdirectory(satellites)
add_subdirectory(protocol)
add_subdirectory(plasma)
if (ENABLE_TEST_COMPILATION)
   add_subdirectory(tests)
endif()
add_subdirectory(surface)
add_subdirectory(oracle)

add_executable(8infinit
  _infinit.cc
)
target_link_libraries(8infinit LINK_PRIVATE
	${INFINIT_LIBRARIES}
)
add_dependencies(8infinit dependencies)
