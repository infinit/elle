
set(QT_USE_QTNETWORK true)
set(QT_DONT_USE_QTGUI false)
set(QT_USE_QTXML true)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

file(GLOB watchdog_srcs *.cc)
#file(GLOB watchdog_moc_hdrs MetaClient.hh)

QT4_WRAP_CPP(watchdog_moc_srcs ${watchdog_moc_hdrs})

add_executable(watchdog ${watchdog_moc_srcs} ${watchdog_srcs})

target_link_libraries(watchdog ${QT_LIBRARIES})
