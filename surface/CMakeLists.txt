add_subdirectory(gap)
add_subdirectory(cli)

if(INFINIT_MACOSX)
	add_subdirectory(macosx)
endif(INFINIT_MACOSX)


#if(INFINIT_LINUX)
#	add_subdirectory(linux)
#endif()
