//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Network.hh
//
// created       julien quintard   [thu oct 15 14:32:58 2009]
// updated       julien quintard   [mon jul 18 22:06:12 2011]
//

#ifndef ELLE_NETWORK_NETWORK_HH
#define ELLE_NETWORK_NETWORK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>
#include <elle/radix/Trait.hh>

#include <elle/package/Archive.hh>

#include <elle/network/Tag.hh>
#include <elle/network/Parcel.hh>
#include <elle/network/Message.hh>
#include <elle/network/Procedure.hh>

#include <elle/utility/Registrar.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace package;
  using namespace utility;

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
      // classes
      //

      ///
      /// this class represents the functionoid used to forward the call.
      ///
      class Functionoid:
	public Entity
      {
      public:
	//
	// constructors & destructors
	//
	virtual ~Functionoid()
	{
	}

	//
	// methods
	//
	virtual Status	Call(Archive&) const = 0;
      };

      ///
      /// this implementation takes an archive and triggers the
      /// procedure's callback.
      ///
      template <typename P>
      class Selectionoid:
	public Functionoid
      {
      public:
	//
	// constructors & destructors
	//
	Selectionoid(const P&);

	//
	// methods
	//
	Status		Call(Archive&) const;

	//
	// interfaces
	//

	// dumpable
	Status		Dump(const Natural32) const;

	//
	// attributes
	//
	P		procedure;
      };

      //
      // types
      //
      typedef std::map<const Tag, Functionoid*>		Container;
      typedef typename Container::iterator		Iterator;
      typedef typename Container::const_iterator	Scoutor;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      template <const Tag I,
		const Tag O>
      static Status	Register(const Procedure<I, O>&);

      static Status	Dispatch(Parcel*);

      static Status	Show(const Natural32 = 0);

      //
      // static attributes
      //
      static Container	Procedures;
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
#include <elle/network/Bridge.hh>
#include <elle/network/Data.hh>
#include <elle/network/Door.hh>
#include <elle/network/Gate.hh>
#include <elle/network/Header.hh>
#include <elle/network/Host.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Lane.hh>
#include <elle/network/Message.hh>
#include <elle/network/Outputs.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Parcel.hh>
#include <elle/network/Port.hh>
#include <elle/network/Procedure.hh>
#include <elle/network/Range.hh>
#include <elle/network/Raw.hh>
#include <elle/network/Session.hh>
#include <elle/network/Slot.hh>
#include <elle/network/Socket.hh>
#include <elle/network/Tag.hh>

#endif
