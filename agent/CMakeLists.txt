project(Agent CXX)

add_definitions(-fPIC)

add_library(agent
  Agent.cc
)

target_link_libraries(agent nucleus lune)
