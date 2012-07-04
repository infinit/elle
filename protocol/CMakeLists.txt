add_library(protocol
  Channel.cc
  ChanneledStream.cc
  Packet.cc
  RPC.cc
  Serializer.cc
  Stream.cc
)

add_definitions(-fPIC)

target_link_libraries(protocol)
