PROJECT(Agent CXX)

ADD_LIBRARY(agent
  Agent.cc
)

TARGET_LINK_LIBRARIES(agent nucleus lune)
