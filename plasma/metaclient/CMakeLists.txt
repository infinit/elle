
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

file(GLOB metaclient_srcs *.cc)
file(GLOB metaclient_moc_hdrs MetaClient.hh)
QT4_WRAP_CPP(metaclient_moc_srcs ${metaclient_moc_hdrs})

add_library(metaclient ${metaclient_srcs} ${metaclient_moc_srcs})
target_link_libraries(metaclient
        ${INFINIT_QJSON_SHARED_LIBRARIES}
	${QT_QTNETWORK_LIBRARIES}
	${QT_QTCORE_LIBRARIES}
)
