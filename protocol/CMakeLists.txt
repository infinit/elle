add_library(protocol
  Packet.cc
  PacketStream.cc
)

add_definitions(-fPIC)

target_link_libraries(protocol)
