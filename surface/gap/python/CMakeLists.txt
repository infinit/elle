find_package(Python3Libs)

#find_package(Boost COMPONENTS python3 REQUIRED)
find_package(Boost COMPONENTS python3)

if (NOT Boost_PYTHON3_LIBRARY)
	#XXX on gentoo boost python package has complete python version suffix
	find_package(Boost COMPONENTS python-3.2)
endif()

message(STATUS "Python3 libraries: ${PYTHON3_LIBRARIES}")
message(STATUS "Python3 include directory: ${PYTHON3_INCLUDE_DIR}")
message(STATUS "Boost python library: ${Boost_PYTHON3_LIBRARY}")

include_directories("${PYTHON3_INCLUDE_DIR}")

add_definitions(-fPIC -fno-strict-aliasing)
file(GLOB gap_srcs ../*.cc)
list(APPEND gap_srcs pygap.cc)
add_library(_gap SHARED ${gap_srcs})
link_directories(${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(_gap
    elle common lune hole metal nucleus infinit
    ${INFINIT_OPENSSL_SHARED_LIBRARIES}
	${Boost_LIBRARIES}
	${PYTHON3_LIBRARIES}
)
set_target_properties(_gap PROPERTIES PREFIX "")

if(UNIX)
	set_target_properties(_gap PROPERTIES SUFFIX ".so")
endif()

file(COPY gap.py DESTINATION .)
