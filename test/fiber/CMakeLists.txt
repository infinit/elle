ADD_EXECUTABLE(test-fiber Test.cc)

TARGET_LINK_LIBRARIES(test-fiber elle
				 ${QT_QTNETWORK_LIBRARIES}
				 ${QT_QTCORE_LIBRARIES})

IF(${WIN32})
  SET(SRCS_WIN32 ../../thirdparty/ucontext-win32.cc)
ENDIF(${WIN32})
ADD_EXECUTABLE(makecontext-test EXCLUDE_FROM_ALL makecontext.cc ${SRCS_WIN32})
ADD_EXECUTABLE(uc_link-test EXCLUDE_FROM_ALL uc_link.cc ${SRCS_WIN32})

QT4_WRAP_CPP(x_MOCS
  QtObj.hh
)

ADD_EXECUTABLE(fiber-qt-simple EXCLUDE_FROM_ALL qt-simple.cc ${x_MOCS})
TARGET_LINK_LIBRARIES(fiber-qt-simple elle ${QT_QTNETWORK_LIBRARIES} ${QT_QTCORE_LIBRARIES})
