PROJECT(Elle CXX)

QT4_WRAP_CPP(elle_MOCS
  network/Slot.hh
  network/Bridge.hh
  network/Door.hh
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
  network/Message.cc

  ${elle_MOCS}
)

#ADD_SUBDIRECTORY(test)
#ADD_SUBDIRECTORY(benchmark)

# XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

ADD_EXECUTABLE(XXX
  XXX.cc
)

TARGET_LINK_LIBRARIES(XXX elle ssl crypto dl
                      ${QT_QTNETWORK_LIBRARIES}
                      ${QT_QTCORE_LIBRARIES})
