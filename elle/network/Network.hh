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
// updated       julien quintard   [fri feb  5 02:14:12 2010]
//

#ifndef ELLE_NETWORK_NETWORK_HH
#define ELLE_NETWORK_NETWORK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/archive/Archive.hh>

#include <elle/network/Host.hh>
#include <elle/network/Socket.hh>
#include <elle/network/Tag.hh>
#include <elle/network/Environment.hh>

namespace elle
{
  using namespace core;
  using namespace archive;

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
      /// this class is the base class for functionoids.
      ///
      class Functionoid:
	public Dumpable
      {
      public:
	//
	// enumeration
	//
	enum Type
	  {
	    TypeUnknown,
	    TypeFunction,
	    TypeMethod
	  };

	//
	// constructors & destructors
	//
	Functionoid();
	Functionoid(Type);

	//
	// methods
	//
	virtual Status		Trigger(Environment&,
					Archive&) const = 0;

	//
	// attributes
	//
	Type		type;
      };

      ///
      /// this functionoid represents a callback.
      ///
      template <typename T>
      class CallbackFunction:
	public Functionoid
      {
      public:
	//
	// constructors & destructors
	//
	CallbackFunction(Status (*)(Environment&,
				    T&));

	//
	// methods
	//
	Status		Trigger(Environment&,
				Archive&) const;

	//
	// interfaces
	//

	// dumpable
	Status		Dump(const Natural32 = 0) const;

	//
	// attributes
	//
	Status		(*function)(Environment&,
				    T&);
      };

      template <typename T, class I>
      class CallbackMethod:
	public Functionoid
      {
      public:
	//
	// constructors & destructors
	//
	CallbackMethod(I*,
		       Status (I::*)(Environment&,
				     T&));

	//
	// methods
	//
	Status		Trigger(Environment&,
				Archive&) const;

	//
	// interfaces
	//

	// dumpable
	Status		Dump(const Natural32 = 0) const;

	//
	// attributes
	//
	I*		instance;
	Status		(I::*method)(Environment&,
				     T&);
      };

      //
      // types
      //
      typedef std::map<Tag, Functionoid*>	Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static methods
      //
      template <typename T>
      static Status	Register(Status (*)(Environment&,
					    T&));
      template <typename T, class I>
      static Status	Register(I*,
				 Status (I::*)(Environment&,
					       T&));

      static Status	Dispatch(Environment&,
				 Archive&);

      static Status	Dump(const Natural32 = 0);

      //
      // static attributes
      //
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

#endif
