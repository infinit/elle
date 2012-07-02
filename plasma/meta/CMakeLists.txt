
add_definitions(-fPIC)

add_library(metal Client.cc)
target_link_libraries(metal elle)
