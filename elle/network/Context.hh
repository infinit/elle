//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Context.hh
//
// created       julien quintard   [thu feb 25 13:50:42 2010]
// updated       julien quintard   [sun feb 28 11:01:52 2010]
//

#ifndef ELLE_NETWORK_CONTEXT_HH
#define ELLE_NETWORK_CONTEXT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Misc.hh>

#include <elle/network/Address.hh>
#include <elle/network/Socket.hh>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class enables receivers to access metadata related to a
    /// network communication such as the correspondant address etc.
    ///
    /// XXX \todo this class should be put in the local storage so that
    ///     every thread has one.
    ///
    class Context
    {
    public:
      //
      // enumerations
      //
      enum Mode
	{
	  ModeMessage,
	  ModeResponse,
	  ModeReturn
	};

      //
      // static methods
      //
      static Status	Initialize(Socket*,
				   const Address&);

      static Context*	Instance();

      //
      // static attrributes
      //
      static Context*	Environment; // XXX find another name!!!

      //
      // constructors & destructors
      //
      Context(Socket*,
	      const Address&);

      //
      // attributes
      //
      Socket*		socket;
      Address		address;
      Mode		mode;
    };

  }
}

#endif
