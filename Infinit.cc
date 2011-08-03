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
// updated       julien quintard   [mon aug  1 10:23:27 2011]
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
const elle::String		Infinit::Key("RUxMAQ0AAAAAAgAAxcbd+krWt+U+s5Uu9pTcJqH7y4aEPyOfneFTPUhmp7Ogf1eOzZqHj5Qmdyt3NsYuNYslpzD32zv-xbvFyTm9dOmqkzdsZ8LuxTSVVTRsu+3xS21Fl56KV0J3p7WymvP6VtbHguF-u72+Q3MppN-HtJ0tz+nL8v0nU1pCdFPLjBRL5LKEEc5uyhVKKgz12qfu83IteTPz-qAVt71gDPdxHxNqFtGs04LjZDvB7hhhratakmOTeOPTz+E7Fo-2qTZ5No-cVe7Du94sJahjBbhjRIKvE72fzWVHmUS8+jVS3BPF4QOc5j8eQIlbOV-pUuzqPfIY51LpZesiR6dPg36NXK2RMGWEUiwDAGC6j3kwlXKXByLRnE8WpiaCZxQQOM1ai7lT0KIpfluD4njDU1MdaV6oxlrF8A-Mv8EQLsUzL+AA41rvP0NW8xB7vcKqUMg3wGn6J-GZBTT56SuGkC6oDyVO0s5NScOgfYlF5ehH9KqQQHYyh6RfsgQOTeWD2+fvUU8KJz8x3dGXs9O+0y+3E14SoFmJYEuTjM2fdmmxSmkQMl51tiCvmwVr5vHR7MLvT7HC4pV5MzV+QGoCakR+6CfM596FOmXh7XHiIFjshogGrjPA07MtAggvlDZ3bt-Z6zta5vaYHtFvDZads18B77BgPCNAaluBgiiJRWZ-IoMNAAAAAwAAAAEAAQ==");

///
/// this constant contains the version string.
///
const elle::String		Infinit::Version("Infinit[alpha]");

///
/// this constant contains the copyright string.
///
const elle::String		Infinit::Copyright(
				  Infinit::Version +
				  " "+
				  "Copyright (c) 2008, 2009, 2010, 2011, "
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

    // restore the authority's public key.
    if (K.Restore(Infinit::Key) == elle::StatusError)
      escape("unable to restore the authority's public key");

    // create the authority based on the hard-coded public key.
    if (Infinit::Authority.Create(K) == elle::StatusError)
      escape("unable to create the authority");
  }

  //
  // load the configuration.
  //
  {
    // load the configuration file.
    if (Infinit::Configuration.Load() == elle::StatusError)
      escape("unable to load the configuration");

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
