add_subdirectory(elle) # Elle unit test are safe from INFINIT_LIBRARIES
# XXX add_subdirectory(lune)
add_subdirectory(surface)

link_libraries(${INFINIT_LIBRARIES})
add_subdirectory(nucleus)
add_subdirectory(plasma)
add_subdirectory(protocol)
