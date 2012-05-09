
file(GLOB surface_srcs 
	*
)
add_custom_command(
  	TARGET
    	surface
	POST_BUILD
	COMMAND
    	xcodebuild -project Surface.xcodeproj -alltargets
)