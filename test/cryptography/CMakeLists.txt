add_executable(test-cryptography EXCLUDE_FROM_ALL Test.cc)

<<<<<<< HEAD
TARGET_LINK_LIBRARIES(test-cryptography comet elle
					${QT_QTNETWORK_LIBRARIES}
					${QT_QTCORE_LIBRARIES}
                                        ${Boost_LIBRARIES})
=======
target_link_libraries(test-cryptography
	elle comet
	${QT_QTNETWORK_LIBRARIES}
	${QT_QTCORE_LIBRARIES}
	${Boost_LIBRARIES}
	pthread
)
>>>>>>> hotgloupi/serialize
