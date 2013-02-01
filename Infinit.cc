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
elle::String const Infinit::Key("AAAAAAAAAAAAAgAAqB6s7G8/N4Sp/Xw/kG7M8V93vLkhWxslzsbEjdBHsJ0P4BhXOaDv7HMlW8ENDxbrANH/OI61uwVT4Hw2HdB84XVk5nGRmDrWdwgJC8XHu9IP8pSkVha2rL6q3ZQ8BhA2chZhV5iftbgXfvyBaIr9etXUuzn/BFx4DAQI6AVQYbQ5SSljPZx8CJcmJJjJ+D2yT3B9ge+ueZciLMZuuNlJGeECd7hOd2jcI5UZZEZNMhtJ+tagqb67OXBKmU4fRdhQS5JXir6C0oXsSb1MuyHvcViZokxdo5jnr+ll73Be+ZfSDfta585vdm1nWjCk7NsvAMmk5rP/JcaVzuyAK7zCmCpjeT1bi6MCGsZtE4hwf+KxyANwHHyTa7PD0iM9ZUFpbKfjOye+ZLXIV84JYM9TkwvIaJJS28V6MxtF9tj+Hy9jhSuk0cSDX5fUerPtnGe5Z9zz6ABPGHFTkFhBtNJFtSwbqGOMdyootJMgOo0IGeOZY5kPXOibXU+aodUM5q6pqs/T928p+qN6UAbkXnnYmhgQjDNWldf1PR2es2rWELIQGsrKzP5ceQioK3hOkItsI8mjFUnDF6FLDxCUAHt7AuqcVklZ2JCW591pTYw8MeXtt2AwRLpQjnKZcA3Pv4MBZeYXiR3PzqZopSeWnZkbppOZsafi2by0z2+erhabOW8AAAMAAAABAAE=");

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
