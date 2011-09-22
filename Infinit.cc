//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// author        julien quintard   [tue may  4 22:01:45 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>

//
// ---------- definitions -----------------------------------------------------
//

///
/// this definition specifies the Infinit authority public key which
/// can be used to verify the authenticity of the various certificates
/// populating the Infinit system.
///
const elle::String		Infinit::Key("DdoCALF1pjOayQtUIHujHB5z7lNUBKIhhalQ893t48U5A6SNWJOzKwx+oi7fD7fZXEenaBnor+hPdF4KuhEIhQMWsPIwlJCvSQm-YzG5x75ab4EjC5ghvAnRQVHbhhbHyuzy4DbS8c6hM5L6yXP0ExtQHKnCZlYjf59jtb6xb691t6zMeILxeWK8F1M19oLPcJd3Gq2EXRFrGb+Zy9fXK4OmjtnzrFk13T5GJ9WiLfeFQ+PlPDlIYc0iyuanvVPQcBHCC0XgGnI5WfGyCgp9zQ17fQAfHy-RvjRlHcx3A1uFjwLRjcNKSjcs1gwetPl6l2hemtNGDUpxephuw7bmDQiisGziv4JJdNhEBn7mXLBD+bJC6-KerrKamb7pWWEsp2Pz09AKrw+1uuIFJVGmQDkgqBI83aqYQV4bPjIBB498TgyNCZxhWviA4DisUi+t4PnNUysbz6+5vgkIMw+ewnF01tDwp7W7g6x-M5Abr15H47d0lGHJpCjFZY+vJeOyLhFM3bDbP9WieHTiia+GuU2yK7JL-pPrVPeoxGUblT8WedxipTxRtBrx4Z9G5pNSF7tVa7G2wp-9pjoaphMpDbyhqi5K91qT7PcdYUS9yCD3Y8u1As1XOPIIbYgSayNWe2-JCvK1i-uzxWQDNpgQE-6gcuvt--G57mOS7jP5-0BGGR5lDaMBAAE=");

///
/// this constant contains the version string.
///
const elle::String		Infinit::Version("Infinit[alpha]");

///
/// this constant contains the copyright string.
///
const elle::String		Infinit::Copyright(
				  Infinit::Version +
				  " " +
				  "Copyright (c) 2007, ..., 2012 "
				  "Julien Quintard, All rights reserved.\n");

///
/// this variable contains the authority.
///
lune::Authority			Infinit::Authority;

///
/// this variable contains the system configuration
///
lune::Configuration		Infinit::Configuration;

///
/// this variable contains the program's parser.
///
elle::Parser*			Infinit::Parser;

//
// ---------- methods ---------------------------------------------------------
//

///
/// this method initializes Infinit.
///
elle::Status		Infinit::Initialize()
{
  enter();

  // disable the meta logging.
  if (elle::Meta::Disable() == elle::StatusError)
    escape("unable to disable the meta logging");

  //
  // create the autority.
  //
  {
    elle::PublicKey	K;

    // ignore this step if the key is empty.
    //
    // this is especially useful whenever the authority must be author 
    // the very first time.
    if (Infinit::Key.empty() == false)
      {
	// restore the authority's public key.
	if (K.Restore(Infinit::Key) == elle::StatusError)
	  escape("unable to restore the authority's public key");

	// create the authority based on the hard-coded public key.
	if (Infinit::Authority.Create(K) == elle::StatusError)
	  escape("unable to create the authority");
      }
  }

  //
  // load the configuration.
  //
  {
    // if the configuration file exists...
    if (Infinit::Configuration.Exist() == elle::StatusTrue)
      {
	// load the configuration file.
	if (Infinit::Configuration.Load() == elle::StatusError)
	  escape("unable to load the configuration");
      }

    // pull the parameters.
    if (Infinit::Configuration.Pull() == elle::StatusError)
      escape("unable to pull the configuration parameters");
  }

  // enable the meta logging.
  if (elle::Meta::Enable() == elle::StatusError)
    escape("unable to enable the meta logging");

  leave();
}

///
/// this method cleans Infinit.
///
elle::Status		Infinit::Clean()
{
  enter();

  // nothing to do.

  leave();
}
