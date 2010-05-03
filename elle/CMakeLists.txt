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
  core/Natural.cc

  standalone/Maid.cc
  standalone/Region.cc
  standalone/Report.cc
  standalone/Standalone.cc

  radix/Meta.cc
  radix/Radix.cc
  radix/Trace.cc

  io/Dumpable.cc

  archive/Archive.cc
  archive/Archivable.cc

  system/System.cc

  concurrency/Accord.cc
  concurrency/Concurrency.cc
  concurrency/Environment.cc
  concurrency/Event.cc
  concurrency/Fiber.cc
  concurrency/Frame.cc
  concurrency/Mutex.cc
  concurrency/Program.cc
  concurrency/Routine.cc
  concurrency/Semaphore.cc
  concurrency/Thread.cc
  concurrency/Timer.cc

  factory/Factory.cc

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

  network/Address.cc
  network/Channel.cc
  network/Door.cc
  network/Header.cc
  network/Host.cc
  network/Lane.cc
  network/Network.cc
  network/Parcel.cc
  network/Session.cc
  network/Slot.cc
  network/Socket.cc

  util/Base64.cc
  util/Hexadecimal.cc
  util/Parser.cc
  util/Settings.cc
  util/Time.cc

  Elle.cc

  ${elle_MOCS}
)

ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(benchmark)
