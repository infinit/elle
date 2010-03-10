ADD_EXECUTABLE(benchmark-crypto
  Benchmark.cc
  Time.cc
)

TARGET_LINK_LIBRARIES(benchmark-crypto elle ssl crypto dl
				       ${QT_QTNETWORK_LIBRARIES}
				       ${QT_QTCORE_LIBRARIES})
