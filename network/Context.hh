//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       network
//
// file          /home/mycure/infinit/elle/network/Context.hh
//
// created       julien quintard   [fri mar  5 10:43:11 2010]
// updated       julien quintard   [sun mar  7 20:09:10 2010]
//

#ifndef ELLE_NETWORK_CONTEXT_HH
#define ELLE_NETWORK_CONTEXT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>

#include <elle/network/Socket.hh>
#include <elle/network/Address.hh>

namespace elle
{
  using namespace core;
  using namespace misc;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains metadata related to an event such as the
    /// socket and sender's address for instance.
    ///
    class Context:
      public Dumpable
    {
    public:
      //
      // static methods
      //
      static Status	Assign(Context*);

      //
      // constructors & destructors
      //
      Context(Socket*,
	      const Address&);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Socket*		socket;
      Address		address;
    };

//
// ---------- extern ----------------------------------------------------------
//

    extern Context*		context;

  }
}

#endif
