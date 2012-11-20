add_subdirectory(elle) # Elle unit test are safe from INFINIT_LIBRARIES

link_libraries(${INFINIT_LIBRARIES})
add_subdirectory(nucleus)
add_subdirectory(surface)
add_subdirectory(protocol)
