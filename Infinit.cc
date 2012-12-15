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
const elle::String              Infinit::Key{"AAAAAAACthRb4Cv4wLbwxOmkPrDrK/h7nuZYJB16Uv2m5jIKq355Seo72ijEGqS1gHjgJMO8twz+L+RoIoQiedO0vnH8HZZjPOWV756pn4+PxVQiG76fPhwXzJ7Ix2bcUP7Em56gpp4sP/Z6xBRH4mldbd49nRWoqEVm07og5twO9KW6h5Zc/ZbVAbjlyzoC4KuECgt6EFtw/uW6sANvR1YkflQKuOPAM4WFpSD7mOMSGJ6yKeeiYp8RULFy0q3lKVqy1v4W5iVUQsLHOtSR3yojGwuFUfhV9C9jmDnq5OrvifIzLwu40kUDyxChF7EOiy6d6qJzDDL9sUpSPoz3xQajiH0Q/oiUaFDP0tula1Xph4vvZHcf7ojcBPSKij8mJdyLNnPCvfMZap6cg8wSBLVrAde8/RRxjnsKjrd7L/0y6C6MYC95SXVvmNrX71NBgl2XXEr1oEdDrXtQy/4pTXQxIlS4/gtU6VOlfAzDY2kz9eeKapK1QsO1fbEw304kO+6HaIvjZhudp6ByV71bNm+ph7lrjH46xT81dfFEzFv3kWuQQu/1A9Y5jLMhyoiCktZ0s81zUw6OKwfHwk6BEeP4M57k8fNPb9jG4Lh4xsExG4s3kOYEV5QtKI4RRhKm/JqhTDQpqawJeWdAONtg3PKHAog7qRYchhTSjmiQCLXC7vDJLoUAAAMAAQAB"};

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
