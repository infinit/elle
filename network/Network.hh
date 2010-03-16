//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Network.hh
//
// created       julien quintard   [thu oct 15 14:32:58 2009]
// updated       julien quintard   [tue mar 16 12:20:49 2010]
//

#ifndef ELLE_NETWORK_NETWORK_HH
#define ELLE_NETWORK_NETWORK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Misc.hh>
#include <elle/archive/Archive.hh>
#include <elle/concurrency/Concurrency.hh>

#include <elle/network/Host.hh>
#include <elle/network/Socket.hh>
#include <elle/network/Tag.hh>
#include <elle/network/Raw.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Parameters.hh>
#include <elle/network/Data.hh>
#include <elle/network/Context.hh>
#include <elle/network/Parcel.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace archive;
  using namespace concurrency;

  ///
  /// XXX
  ///
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Network
    {
    public:
      //
      // constants
      //
      static const Natural32		Timeout = 0;

      //
      // classes
      //

      ///
      /// this class represents the functionoid used to dispatch the
      /// network event.
      ///
      class Functionoid:
	public Dumpable
      {
      public:
	//
	// methods
	//
	virtual Status	Call(Data&) const = 0;
      };

      ///
      /// this implementation takes an archive, extracts a number of
      /// arguments depending on the selectionoid and triggers the callback.
      ///
      template <const Natural32, typename M>
      class Selectionoid:
	public Functionoid
      {
      };

      //
      // types
      //
      typedef std::map<const Tag, Functionoid*>		Container;
      typedef Container::iterator			Iterator;
      typedef Container::const_iterator			Scoutor;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      template <const Tag G>
      static Status	Register(Callback&);

      static Status	Dispatch(Context*&,
				 Header*&,
				 Data*&);

      static Status	Dump(const Natural32 = 0);

      //
      // static attributes
      //
      static Accord	Control;

      static Container	Callbacks;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Network.hxx>

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Address.hh>
#include <elle/network/Port.hh>
#include <elle/network/Message.hh>
#include <elle/network/Slot.hh>
#include <elle/network/Bridge.hh>
#include <elle/network/Door.hh>
#include <elle/network/Parameters.hh>
#include <elle/network/Arguments.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Outputs.hh>
#include <elle/network/Session.hh>

#endif
