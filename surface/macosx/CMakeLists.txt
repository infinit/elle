
file(GLOB_RECURSE surface_srcs 
)
add_custom_target(surface
	ALL 
  	WORKING_DIRECTORY
  		${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND
    	xcodebuild -project Surface.xcodeproj -target Infinit
    SOURCES
    	${surface_srcs}
)