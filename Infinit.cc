#include <elle/cryptography/PublicKey.hh>
#include <elle/utility/Time.hh>

#include <Infinit.hh>

//
// ---------- definitions -----------------------------------------------------
//

///
/// this definition specifies the Infinit authority public key which
/// can be used to verify the authenticity of the various certificates
/// populating the Infinit system.
///
const elle::String              Infinit::Key{
    "AAAAAAACtdQhPIswro5dUmN7mgacsdxvYhRKXoi9D72nC9cUieMT9FtwP7eqVyeCVV+FMBTZD"
    "GSxVrh7f9b0TeYqAu8sUbSCkQY+Qcbd2DOoOpuJztINuF2+2nP1LlMFYICeomUKj8wfDSoYo3"
    "Y7FEuSy2AeJzHnrV+DUjGcJGu02WswraqqQbf/6wNwcXBg0hcvegwuLTjyzmA9TzwT7lHvF3s"
    "Q2olxjOwk3+leRpiYHwV1yjlU+fR0cQeK+ouNmmIXaDbkM+BhoQDCmzD1nefNucf5J5cSRPHt"
    "kJiB9LU+Cz226e+Js91zrKEzxhLfaiVNrhIL5PSSz/jD+t7MpA8kTtT002JFOJTDhkuoAVSSN"
    "5By+/BTQa0609XRQ6+pEvyyZunRYkaZzciYHsg6PxfiURKyeWklK9ltqB2s/bmGLks5Bkblhk"
    "0ms/V5y2950oc6tUOur/FgnDLGegssriZQpxZ6rwx77/Y1jB8yg+MKu4u7/R/uMfkqrkEbUbY"
    "bUNRaIvhLOMTXCXKfMA8LrBDmzLiawozl0pVoMPqd1HQCZbl37hEh4afHf3Coivn0uZopdL+/"
    "xthQTYzR8eTQmyV89lVY/bA4dou2SHtx/potV3a+w3yF8NHzZ0y+Zzfukfs1eY15vTVFn4KHm"
    "p0npuc430bSNUoWB9KFgw+SOLZL83oTI8cAAAMAAQAB"
};

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
                                  "Copyright (c) 2012 "
                                  "infinit.io All rights reserved.");

///
/// this variable contains the authority.
///
elle::Authority                 Infinit::Authority;

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
  // create the autority.
  //
  {
    elle::cryptography::PublicKey     K;

    // ignore this step if the key is empty.
    //
    // this is especially useful whenever the authority must be author
    // the very first time.
    if (Infinit::Key.empty() == false)
      {
        // restore the authority's public key.
        if (K.Restore(Infinit::Key) == elle::Status::Error)
          escape("unable to restore the authority's public key");

        // create the authority based on the hard-coded public key.
        if (Infinit::Authority.Create(K) == elle::Status::Error)
          escape("unable to create the authority");
      }
  }

  //
  // load the configuration.
  //
  {
    // if the configuration file exists...
    if (lune::Configuration::exists() == true)
      Infinit::Configuration.load();

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
