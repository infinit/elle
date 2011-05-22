PROJECT(Hole CXX)

ADD_EXECUTABLE(Hole
  local/Local.cc

  remote/Remote.cc

  Hole.cc
  Holeable.cc

  hole.cc
)

TARGET_LINK_LIBRARIES(Hole elle nucleus lune
		           ${QT_QTCORE_LIBRARIES}
		           ${QT_QTNETWORK_LIBRARIES}
			   ${QT_QTKERNEL_LIBRARIES})
