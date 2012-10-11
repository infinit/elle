PROJECT(Elle CXX)

IF(INFINIT_LINUX)
  SET(elle_PLATFORM
    utility/Unicode-unix.cc)
ELSEIF(INFINIT_MACOSX)
  SET(elle_PLATFORM
    utility/Unicode-unix.cc)
ELSEIF(INFINIT_WINDOWS)
  SET(elle_PLATFORM
    utility/Unicode-win32.cc)
ENDIF()

set_property(
    SOURCE reactor/src/reactor/backend/coro_io/libcoroutine/asm.S
    PROPERTY LANGUAGE C
)

add_library(elle ${LINK_METHOD}

  assert.cc

  Buffer.cc
  Exception.cc
  IOStream.cc
  Printable.cc

  standalone/Log.cc
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
  io/Pattern.cc
  io/Piece.cc
  io/Path.cc

  system/System.cc

  types.cc

  concurrency/Program.cc
  concurrency/Routine.cc
  concurrency/Scheduler.cc

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

  network/Host.cc
  network/Interface.cc
  network/Locus.cc

  os/getenv.cc
  os/path.cc

  utility/Duration.cc
  utility/Factory.cc
  #utility/HTTP.cc
  utility/Parser.cc
  utility/Settings.cc
  utility/Time.cc

  format/hexadecimal.cc

  format/ini/Section.cc
  format/ini/File.cc

  format/json/Array.cc
  format/json/Bool.cc
  format/json/Dictionary.cc
  format/json/Factory.cc
  format/json/Float.cc
  format/json/Integer.cc
  format/json/Null.cc
  format/json/Object.cc
  format/json/Parser.cc
  format/json/String.cc

  serialize/ArchiveMode.cc
  serialize/Base64Archive.cc
  serialize/JSONArchive.cc
  serialize/IniArchive.cc

  Elle.cc
  print.cc

  log.cc

  log/Logger.cc

  reactor/src/reactor/backend/coro_io/libcoroutine/asm.S
  reactor/src/reactor/backend/coro_io/libcoroutine/context.cc
  reactor/src/reactor/backend/coro_io/libcoroutine/coroutine.cc
  reactor/src/reactor/backend/coro_io/thread.cc
  reactor/src/reactor/backend/pthread/pthread.cc
  reactor/src/reactor/backtrace.cc
  reactor/src/reactor/duration.cc
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

  Version.cc

  ${elle_MOCS}
  ${elle_PLATFORM}
)

add_dependencies(elle dependencies)
