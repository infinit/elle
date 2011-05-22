//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/remote/Remote.cc
//
// created       julien quintard   [fri may 20 19:32:16 2011]
// updated       julien quintard   [sun may 22 11:36:30 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <hole/remote/Remote.hh>

namespace hole
{
  namespace remote
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Remote::Remote(const nucleus::Network&			network):
      Holeable(network)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status	Remote::Host(const elle::Address&	host)
    {
      enter();

      // set the host.
      this->host = host;

      leave();
    }

//
// ---------- holeable --------------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status	Remote::Join()
    {
      enter();

      // XXX TCP connection

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Remote::Leave()
    {
      enter();

      // XXX close connection

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Remote::Put(const nucleus::Address&	address,
				    const nucleus::ImmutableBlock& block)
    {
      enter();

      // XXX
      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Remote::Put(const nucleus::Address&	address,
				    const nucleus::MutableBlock& block)
    {
      enter();

      // XXX
      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Remote::Get(const nucleus::Address&	address,
				    nucleus::ImmutableBlock&	block)
    {
      enter();

      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Remote::Get(const nucleus::Address&	address,
				    const nucleus::Version&	version,
				    nucleus::MutableBlock&	block)
    {
      enter();

      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Remote::Kill(const nucleus::Address&	address)
    {
      enter();

      // XXX

      leave();
    }

  }
}
