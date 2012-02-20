

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_DIR})

set(metalib_srcs metalib.cc identity.cc)

add_library(metalib SHARED ${metalib_srcs})
target_link_libraries(metalib elle lune)
set_target_properties(metalib PROPERTIES PREFIX "")

file(COPY __init__.py DESTINATION .)
