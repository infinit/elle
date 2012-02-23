

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_DIR})

file(GLOB metalib_srcs *.cc)

add_definitions(-fPIC)
set(CMAKE_EXE_LINKER_FLAGS "-static")
add_library(metalib SHARED ${metalib_srcs})
link_directories(${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(metalib
	elle lune
	${INFINIT_OPENSSL_STATIC_LIBRARIES}
	${INFINIT_MSGPACK_STATIC_LIBRARIES}
)
set_target_properties(metalib PROPERTIES PREFIX "")
#set(needed_libs ${INFINIT_OPENSSL_SHARED_LIBRARIES} ${INFINIT_MSGPACK_SHARED_LIBRARIES})
#foreach(lib ${needed_libs})
#	add_custom_command(
#		TARGET metalib
#		PRE_LINK
#		COMMAND cmake -E copy ${lib} .
#		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#	)
#endforeach()

file(COPY __init__.py DESTINATION .)
