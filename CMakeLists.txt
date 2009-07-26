PROJECT(Elle CXX)

ADD_LIBRARY(elle
  archive/Archive.cc
  core/Large.cc
  core/Null.cc
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
  io/IO.cc
  misc/Region.cc
  misc/Report.cc
  system/System.cc
)

ADD_EXECUTABLE(archive
  test/archive/Generator.cc
  test/archive/Pack.cc
  test/archive/Referee.cc
  test/archive/Test.cc
)

ADD_EXECUTABLE(crypto2
  test/crypto/Test.cc
)

TARGET_LINK_LIBRARIES(archive elle ssl crypto dl)
TARGET_LINK_LIBRARIES(crypto2 elle ssl crypto dl)

INSTALL(
  TARGETS elle
  ARCHIVE DESTINATION ${INFINIT_LIBDIR})

INSTALL(FILES
  Elle.hh
  DESTINATION ${INFINIT_INCLUDEDIR}/elle)

INSTALL(FILES
  archive/Archivable.hh
  archive/Archive.hh
  archive/Archive.hxx
  DESTINATION ${INFINIT_INCLUDEDIR}/elle/archive)

INSTALL(FILES
  core/Boolean.hh
  core/Character.hh
  core/Core.hh
  core/Core.hxx
  core/Integer.hh
  core/Large.hh
  core/Natural.hh
  core/Null.hh
  core/Object.hh
  core/Real.hh
  core/String.hh
  DESTINATION ${INFINIT_INCLUDEDIR}/elle/core)

INSTALL(FILES
  crypto/Cipher.hh
  crypto/Clear.hh
  crypto/Code.hh
  crypto/Cryptography.hh
  crypto/Digest.hh
  crypto/KeyPair.hh
  crypto/OneWay.hh
  crypto/Plain.hh
  crypto/PrivateKey.hh
  crypto/PublicKey.hh
  crypto/SecretKey.hh
  crypto/Signature.hh
  DESTINATION ${INFINIT_INCLUDEDIR}/elle/crypto)

INSTALL(FILES
  io/Dumpable.hh
  io/IO.hh
  DESTINATION ${INFINIT_INCLUDEDIR}/elle/io)

INSTALL(FILES
  misc/Byte.hh
  misc/Misc.hh
  misc/Region.hh
  misc/Report.hh
  misc/Status.hh
  DESTINATION ${INFINIT_INCLUDEDIR}/elle/misc)

INSTALL(FILES
  system/System.hh
  DESTINATION ${INFINIT_INCLUDEDIR}/elle/system)
