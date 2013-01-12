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
const elle::String Infinit::Key{"AAAAAAACAADF97QoZjk/7NcNVzInsPL1X+7M3mDH4Ca9yj3WBmjfmkLVF53QqtQ0yl3/7iykafbkNDdOHrJChmNN1RjnK8fikdd1/R4T2Tc1K+TeFUPyOv5STWvoJbNGXyFaxt3TVmd8FQ5SoCjspORZ+Ze38My6GEDKbZvknRPWAi6lbARAtxtFFhOiYQ3Ge2uefqHjSAFqk7VVVowoUQP2GmTOzCQfBwv6+e7M8Psi4WpvJnjDMgsVC8NCydKhGytAry9xO7yzX0PvDgpy48eozAIAEyRko5+DqPIl0dHc2DiJYMUh7rmSs6ECKvGYMEZXia4m5zIQVwELBz5IBbM6zcNZhRvmlzXBcy+js2lwJVIfdNOd/ymXPjcRGCPkr7eMotp2R0OAHK0wbR5vsUZ4EqlgA8NEAblpPwP1pxqtuP2km9pRt266HhuZO+50JXleVdyEsYPulZ5t5LEZS60gygbEvstM5gkoXpdJQQcmz1Tiu92VolCPfgqJNaciMaiP9wyj6WNqAYX/eRuOU/PqKO0NDo56COfgIx1NbZrrdncFczOEYH/bWXZc+LVdvySKLUJI0BZ+3BXRj/BNNWtA6cS7BcyBJMTjA+KFuTWftq3nW/y/1BH1Hxnd9B0P3/u1hGkf/F64XxLNj9cvwIVGMvduLLbrzgEWEb3F2RFPSnXyxU1J/QAAAwAAAAEAAQ=="};

///
/// this constant contains the version of the current Infinit software.
///
const elle::Version Infinit::version(0, 4);

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
    throw reactor::Exception(elle::concurrency::scheduler(),
                             "unable to restore the authority's public key");
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
  // disable the meta logging.
  if (elle::radix::Meta::Disable() == elle::Status::Error)
    escape("unable to disable the meta logging");

  //
  // load the configuration.
  //
  {
    // if the configuration file exists...
    if (lune::Configuration::exists(Infinit::User) == true)
      Infinit::Configuration.load(Infinit::User);

    // pull the parameters.
    if (Infinit::Configuration.Pull() == elle::Status::Error)
      escape("unable to pull the configuration parameters");
  }

  // enable the meta logging.
  if (elle::radix::Meta::Enable() == elle::Status::Error)
    escape("unable to enable the meta logging");

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
