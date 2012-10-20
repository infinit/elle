# Copyright (c) Glyn Matthews 2012.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


cmake_minimum_required(VERSION 2.8)
project(Uri)

include_directories(${Uri_SOURCE_DIR}/include)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package( Boost 1.51.0 REQUIRED unit_test_framework system regex filesystem )

set(CMAKE_VERBOSE_MAKEFILE true)

if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
  INCLUDE(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG(-std=c++0x HAVE_STD0X)
  CHECK_CXX_COMPILER_FLAG(-std=c++11 HAVE_STD11)

  if (HAVE_STD11)
    set(CMAKE_CXX_FLAGS -std=c++11)
  elseif (HAVE_STD0X)
    set(CMAKE_CXX_FLAGS -std=c++0x)
  else()
    message(FATAL_ERROR "No advanced standard C++ support (-std=c++0x and -std=c++11 not defined).")
  endif()
  message("C++ Flags: ${CMAKE_CXX_FLAGS} link flags: ${CMAKE_CXX_LINK_FLAGS}")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
  set(CMAKE_CXX_LINK_FLAGS "-std=c++11 -stdlib=libc++")
  message("C++ Flags: ${CMAKE_CXX_FLAGS} link flags: ${CMAKE_CXX_LINK_FLAGS}")
endif()

if (Boost_FOUND)
  if (MSVC)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  endif(MSVC)
  if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0501)
  endif(WIN32)
  include_directories(${Boost_INCLUDE_DIRS})
  enable_testing()

  add_subdirectory(src)
  add_subdirectory(test)

endif(Boost_FOUND)
