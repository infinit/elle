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
// updated       julien quintard   [wed sep 14 14:51:10 2011]
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
const elle::String		Infinit::Key("DdoCANhuARnBt9QTo3B9dcIviF-1iQCBc9CQvUxzg6w0mIiFaLCUaAas+ZSNwaKBvbdaZT-nSm2zzNOdgD+KlPu0FZDhXypNArOKylNVmTRzrPZZBQ67Q6K+gQeCXk2MAGS2itxvyTzYGwMdzbwo6Kkhq+R2FCMswbV8SUcLyQCn-KRCgZuwZia4VNfOvcDUkVxCDSfOvzM2bPUnTTrwy9d4Jur-knynQS9AVFm1te3ynUeYVbRmBzfI54qbhJVfy4iu0L0RNaDRgq+sLo51klOpspmu7LUc3-NysXDD63JH6bkB6cVKDanMeJ2yzSezLtAXZkU1axG96G2sQQ0zowhL9l5Wqfi4Kd1z31DWb6Fh7kA3WtzUtGikZ72R5HySZMi2ELpAxXub83CrrG0Z7xEODYIeSjopCZ11124mM8XS2cxrjKKV7fY7SBAjmTSKbMW+SJxpA9-mLMVctJbu-yMruAszZuRtBj3LGbDQQE79RgKbg8rupbx2-3TqQ7tIGpdiHq+OxrqqVdkgbRDQem4IRaXoN+WydD7GauQgVmxP4yTy-Au+sGwNxXfKkJZ-zmBMyfPEXF4e-gZ-yhy6xEilMw2Zd9nGjS+LGqsUq45cUaI6msFSK6X0RsV662H7-cEGj7wrbDXdgkgVvzbrXzb2IC6URQOX34IGm9EWgGrkCaq1DaMBAAE=");

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
