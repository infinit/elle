ADD_EXECUTABLE(test-crypto
  Test.cc
)

TARGET_LINK_LIBRARIES(test-crypto elle ssl crypto dl)
