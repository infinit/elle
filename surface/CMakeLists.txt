add_subdirectory(gap)
add_subdirectory(cli)

if(INFINIT_MACOSX)
  if(${HAS_XCODE})
	add_subdirectory(macosx)
  endif()
endif(INFINIT_MACOSX)
