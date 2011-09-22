ADD_EXECUTABLE(test-fiber EXCLUDE_FROM_ALL
  Test.cc
)

TARGET_LINK_LIBRARIES(test-fiber elle
				 ${QT_QTNETWORK_LIBRARIES}
				 ${QT_QTCORE_LIBRARIES})
ADD_EXECUTABLE(makecontext-test makecontext.cc ../../thirdparty/ucontext-win32.cc)