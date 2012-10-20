# Copyright (c) Glyn Matthews 2012.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


set(Uri_SRCS uri.cpp schemes.cpp normalize.cpp)
add_library(cppnetlib-uri ${Uri_SRCS})
foreach (src_file ${Uri_SRCS})
  if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
    set_source_files_properties(${src_file}
      PROPERTIES COMPILE_FLAGS ${CPP-NETLIB_CXXFLAGS})
  endif()
endforeach(src_file)
