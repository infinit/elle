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
// updated       julien quintard   [mon feb 22 14:39:14 2010]
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
#include <elle/network/Packet.hh>

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

    /// \todo Network::Dispatch devrait retourner un offset correspondant
    /// aux donnees ne faisant pas partie du message (mais du message
    /// suivant) qu'on mettrait dans un buffer.
    ///
    /// de plus, il faudrait que chaque message ait un CRC et une taille
    /// de donnee. le slot Read() mettrait donc les donnees dans un buffer
    /// et si les donnees sont au complet, alors on extrait le message sinon
    /// on attend.
    ///
    /// a noter que la taille devrait rester dans un interval pour eviter
    /// des problemes.
    ///
    /// de plus Message devrait serializer "Message" au debut cf Message.h
    ///
    /// pour finir, lorsque les donnees sont envoyees, la taille + CRC
    /// devrait etre mis dans un Header qui precederait chaque Message
    /// et c'est ca qui devrait vraiment etre envoye.

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
      /// this implementation takes an archive, extracts a number of
      /// arguments depending on the selectionoid and triggers the callback.
      ///
      template <const Natural32, class P>
      class Selectionoid:
	public Functionoid
      {
      public:
	//
	// constructors & destructors
	//
	Selectionoid();
	~Selectionoid();

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
      template <const Tag G, class P>
      static Status	Register(Callback&);

      static Status	Dispatch(Environment&,
				 Packet&);

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
#include <elle/network/Packet.hh>
#include <elle/network/Slot.hh>
#include <elle/network/Bridge.hh>
#include <elle/network/Door.hh>
#include <elle/network/Parameters.hh>

#endif
