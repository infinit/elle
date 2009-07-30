PROJECT(Elle CXX)

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
  io/IO.cc
  misc/Region.cc
  misc/Report.cc
  system/System.cc
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

TARGET_LINK_LIBRARIES(test-archive elle ssl crypto dl)
TARGET_LINK_LIBRARIES(test-crypto elle ssl crypto dl)
