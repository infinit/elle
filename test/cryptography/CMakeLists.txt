ADD_EXECUTABLE(test-cryptography
  Test.cc
)

TARGET_LINK_LIBRARIES(test-cryptography elle ssl crypto dl)
