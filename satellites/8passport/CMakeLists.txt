PROJECT(8passport CXX)

ADD_EXECUTABLE(8passport
  Passport.cc
)

TARGET_LINK_LIBRARIES(8passport infinit elle nucleus lune etoile hole
		                ${QT_QTNETWORK_LIBRARIES}
			        ${QT_QTCORE_LIBRARIES})
