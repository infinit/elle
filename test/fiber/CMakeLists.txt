ADD_EXECUTABLE(test-fiber EXCLUDE_FROM_ALL
  Test.cc
)

TARGET_LINK_LIBRARIES(test-fiber elle
				 ${QT_QTNETWORK_LIBRARIES}
				 ${QT_QTCORE_LIBRARIES})
IF(${WIN32})
  SET(SRCS_WIN32 ../../thirdparty/ucontext-win32.cc)
ENDIF(${WIN32})
ADD_EXECUTABLE(makecontext-test EXCLUDE_FROM_ALL makecontext.cc ${SRCS_WIN32})