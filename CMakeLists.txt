PROJECT(Elle CXX)

QT4_WRAP_CPP(elle_MOCS
  network/LocalServer.hh
  network/LocalSocket.hh
  network/TCPServer.hh
  network/TCPSocket.hh
  network/UDPSocket.hh

  concurrency/Timer.hh
  concurrency/Broker.hh

  ${elle_MOCS_WIN32}
)

IF(${WIN32})
  SET(elle_WIN32
    thirdparty/ucontext-win32.cc
    utility/Unicode-win32.cc)
ENDIF()

IF(${UNIX})
  SET(elle_UNIX
    utility/Unicode-unix.cc)
ENDIF()

ADD_LIBRARY(elle
  core/Integer.cc
  core/Large.cc
  core/Null.cc
  core/Natural.cc
  core/Type.cc

  standalone/Log.cc
  standalone/Maid.cc
  standalone/Morgue.cc
  standalone/Region.cc
  standalone/Report.cc
  standalone/Standalone.cc

  radix/Meta.cc
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
  package/Footprint.cc

  system/System.cc

  concurrency/Broker.cc
  concurrency/Concurrency.cc
  concurrency/Environment.cc
  concurrency/Event.cc
  concurrency/Fiber.cc
  concurrency/Frame.cc
  concurrency/Hurdle.cc
  concurrency/Mutex.cc
  concurrency/Program.cc
  concurrency/Routine.cc
  concurrency/Semaphore.cc
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

  network/Header.cc
  network/Host.cc
  network/LocalServer.cc
  network/LocalSocket.cc
  network/Network.cc
  network/Parcel.cc
  network/Locus.cc
  network/Session.cc
  network/Socket.cc
  network/AbstractSocket.cc
  network/TCPServer.cc
  network/TCPSocket.cc
  network/UDPSocket.cc

  utility/Base64.cc
  utility/Duration.cc
  utility/Factory.cc
  utility/Hexadecimal.cc
  utility/HTTP.cc
  utility/JSON.cc
  utility/Parser.cc
  utility/REST.cc
  utility/Settings.cc
  utility/Time.cc

  Elle.cc

  ${elle_MOCS}
  ${elle_WIN32}
  ${elle_UNIX}
)

TARGET_LINK_LIBRARIES(elle comet
  ${QT_QTNETWORK_LIBRARIES}
  ${QT_QTCORE_LIBRARIES}
  msgpack
  crypto
  ssl
  readline
  curl
  json
  json-glib)

IF(${UNIX})
  TARGET_LINK_LIBRARIES(elle dl)
ENDIF()

IF(${WIN32})
  TARGET_LINK_LIBRARIES(elle pdcurses)
ENDIF()

ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(benchmark)
