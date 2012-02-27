
set(QT_USE_QTNETWORK true)
set(QT_USE_QTXML true)
set(QT_DONT_USE_QTGUI false)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

file(GLOB updater_srcs *.cc)
file(GLOB updater_ui *.ui)
set(updater_moc_hdrs
  Application.hh
  ReleaseUpdater.hh
  LoginDialog.hh
  IdentityUpdater.hh
)

QT4_WRAP_CPP(updater_moc_srcs ${updater_moc_hdrs})
QT4_WRAP_UI(updater_ui_hdrs ${updater_ui})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(updater ${updater_srcs} ${updater_moc_srcs} ${updater_ui_hdrs})
target_link_libraries(updater
	metaclient
	${QT_LIBRARIES}
	${INFINIT_QJSON_SHARED_LIBRARIES}
)
