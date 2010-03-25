PROJECT(Elle CXX)

QT4_WRAP_CPP(elle_MOCS
  network/Slot.hh
  network/Lane.hh
  network/Door.hh

  concurrency/Timer.hh
)

ADD_LIBRARY(elle
  core/Large.cc
  core/Null.cc
  core/Entity.cc

  io/Dumpable.cc

  misc/Region.cc
  misc/Maid.cc
  misc/Report.cc
  misc/Routine.cc

  archive/Archive.cc
  archive/Archivable.cc

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

  system/System.cc

  network/Network.cc
  network/Host.cc
  network/Address.cc
  network/Socket.cc
  network/Slot.cc
  network/Header.cc
  network/Session.cc
  network/Parcel.cc
  network/Door.cc
  network/Lane.cc
  network/Channel.cc

  concurrency/Concurrency.cc
  concurrency/Accord.cc
  concurrency/Mutex.cc
  concurrency/Event.cc
  concurrency/Semaphore.cc
  concurrency/Thread.cc
  concurrency/Application.cc
  concurrency/Timer.cc
  concurrency/Frame.cc
  concurrency/Fiber.cc

  Elle.cc

  ${elle_MOCS}
)

ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(benchmark)
