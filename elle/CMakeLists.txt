PROJECT(Elle CXX)

QT4_WRAP_CPP(elle_MOCS
  network/Bridge.hh
  network/Door.hh
  network/Gate.hh
  network/Lane.hh
  network/Slot.hh

  concurrency/Timer.hh
  concurrency/Broker.hh
)

ADD_LIBRARY(elle
  core/Large.cc
  core/Null.cc
  core/Natural.cc
  core/Type.cc

  standalone/Log.cc
  standalone/Maid.cc
  standalone/Region.cc
  standalone/Report.cc
  standalone/Standalone.cc

  radix/Meta.cc
  radix/Morgue.cc
  radix/Object.cc
  radix/Radix.cc
  radix/Trace.cc

  io/Console.cc
  io/Directory.cc
  io/Dumpable.cc
  io/File.cc
  io/Link.cc
  io/Manipulators.cc
  io/Pattern.cc
  io/Piece.cc
  io/Path.cc

  package/Archivable.cc
  package/Archive.cc

  system/System.cc

  concurrency/Accord.cc
  concurrency/Broker.cc
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

  cryptography/Cipher.cc
  cryptography/Code.cc
  cryptography/Cryptography.cc
  cryptography/Digest.cc
  cryptography/KeyPair.cc
  cryptography/OneWay.cc
  cryptography/PrivateKey.cc
  cryptography/PublicKey.cc
  cryptography/Random.cc
  cryptography/SecretKey.cc
  cryptography/Seed.cc
  cryptography/Signature.cc

  network/Bridge.cc
  network/Channel.cc
  network/Door.cc
  network/Gate.cc
  network/Header.cc
  network/Host.cc
  network/Lane.cc
  network/Network.cc
  network/Parcel.cc
  network/Point.cc
  network/Session.cc
  network/Slot.cc
  network/Socket.cc

  utility/Base64.cc
  utility/Duration.cc
  utility/Factory.cc
  utility/Hexadecimal.cc
  utility/Parser.cc
  utility/Settings.cc
  utility/Time.cc

  Elle.cc

  ${elle_MOCS}
)

TARGET_LINK_LIBRARIES(elle comet crypto ssl dl readline)

ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(benchmark)
