PROJECT(User CXX)

ADD_EXECUTABLE(8user
  User.cc
)

TARGET_LINK_LIBRARIES(8user elle crypto ssl dl
		            ${QT_QTNETWORK_LIBRARIES}
			    ${QT_QTCORE_LIBRARIES})
