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
// updated       julien quintard   [mon jun 20 01:47:20 2011]
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
const elle::String		Infinit::Key("RUxMAQ0AAAAAAgAAsKKVNXszNo9F2vug8A-TcYCn4mVUeZorx5vGN6OACFSh18COd1e8+Iu0j-0+Ja0e52b9qKeEQ3UHZUJ1+1bKxmOsQTvUOLFkgBXbmwnOtqg1KDNpFaHqtzAgVN3uQEcYKlBWnA-26otBZTgTxXMOMgLWog+s0wqhTwhiZ1YkUygy+Ld8ljvOOccyVxHyLOxhx8NKybnuPvBKz6JvSEUvmk4tLLyx82japPu0ck3HQCRQGaIliAnFWG70M02ME8+80RO3zMkkmxhQiI5z8GIo0uvWwXlA7Cv4DEh9ej74XvbyXQ0iaO0Evo1zG79xoCJ8XNdZeyL+DmHFf65D8mLxq9GL8lAipImuBrJVcueNrKNqIDCwvpnJM9pLQ0ZKKTiumjPQH9rLTVTzKxONPlHXdVw-okniAmFtCZTpo1sYF4q-Tnb0yLOfL5YVq+y8GNGUp-6P9hoDhLUTU5CoWyrV1UznK4PhT-TtUkqi8KfnrNY8jZBYhmD2ChUa+7YCBvCgRu4VIcVynUvHCPYKAVhgCH9DAWkGkZbPMv5uB7HGGxQVtmv2JHkZKtK+BxvkTY2roLGm9sJPozwQblfPD24G-ASgHmyakkvINR8S4j1Di6VTpErw4IUKe9IXRHOWsFx2v4Wh9CUQjmE0cu4jRvHYUvSMKXvKYP+jMs7UdCtmk2kNAAAAAwAAAAEAAQ==");

///
/// this constant contains the version string.
///
const elle::String		Infinit::Version("Infinit[alpha]");

///
/// this variable contains the authority.
///
lune::Authority			Infinit::Authority;

//
// ---------- methods ---------------------------------------------------------
//

///
/// this method initializes Infinit.
///
elle::Status		Infinit::Initialize()
{
  enter();

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
