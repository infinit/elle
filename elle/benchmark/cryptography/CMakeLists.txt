ADD_EXECUTABLE(benchmark-crypto
  Benchmark.cc
  Time.cc
)

TARGET_LINK_LIBRARIES(benchmark-crypto elle
				       ${QT_QTNETWORK_LIBRARIES}
				       ${QT_QTCORE_LIBRARIES})
