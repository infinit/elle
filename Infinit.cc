//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/Infinit.cc
//
// created       julien quintard   [tue may  4 22:01:45 2010]
// updated       julien quintard   [tue sep  6 17:40:47 2011]
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
const elle::String		Infinit::Key("DdoCAPLNAdjAlqMY2pBzKjg06nZrsC9Xc35eJPqfbz+p5EI4FX0IBNwG4De-yN1tis5JL5dTDsANCMwA+hYTUb1WmY1Dqka5OMZp6vsFXAcQxjL3h+JbZ+wwiJnzmDJM3oBhZfHzWaQcVHAH2-vDFcOVlv3yGMYntHim-d8VVyAqzMkn0Wndm9eWJJTzL9rtf-kk9p5nhk0P9rvggDhiVlPmfB6TTz76JgazsVI8wKcex5WmsUVDy26qqo0ETFDYzw5Z5W5JqYlOtxUXo6ChyHT6XE1K49-JnWdCLKFMwFw8reiuIMsC7TxJ41JiK+2KXWZ-kPKRJF5nZ+vc6oaf5aKr1aOXT01a+ArvnRB31MXMxzooxS416BLPH9-TN86Y3uWPWtKC7jEi+9yamHaTDy8j-0eAndSfXfwqW3-eosgnK1oVVZqVNi1W2yqcj3p2Nvs9XtYcJ4O0yand+Xc8miqFfe64FsyOehzzLRxwOxKCLI0rz7jqfiYfOL0+4ebTksL0DIqcyUhaeEoKXjhTVEe+5OiUvyMQiNWQpFlAtH8WRQZvAy9zZKN1bGdxSRt7F1Ld5JZiWfRwXxZpvFs31ePisGSSWBMWuI3Sd46NGlFYyGfHL91LQc259tcRp26kqPTCHsifuCZnhzoWLQflWAmlksSyFcktmbP6bRhQp93hvz+hDaMBAAE=");

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
    // this is especially useful whenever the authority must be created
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
