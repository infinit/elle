
file(GLOB_RECURSE surface_srcs 
	*.[mhc]
	*.xib
	*.xcodeproj
	*.plist
	*.icns
	*.png
)

add_custom_target(surface
	ALL 
  	WORKING_DIRECTORY
  		${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND
    	xcodebuild -project Surface.xcodeproj -target Infinit SYMROOT=${CMAKE_CURRENT_BINARY_DIR} -configuration Release
    DEPENDS
    	${surface_srcs}
)