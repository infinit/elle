
SET(QT_USE_QTNETWORK true)
set(QT_DONT_USE_QTGUI false)
set(QT_USE_QTXML true)

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

file(GLOB updater_srcs *.cc)
file(GLOB updater_moc_hdrs Application.hh)
QT4_WRAP_CPP(updater_moc_srcs ${updater_moc_hdrs})

add_executable(updater ${updater_srcs} ${updater_moc_srcs})
target_link_libraries(updater ${QT_LIBRARIES})
