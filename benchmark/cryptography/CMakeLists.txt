ADD_EXECUTABLE(benchmark-crypto
  Benchmark.cc
  Time.cc
)

TARGET_LINK_LIBRARIES(benchmark-crypto elle ssl crypto dl)
