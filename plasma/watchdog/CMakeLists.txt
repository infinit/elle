
find_package(Qt4 4.6.0 REQUIRED QtCore QtNetwork)
include(${QT_USE_FILE})
include_directories(${QT_INCLUDES})
add_definitions(${QT_DEFINITIONS})

file(GLOB watchdog_srcs *.cc)
file(GLOB watchdog_moc_hdrs *.hh)

QT4_WRAP_CPP(watchdog_moc_srcs ${watchdog_moc_hdrs})

add_executable(8watchdog ${watchdog_moc_srcs} ${watchdog_srcs})

target_link_libraries(8watchdog
  ${INFINIT_LIBRARIES}
  ${QT_LIBRARIES}
)
