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

  cryptography/Cipher.cc
  cryptography/Code.cc
  cryptography/Cryptography.cc
  cryptography/Digest.cc
  cryptography/KeyPair.cc
  cryptography/OneWay.cc
  cryptography/PrivateKey.cc
  cryptography/PublicKey.cc
  cryptography/SecretKey.cc
  cryptography/Signature.cc

  factory/Factory.cc

  misc/Report.cc
  misc/Region.cc
  misc/Callback.cc

  system/System.cc

  network/Network.cc
  network/Host.cc
  network/Address.cc
  network/Socket.cc
  network/Slot.cc
  network/Bridge.cc
  network/Door.cc
  network/Header.cc
  network/Identifier.cc
  network/Context.cc

  concurrency/Accord.cc
  concurrency/Mutex.cc
  concurrency/Semaphore.cc
  concurrency/Thread.cc
  concurrency/Event.cc

  Elle.cc

  ${elle_MOCS}
)

ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(benchmark)
