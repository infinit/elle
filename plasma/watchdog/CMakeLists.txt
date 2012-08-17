
set(QT_USE_QTNETWORK true)
set(QT_DONT_USE_QTGUI true)
set(QT_USE_QTXML true)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

file(GLOB watchdog_srcs *.cc)
file(GLOB watchdog_moc_hdrs *.hh)

QT4_WRAP_CPP(watchdog_moc_srcs ${watchdog_moc_hdrs})

add_executable(8watchdog ${watchdog_moc_srcs} ${watchdog_srcs})

target_link_libraries(8watchdog
	nucleus elle lune infinit metal common
	${QT_LIBRARIES}
	${INFINIT_QJSON_SHARED_LIBRARIES}
)
