
SET(PythonLibs_FIND_VERSION 2)

find_package(PythonLibs)

message(STATUS "Python2 include directory: ${PYTHON_INCLUDE_DIR}")
include_directories(${PYTHON_INCLUDE_DIR})

file(GLOB metalib_srcs *.cc)

add_library(_metalib SHARED ${metalib_srcs})
link_directories(${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(_metalib
	PROPERTIES
		OUTPUT_NAME "_metalib"
		PREFIX ""
		SUFFIX ".so"
)

target_link_libraries(_metalib
	${INFINIT_LIBRARIES}
	${PYTHON_LIBRARIES}
)

file(COPY __init__.py DESTINATION .)
