ADD_EXECUTABLE(test-archive
  Generator.cc
  Pack.cc
  Referee.cc
  Test.cc
)

TARGET_LINK_LIBRARIES(test-archive elle ssl crypto dl)
