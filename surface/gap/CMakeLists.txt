find_package(Qt4 4.6.0 REQUIRED QtCore QtNetwork)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})
include_directories(${QT_INCLUDES})

file(GLOB gap_srcs
	gap.cc
	State.cc
	_detail/device.cc
	_detail/file.cc
	_detail/InfinitInstanceManager.cc
	_detail/network.cc
	_detail/notification.cc
	_detail/process.cc
	_detail/swagger.cc
	_detail/transaction.cc
	_detail/user.cc
	MetricReporter.cc
)

add_library(gap SHARED ${gap_srcs})

target_link_libraries(gap LINK_PRIVATE trophol ${INFINIT_LIBRARIES} ${QT_LIBRARIES})
add_subdirectory(python)
