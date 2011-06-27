PROJECT(8user CXX)

ADD_EXECUTABLE(8user
  User.cc
)

TARGET_LINK_LIBRARIES(8user infinit elle nucleus lune etoile
		            ${QT_QTNETWORK_LIBRARIES}
			    ${QT_QTCORE_LIBRARIES})
