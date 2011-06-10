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
// updated       julien quintard   [thu jun  2 15:55:45 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>

//
// ---------- definitions -----------------------------------------------------
//

///
/// this definition specifies the Infinit authority's public key which
/// can be used to verify the authenticity of the various certificates
/// populating the Infinit system.
///
const elle::String		Infinit::Authority("RUxMAQ0AAAAAAgAAsKKVNXszNo9F2vug8A-TcYCn4mVUeZorx5vGN6OACFSh18COd1e8+Iu0j-0+Ja0e52b9qKeEQ3UHZUJ1+1bKxmOsQTvUOLFkgBXbmwnOtqg1KDNpFaHqtzAgVN3uQEcYKlBWnA-26otBZTgTxXMOMgLWog+s0wqhTwhiZ1YkUygy+Ld8ljvOOccyVxHyLOxhx8NKybnuPvBKz6JvSEUvmk4tLLyx82japPu0ck3HQCRQGaIliAnFWG70M02ME8+80RO3zMkkmxhQiI5z8GIo0uvWwXlA7Cv4DEh9ej74XvbyXQ0iaO0Evo1zG79xoCJ8XNdZeyL+DmHFf65D8mLxq9GL8lAipImuBrJVcueNrKNqIDCwvpnJM9pLQ0ZKKTiumjPQH9rLTVTzKxONPlHXdVw-okniAmFtCZTpo1sYF4q-Tnb0yLOfL5YVq+y8GNGUp-6P9hoDhLUTU5CoWyrV1UznK4PhT-TtUkqi8KfnrNY8jZBYhmD2ChUa+7YCBvCgRu4VIcVynUvHCPYKAVhgCH9DAWkGkZbPMv5uB7HGGxQVtmv2JHkZKtK+BxvkTY2roLGm9sJPozwQblfPD24G-ASgHmyakkvINR8S4j1Di6VTpErw4IUKe9IXRHOWsFx2v4Wh9CUQjmE0cu4jRvHYUvSMKXvKYP+jMs7UdCtmk2kNAAAAAwAAAAEAAQ==");

///
/// this constant contains the version string.
///
const elle::String		Infinit::Version("Infinit[alpha]");

///
/// this variable contains the network descriptor.
///
lune::Descriptor		Infinit::Descriptor;

///
/// this variable contains the user identity.
///
lune::Identity			Infinit::Identity;

//
// ---------- methods ---------------------------------------------------------
//

///
/// this method initializes Infinit.
///
elle::Status		Infinit::Initialize(const elle::String&	user,
					    const elle::String&	network)
{
  lune::Authority	authority;

  enter();

  //
  // load the authority.
  //
  {
    elle::PublicKey	K;

    // restore the authority's public key.
    if (K.Restore(Infinit::Authority) == elle::StatusError)
      escape("unable to restore the authority's public key");

    // create the authority based on the hard-coded public key.
    if (authority.Create(K) == elle::StatusError)
      escape("unable to create the authority");
  }

  //
  // load the user identity.
  //
  {
    elle::String	prompt;
    elle::String	pass;

    // does the identity exist.
    if (Infinit::Identity.Exist(user) == elle::StatusFalse)
      escape("the user identity does not seem to exist");

    // prompt the user for the passphrase.
    prompt = "Enter passphrase for keypair '" + user + "': ";
    pass = elle::String(::getpass(prompt.c_str()));

    // load the identity.
    if (Infinit::Identity.Load(user) == elle::StatusError)
      escape("unable to load the identity");

    // verify the identity.
    if (Infinit::Identity.Validate(authority) != elle::StatusTrue)
      escape("the identity seems to be invalid");

    // decrypt the identity.
    if (Infinit::Identity.Decrypt(pass) == elle::StatusError)
      escape("unable to decrypt the identity");
  }

  //
  // retrieve the descriptor.
  //
  {
    // does the network exist.
    if (Infinit::Descriptor.Exist(network) == elle::StatusFalse)
      escape("this network does not seem to exist");

    // load the descriptor.
    if (Infinit::Descriptor.Load(network) == elle::StatusError)
      escape("unable to load the descriptor");

    // validate the descriptor.
    if (Infinit::Descriptor.Validate(authority) != elle::StatusTrue)
      escape("unable to validate the descriptor");
  }

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
