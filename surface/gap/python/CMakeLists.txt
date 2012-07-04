find_package(Python3Libs)
find_package(Boost COMPONENTS python3 REQUIRED)

#include_directories("/usr/include/python3.2mu")
#set(PYTHON3_LIBRARIES "python3.2mu")

message(STATUS "Python3 libraries: ${PYTHON3_LIBRARIES}")
message(STATUS "Python3 include directory: ${PYTHON3_INCLUDE_DIR}")

include_directories("${PYTHON3_INCLUDE_DIR}")

add_definitions(-fPIC)
file(GLOB gap_srcs ../*.cc)
list(APPEND gap_srcs pygap.cc)
add_library(_gap SHARED ${gap_srcs})
link_directories(${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(_gap
    elle lune hole metal nucleus infinit
    ${INFINIT_OPENSSL_SHARED_LIBRARIES}
	${Boost_LIBRARIES}
	${PYTHON3_LIBRARIES}
)
set_target_properties(_gap PROPERTIES PREFIX "")

file(COPY gap.py DESTINATION .)
