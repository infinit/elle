add_executable(test-cryptography EXCLUDE_FROM_ALL Test.cc)

target_link_libraries(test-cryptography
	elle comet
	${QT_QTNETWORK_LIBRARIES}
	${QT_QTCORE_LIBRARIES}
	${Boost_LIBRARIES}
	pthread
)
