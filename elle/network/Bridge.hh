//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Bridge.hh
//
// created       julien quintard   [thu feb  4 14:39:34 2010]
// updated       julien quintard   [thu feb  4 16:52:33 2010]
//

#ifndef ELLE_NETWORK_BRIDGE_HH
#define ELLE_NETWORK_BRIDGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/network/Door.hh>

#include <QObject>
#include <QLocalServer>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Bridge
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
	virtual Status		Trigger(Door&) const = 0;

	//
	// attributes
	//
	Type		type;
      };

      ///
      /// this functionoid represents a callback.
      ///
      class CallbackFunction:
	public Functionoid
      {
      public:
	//
	// constructors & destructors
	//
	CallbackFunction(Status (*)(Door&));

	//
	// methods
	//
	Status		Trigger(Door&) const;

	//
	// interfaces
	//

	// dumpable
	Status		Dump(const Natural32 = 0) const;

	//
	// attributes
	//
	Status		(*function)(Door&);
      };

      template <class I>
      class CallbackMethod:
	public Functionoid
      {
      public:
	//
	// constructors & destructors
	//
	CallbackMethod(I*,
		       Status (I::*)(Door&));

	//
	// methods
	//
	Status		Trigger(Door&) const;

	//
	// interfaces
	//

	// dumpable
	Status		Dump(const Natural32 = 0) const;

	//
	// attributes
	//
	I*		instance;
	Status		(I::*method)(Door&);
      };

      //
      // types
      //
      typedef std::list<Functionoid*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static methods
      //
      Status		Register(const String&,
				 Status (*)(Door&));
      template <class I>
      Status		Register(const String&,
				 I*,
				 Status (I::*)(Door&));

      Status		Dump(const Natural32 = 0);

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

// XXX #include <elle/network/Bridge.hxx>

#endif
