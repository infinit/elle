PROJECT(8passport CXX)

ADD_EXECUTABLE(8passport
  Passport.cc
)

TARGET_LINK_LIBRARIES(8passport elle lune etoile crypto ssl dl
		                ${QT_QTNETWORK_LIBRARIES}
			        ${QT_QTCORE_LIBRARIES})
