PROJECT(Elle CXX)

QT4_WRAP_CPP(elle_MOCS
  network/LocalServer.hh
  network/LocalSocket.hh
  network/TCPServer.hh
  network/TCPSocket.hh
  network/UDPSocket.hh

  concurrency/Timer.hh
  concurrency/Broker.hh
)

IF(INFINIT_LINUX)
  SET(elle_PLATFORM
    thirdparty/epth-linux.cc
    utility/Unicode-unix.cc)
ELSEIF(INFINIT_MACOSX)
  SET(elle_PLATFORM
    thirdparty/epth-macosx.cc
    utility/Unicode-unix.cc)
ELSEIF(INFINIT_WINDOWS)
  SET(elle_PLATFORM
    thirdparty/epth-windows.cc
    utility/Unicode-win32.cc)
ENDIF()

set_property (SOURCE reactor/src/reactor/backend/coro_io/libcoroutine/asm.S PROPERTY LANGUAGE CXX)

ADD_LIBRARY(elle
	#core/Integer.cc
	#core/Large.cc
	#core/Null.cc
	#core/Natural.cc
	#core/Type.cc

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

  #package/Archivable.cc
  #package/Archive.cc
  #package/Footprint.cc

  system/System.cc

  concurrency/Broker.cc
  concurrency/Concurrency.cc
  concurrency/Environment.cc
  concurrency/Event.cc
  concurrency/Frame.cc
  concurrency/Program.cc
  concurrency/Routine.cc
  concurrency/Scheduler.cc
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

  utility/Duration.cc
  utility/Factory.cc
  utility/HTTP.cc
  utility/Parser.cc
  utility/REST.cc
  utility/Settings.cc
  utility/Time.cc

  format/ini/Section.cc
  format/ini/File.cc

  format/json/Null.cc
  format/json/Bool.cc
  format/json/Float.cc
  format/json/Parser.cc
  format/json/Dictionary.cc
  format/json/Array.cc
  format/json/Object.cc
  format/json/Integer.cc
  format/json/String.cc

  serialize/ArchiveMode.cc
  serialize/Base64Archive.cc
  serialize/JSONArchive.cc
  serialize/IniArchive.cc

  Elle.cc

  reactor/src/reactor/backend/coro_io/libcoroutine/asm.S
  reactor/src/reactor/backend/coro_io/libcoroutine/context.cc
  reactor/src/reactor/backend/coro_io/libcoroutine/coroutine.cc
  reactor/src/reactor/backend/coro_io/thread.cc
  reactor/src/reactor/backend/pthread/pthread.cc
  reactor/src/reactor/debug.cc
  reactor/src/reactor/exception.cc
  reactor/src/reactor/lockable.cc
  reactor/src/reactor/mutex.cc
  reactor/src/reactor/network/exception.cc
  reactor/src/reactor/network/resolve.cc
  reactor/src/reactor/network/server.cc
  reactor/src/reactor/network/socket.cc
  reactor/src/reactor/network/tcp-server.cc
  reactor/src/reactor/network/tcp-socket.cc
  reactor/src/reactor/network/udp-server-socket.cc
  reactor/src/reactor/network/udp-server.cc
  reactor/src/reactor/network/udp-socket.cc
  reactor/src/reactor/operation.cc
  reactor/src/reactor/rw-mutex.cc
  reactor/src/reactor/scheduler.cc
  reactor/src/reactor/semaphore.cc
  reactor/src/reactor/signal.cc
  reactor/src/reactor/sleep.cc
  reactor/src/reactor/thread.cc
  reactor/src/reactor/waitable.cc

  ${elle_MOCS}
  ${elle_PLATFORM}
)

add_definitions(-fPIC)
target_link_libraries(elle
	${INFINIT_OPENSSL_STATIC_LIBRARIES}
	${INFINIT_MSGPACK_STATIC_LIBRARIES}
	${INFINIT_JSONCPP_STATIC_LIBRARIES}
	${INFINIT_CURL_STATIC_LIBRARIES}
	comet
	${QT_QTNETWORK_LIBRARIES}
	${QT_QTCORE_LIBRARIES}
	pth
	z
	readline
)
add_dependencies(elle dependencies)

IF(INFINIT_LINUX)
  TARGET_LINK_LIBRARIES(elle dl)
ELSEIF(INFINIT_MACOSX)
  TARGET_LINK_LIBRARIES(elle dl)
ELSEIF(INFINIT_WINDOWS)
  TARGET_LINK_LIBRARIES(elle pdcurses)
ENDIF()

ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(benchmark)
