PROJECT(Elle CXX)

QT4_WRAP_CPP(elle_MOCS
  network/Slot.hh
)

ADD_LIBRARY(elle
  archive/Archive.cc

  core/Large.cc
  core/Null.cc
  core/Void.cc

  crypto/Cipher.cc
  crypto/Code.cc
  crypto/Cryptography.cc
  crypto/Digest.cc
  crypto/KeyPair.cc
  crypto/OneWay.cc
  crypto/PrivateKey.cc
  crypto/PublicKey.cc
  crypto/SecretKey.cc
  crypto/Signature.cc

  factory/Factory.cc

  io/IO.cc

  misc/Region.cc
  misc/Report.cc
  misc/Callback.cc

  system/System.cc

  network/Network.cc
  network/Host.cc
  network/Address.cc
  network/Environment.cc
  network/Socket.cc
  network/Slot.cc
  network/Bridge.cc
  network/Door.cc

  ${elle_MOCS}
)

ADD_EXECUTABLE(test-archive
  test/archive/Generator.cc
  test/archive/Pack.cc
  test/archive/Referee.cc
  test/archive/Test.cc
)

ADD_EXECUTABLE(test-crypto
  test/crypto/Test.cc
)

QT4_WRAP_CPP(test-network_MOCS
  test/network/Node.hh
)

ADD_EXECUTABLE(test-network
  test/network/Test.cc
  test/network/Probe.cc
  test/network/Node.cc

  ${test-network_MOCS}
)

ADD_EXECUTABLE(benchmark-crypto
  benchmark/crypto/Benchmark.cc
  benchmark/crypto/Time.cc
)

TARGET_LINK_LIBRARIES(test-archive elle ssl crypto dl)
TARGET_LINK_LIBRARIES(test-crypto elle ssl crypto dl)
TARGET_LINK_LIBRARIES(test-network elle
                       ${QT_QTNETWORK_LIBRARIES}
                       ${QT_QTCORE_LIBRARIES})

TARGET_LINK_LIBRARIES(benchmark-crypto elle ssl crypto dl)
