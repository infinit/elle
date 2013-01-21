add_library(protocol
  Channel.cc
  ChanneledStream.cc
  Packet.cc
  RPC.cc
  Serializer.cc
  Stream.cc
)

project(8rpc CXX)

add_executable(8rpc
  8rpc.cc
)

target_link_libraries(8rpc
	${INFINIT_LIBRARIES}
)
