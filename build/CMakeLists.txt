CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(json_spirit)

IF(NOT PROJECT_DIR)
  # PROJECT_SOURCE_DIR should be where the CMakeLists.txt is, in
  # build/, so we can generate the top-level project directory from
  # one offset
  SET(PROJECT_DIR ${PROJECT_SOURCE_DIR}/..)
ENDIF()
GET_FILENAME_COMPONENT(PROJECT_DIR ${PROJECT_DIR} ABSOLUTE)

SET(EXEC_PERMS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

#########################
# Settings
#########################
IF(NOT DEFINED JSON_SPIRIT_BUILD_DEMOS)
  SET(JSON_SPIRIT_BUILD_DEMOS TRUE CACHE BOOL "If enabled, builds demos.")
ELSE()
  SET(JSON_SPIRIT_BUILD_DEMOS JSON_SPIRIT_BUILD_DEMOS CACHE BOOL "If enabled, builds demos.")
ENDIF()

IF(NOT DEFINED JSON_SPIRIT_BUILD_TESTS)
  SET(JSON_SPIRIT_BUILD_TESTS TRUE CACHE BOOL "If enabled, builds tests.")
ELSE()
  SET(JSON_SPIRIT_BUILD_TESTS JSON_SPIRIT_BUILD_TESTS CACHE BOOL "If enabled, builds tests.")
ENDIF()

#########################
# Requirements
#########################
IF(NOT Boost_INCLUDE_DIR)
  FIND_PACKAGE(Boost 1.34 REQUIRED)
ENDIF()
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

#########################
# JSON Spirit Library
#########################

SET(JSON_SPIRIT_INCLUDE_DIR ${PROJECT_DIR}/include)
INCLUDE_DIRECTORIES(${JSON_SPIRIT_INCLUDE_DIR})

SET(JSON_SPIRIT_DIR ${PROJECT_DIR}/src)
SET(JSON_SPIRIT_SRCS
  ${JSON_SPIRIT_DIR}/reader.cpp
  ${JSON_SPIRIT_DIR}/value.cpp
  ${JSON_SPIRIT_DIR}/writer.cpp)
ADD_LIBRARY(json_spirit STATIC ${JSON_SPIRIT_SRCS})
INSTALL(
  TARGETS json_spirit
  PERMISSIONS ${EXEC_PERMS}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
INSTALL(
  FILES
  ${JSON_SPIRIT_DIR}/json_spirit.h
  ${JSON_SPIRIT_DIR}/error_position.h
  ${JSON_SPIRIT_DIR}/reader.h
  ${JSON_SPIRIT_DIR}/reader_template.h
  ${JSON_SPIRIT_DIR}/stream_reader.h
  ${JSON_SPIRIT_DIR}/utils.h
  ${JSON_SPIRIT_DIR}/value.h
  ${JSON_SPIRIT_DIR}/writer.h
  ${JSON_SPIRIT_DIR}/writer_template.h
  DESTINATION include)


#########################
# JSON Test
#########################
IF(JSON_SPIRIT_BUILD_TESTS)
  SET(JSON_TEST_DIR ${PROJECT_DIR}/test)
  SET(JSON_TEST_SRCS
    ${JSON_TEST_DIR}/utils_test.cpp
    ${JSON_TEST_DIR}/reader_test.cpp
    ${JSON_TEST_DIR}/stream_reader_test.cpp
    ${JSON_TEST_DIR}/value_test.cpp
    ${JSON_TEST_DIR}/writer_test.cpp
    ${JSON_TEST_DIR}/test.cpp)
  ADD_EXECUTABLE(json_test ${JSON_TEST_SRCS})
  TARGET_LINK_LIBRARIES(json_test json_spirit)
ENDIF()


IF(JSON_SPIRIT_BUILD_DEMOS)
  SET(JSON_DEMO_DIR ${PROJECT_DIR}/demo)

  #########################
  # JSON Spirit Demo
  #########################
  SET(JSON_SIMPLE_DEMO_DIR ${JSON_DEMO_DIR}/simple)
  SET(JSON_SIMPLE_DEMO_SRCS
    ${JSON_SIMPLE_DEMO_DIR}/simple.cpp)
  ADD_EXECUTABLE(json_simple_demo ${JSON_SIMPLE_DEMO_SRCS})
  TARGET_LINK_LIBRARIES(json_simple_demo json_spirit)


  #########################
  # JSON Header-only Demo
  #########################
  SET(JSON_HEADERS_DEMO_DIR ${JSON_DEMO_DIR}/headers)
  SET(JSON_HEADERS_DEMO_SRCS
    ${JSON_HEADERS_DEMO_DIR}/headers.cpp)
  ADD_EXECUTABLE(json_headers_demo ${JSON_HEADERS_DEMO_SRCS})
  TARGET_LINK_LIBRARIES(json_headers_demo json_spirit)

ENDIF() # Demos
