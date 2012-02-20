
file(GLOB installer_srcs *.cc)
file(GLOB installer_ui *.ui)
file(GLOB installer_moc_hdrs MainWindow.hh)

set(QT_USE_QTNETWORK true)
set(QT_DONT_USE_QTGUI false)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

QT4_WRAP_CPP(installer_moc_srcs ${installer_moc_hdrs})

QT4_WRAP_UI(installer_ui_hdrs ${installer_ui})

add_executable(installer ${installer_srcs} ${installer_moc_srcs} ${installer_ui_hdrs})
target_link_libraries(installer ${QT_LIBRARIES})
