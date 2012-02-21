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

INCLUDE_DIRECTORIES(${PROJECT_DIR}/json_spirit)


#########################
# Requirements
#########################
FIND_PACKAGE(Boost 1.34 REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

#########################
# JSON Spirit Library
#########################
SET(JSON_SPIRIT_DIR ${PROJECT_DIR}/json_spirit)
SET(JSON_SPIRIT_SRCS
  ${JSON_SPIRIT_DIR}/json_spirit_reader.cpp
  ${JSON_SPIRIT_DIR}/json_spirit_value.cpp
  ${JSON_SPIRIT_DIR}/json_spirit_writer.cpp)
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
  ${JSON_SPIRIT_DIR}/json_spirit_error_position.h
  ${JSON_SPIRIT_DIR}/json_spirit_reader.h
  ${JSON_SPIRIT_DIR}/json_spirit_reader_template.h
  ${JSON_SPIRIT_DIR}/json_spirit_stream_reader.h
  ${JSON_SPIRIT_DIR}/json_spirit_utils.h
  ${JSON_SPIRIT_DIR}/json_spirit_value.h
  ${JSON_SPIRIT_DIR}/json_spirit_writer.h
  ${JSON_SPIRIT_DIR}/json_spirit_writer_template.h
  DESTINATION include)


#########################
# JSON Test
#########################
SET(JSON_TEST_DIR ${PROJECT_DIR}/json_test)
SET(JSON_TEST_SRCS
  ${JSON_TEST_DIR}/utils_test.cpp
  ${JSON_TEST_DIR}/json_spirit_reader_test.cpp
  ${JSON_TEST_DIR}/json_spirit_stream_reader_test.cpp
  ${JSON_TEST_DIR}/json_spirit_utils_test.cpp
  ${JSON_TEST_DIR}/json_spirit_value_test.cpp
  ${JSON_TEST_DIR}/json_spirit_writer_test.cpp
  ${JSON_TEST_DIR}/json_test.cpp)
ADD_EXECUTABLE(json_test ${JSON_TEST_SRCS})
TARGET_LINK_LIBRARIES(json_test json_spirit)


#########################
# JSON Spirit Demo
#########################
SET(JSON_DEMO_DIR ${PROJECT_DIR}/json_demo)
SET(JSON_DEMO_SRCS
  ${JSON_DEMO_DIR}/json_demo.cpp)
ADD_EXECUTABLE(json_demo ${JSON_DEMO_SRCS})
TARGET_LINK_LIBRARIES(json_demo json_spirit)


#########################
# JSON Header-only Demo
#########################
SET(JSON_HEADERS_ONLY_DEMO_DIR ${PROJECT_DIR}/json_headers_only_demo)
SET(JSON_HEADERS_ONLY_DEMO_SRCS
    ${JSON_HEADERS_ONLY_DEMO_DIR}/json_headers_only_demo.cpp)
ADD_EXECUTABLE(json_headers_only_demo ${JSON_HEADERS_ONLY_DEMO_SRCS})
TARGET_LINK_LIBRARIES(json_headers_only_demo json_spirit)


#########################
# JSON Map Demo
#########################
SET(JSON_MAP_DEMO_DIR ${PROJECT_DIR}/json_map_demo)
SET(JSON_MAP_DEMO_SRCS
  ${JSON_MAP_DEMO_DIR}/json_map_demo.cpp)
ADD_EXECUTABLE(json_map_demo ${JSON_MAP_DEMO_SRCS})
TARGET_LINK_LIBRARIES(json_map_demo json_spirit)


INCLUDE(CPack)
