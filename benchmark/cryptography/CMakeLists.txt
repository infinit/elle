add_executable(benchmark-crypto EXCLUDE_FROM_ALL
  Benchmark.cc
  Time.cc
)

target_link_libraries(benchmark-crypto
  elle
  ${QT_QTNETWORK_LIBRARIES}
  ${QT_QTCORE_LIBRARIES}
  ${OPENSSL_LIBRARIES}
)
