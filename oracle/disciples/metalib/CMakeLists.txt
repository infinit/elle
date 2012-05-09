
find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_DIR})

file(GLOB metalib_srcs *.cc)

add_definitions(-fPIC)
add_library(metalib SHARED ${metalib_srcs})
link_directories(${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(metalib
    elle lune hole nucleus infinit
	libboost_system${CMAKE_SHARED_LIBRARY_SUFFIX}
	libboost_signals${CMAKE_SHARED_LIBRARY_SUFFIX}
    ${INFINIT_OPENSSL_SHARED_LIBRARIES}
	${INFINIT_MSGPACK_STATIC_LIBRARIES}
)
set_target_properties(metalib PROPERTIES PREFIX "")

file(COPY __init__.py DESTINATION .)
