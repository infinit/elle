# Copyright (c) Glyn Matthews 2012.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


set(
  TESTS
  uri_test
  uri_builder_test
  uri_encoding_test
  scheme_tests
  )
foreach (test ${TESTS})
  add_executable(${test} ${test}.cpp)
  add_dependencies(${test} cppnetlib-uri)
  target_link_libraries(${test}
    ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} cppnetlib-uri)
  if (OPENSSL_FOUND)
    target_link_libraries(${test} ${OPENSSL_LIBRARIES})
  endif()
  set_target_properties(${test}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${Uri_BINARY_DIR}/tests)
  add_test(${test}
    ${Uri_BINARY_DIR}/tests/${test})
endforeach (test)
