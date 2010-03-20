//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Link.hh
//
// created       julien quintard   [thu mar 18 21:05:22 2010]
// updated       julien quintard   [thu mar 18 23:30:29 2010]
//

#ifndef ELLE_NETWORK_LINK_HH
#define ELLE_NETWORK_LINK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Misc.hh>
#include <elle/concurrency/Concurrency.hh>

#include <elle/network/Socket.hh>

namespace elle
{
  using namespace misc;
  using namespace concurrency;

  namespace network
  {

//
// ---------- forward declarations --------------------------------------------
//

    class Door;
    class Gate;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class abstracts connection-based sockets such as doors and
    /// gates.
    ///
    class Link:
      public Socket
    {
    public:
      //
      // constructors & destructors
      //
      Link(Socket::Type);

      //
      // methods
      //
      template <typename I>
      Status	Send(const I,
		     const Identifier& = Identifier::Null);
      template <typename I>
      Status		Transmit(const I,
				 const Identifier& = Identifier::Null);
      template <typename I,
		typename O>
      Status		Call(const I,
			     O);
      template <typename O>
      Status		Receive(const Identifier&,
				O);
      template <typename I>
      Status		Reply(const I);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Link.hxx>

#endif
