if(INFINIT_MACOSX)
  # SDK Info
  set(DEVROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer")
  set(SDKROOT "${DEVROOT}/SDKs/MacOSX10.7.sdk")
  set(CMAKE_OSX_SYSROOT "${SDKROOT}")
  # set(CMAKE_OSX_ARCHITECTURES "x86_64")
  set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
endif(INFINIT_MACOSX)

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

add_dependencies(surface gap)
