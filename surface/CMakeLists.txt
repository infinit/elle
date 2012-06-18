add_subdirectory(gap)

if(INFINIT_MACOSX)
	add_subdirectory(macosx)
	message("Add surface/macosx")
endif(INFINIT_MACOSX)

