#include <elle/utility/Time.hh>

#include <cryptography/PublicKey.hh>

#include <Infinit.hh>

//
// ---------- definitions -----------------------------------------------------
//

///
/// this definition specifies the Infinit authority public key which
/// can be used to verify the authenticity of the various certificates
/// populating the Infinit system.
///
elle::String const Infinit::Key("AAAAAAAAAAAAAgAAwvtjO51oHrOMK/K33ajUm4lnYKWW5dUtyK5Pih7gDrtlpEPy7QWAiYjY8Kinlctca2owzcPXrvFE34gxQE/xz11KLzw4ypn4/ABdzjaTgGoCNLJ2OX99IPk6sEjIHwFxR9YcewD6uED2FQgv4OfOROHaL8hmHzRc0/BxjKwtI6fT7Y/e1v2LMig6r30abqcLrZN+v+3rPHN4hb8n1Jz7kRxRbtglFPLDpvI5LUKEGmu3FPKWWZiJsyFuuLUoC9WsheDDZoHYjyrzMD0k7Sp5YVGBBDthZc6SQDp1ktPSTou1Opk+1IxHp/we1/HNhULvGvr6B1KYZJhb/R55H0k6GcaRQmNEKgiLcF6Z9lA5asK49CC/tlZjKRkXkLBKR9zGIODsweY+O9y3AeGX+Pfk9itPals2egsxc/q2mbRaC9svY2TXMwiSO4EPiedqvpuTKj1KTcRbQrp5mSxG1nzaCGyCmUeGzoBJZLNVJHpytAfwf0oYWfo9NOD9dkKkkL5jxfW3+MOwEx4i0tP3xdKmt6wC6CSXedFVm55oOcz2YgARG3hw0vBdLtl3jxfbXAFjCNnpkMrCEMfjzs5ecFVwhmM8OEPqHpyYJYO/9ipwXAKRPugFzMvoyggSA6G5JfVEwuCgOp2v82ldsKl0sC34/mBeKrJvjaZAXm39f6jTw/sAAAMAAAABAAE=");

///
/// this constant contains the version of the current Infinit software.
///
const elle::Version Infinit::version(INFINIT_VERSION_MAJOR,
                                     INFINIT_VERSION_MINOR);

///
/// this constant contains the copyright string.
///
const elle::String              Infinit::Copyright(
                                  "Infinit " +
                                  elle::sprint(Infinit::version) +
                                  " " +
                                  "Copyright (c) 2013 "
                                  "infinit.io All rights reserved.");

// XXX[temporary: for cryptography]
using namespace infinit;

static
elle::Authority
_authority()
{
  cryptography::PublicKey K;

  assert(!Infinit::Key.empty());
  if (K.Restore(Infinit::Key) == elle::Status::Error)
    throw reactor::Exception("unable to restore the authority's public key");
  return elle::Authority(K);
}

elle::Authority
Infinit::authority()
{
  static elle::Authority authority(_authority());
  return authority;
}

///
/// this variable contains the system configuration
///
lune::Configuration             Infinit::Configuration;

///
/// this variable contains the program's parser.
///
elle::utility::Parser*                   Infinit::Parser;

///
/// this variable holds the user name.
///
elle::String                    Infinit::User;

///
/// this variable holds the network name.
///
elle::String                    Infinit::Network;

///
/// this variable holds the mountpoint.
///
elle::String                    Infinit::Mountpoint;

//
// ---------- methods ---------------------------------------------------------
//

///
/// this method initializes Infinit.
///
elle::Status            Infinit::Initialize()
{
  // if the configuration file exists...
  if (lune::Configuration::exists(Infinit::User) == true)
    Infinit::Configuration.load(Infinit::User);

  // pull the parameters.
  if (Infinit::Configuration.Pull() == elle::Status::Error)
    throw elle::Exception("unable to pull the configuration parameters");

  return elle::Status::Ok;
}

///
/// this method cleans Infinit.
///
elle::Status            Infinit::Clean()
{
  // nothing to do.

  return elle::Status::Ok;
}
