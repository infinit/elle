

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_DIR})

file(GLOB metalib_srcs *.cc)

add_definitions(-fPIC)
add_library(metalib SHARED ${metalib_srcs})
target_link_libraries(metalib
	elle lune
)
set_target_properties(metalib PROPERTIES PREFIX "")

file(COPY __init__.py DESTINATION .)
