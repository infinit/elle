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
// updated       julien quintard   [sat feb  6 05:06:13 2010]
//

#ifndef ELLE_NETWORK_NETWORK_HH
#define ELLE_NETWORK_NETWORK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/archive/Archive.hh>
#include <elle/misc/Misc.hh>

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
	virtual Status	Dispatch(Environment&,
				 Archive&) const = 0;
      };

      ///
      /// this class is the real implementation, taking care of turning
      /// the received archive into a live object.
      ///
      template <typename T>
      class Transformer:
	public Functionoid
      {
      public:
	//
	// constructors & destructors
	//
	Transformer();
	~Transformer();

	//
	// methods
	//
	Status		Dispatch(Environment&,
				 Archive&) const;

	//
	// interfaces
	//

	// dumpable
	Status		Dump(const Natural32 = 0) const;

	//
	// attributes
	//
	Callback*	callback;
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
      static Status	Register(Callback*);

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
