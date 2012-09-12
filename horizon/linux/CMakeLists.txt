include(CheckFunctionExists)

check_function_exists(setxattr HAVE_SETXATTR)

add_definitions(-D_FILE_OFFSET_BITS=64)

file(GLOB linux_srcs *.cc)

add_library(linux ${linux_srcs})

