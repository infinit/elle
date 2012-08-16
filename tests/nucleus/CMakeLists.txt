link_libraries(
	elle lune hole nucleus infinit
	pthread
    ${Boost_LIBRARIES}
)
add_subdirectory(proton)
add_subdirectory(neutron)
