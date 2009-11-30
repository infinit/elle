//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/network/Socket.hxx
//
// created       julien quintard   [sat nov 28 15:17:33 2009]
// updated       julien quintard   [mon nov 30 13:18:42 2009]
//

#ifndef ELLE_NETWORK_SOCKET_HXX
#define ELLE_NETWORK_SOCKET_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Tag.hh>

namespace elle
{
  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this functionoid represents a callback.
    ///
    template <const Tag G, typename T, class C, typename M>
    class SocketCallback:
      public SocketDispatcher
    {
    public:
      //
      // constructors & destructors
      //
      SocketCallback(C*						object,
		     M						method)
      {
	this->object = object;
	this->method = method;
      }

      //
      // methods
      //
      Status		Trigger(const Location&			location,
				Archive&			archive)
      {
	T		packet;

	// extract the archive.
	if (packet.Extract(archive) == StatusError)
	  escape("unable to extract the packet's archive");

	// set the tag.
	packet.tag = G;

	// trigger the callback.
	(this->object->*method)(location, packet);

	leave();
      }

      //
      // attributes
      //
      C*		object;
      M			method;
    };

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method register a callback associated to a tag so that
    /// whenever a packet is received with this tag, the callback is triggered.
    ///
    template <const Tag G, typename T, class C>
    Status		Socket::Register(C* object,
					 void (C::*method)(const Location&,
							   const T&))
      {
	typedef void	(C::*M)(const Location&, const T&);

	// XXX check if already registered.

	// create a callback.
	SocketCallback<G, T, C, M>* functionoid =
	  new SocketCallback<G, T, C, M>(object, method);

	// insert the callback in the container.
	std::pair<Socket::Iterator, Boolean> value =
	  this->callbacks.insert(std::pair<Tag,
				 SocketDispatcher*>(G, functionoid));

	// check if the insertion was successful.
	if (value.second == false)
	  escape("unable to inser the callback into the container");

	leave();
      }

  }
}

#endif
