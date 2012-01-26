
SET(QT_USE_QTNETWORK true)
SET(QT_USE_QTGUI false)

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

file(GLOB updater_srcs *.cc)

add_executable(updater ${updater_srcs})
target_link_libraries(updater ${QT_LIBRARIES})
