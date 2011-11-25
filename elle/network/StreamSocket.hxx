//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu mar 18 21:23:33 2010]
//

#ifndef ELLE_NETWORK_STREAMSOCKET_HXX
#define ELLE_NETWORK_STREAMSOCKET_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/LocalSocket.hh>
#include <elle/network/TCPSocket.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace network
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sends a packet in an asynchronous manner.
    ///
    template <typename I>
    Status		StreamSocket::Send(const I		inputs,
					   const Event&		event)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeLocal:
	  {
	    LocalSocket*	socket = static_cast<LocalSocket*>(this);

	    return (socket->Send(inputs, event));
	  }
	case Socket::TypeTCP:
	  {
	    TCPSocket*		socket = static_cast<TCPSocket*>(this);

	    return (socket->Send(inputs, event));
	  }
	default:
	  {
	    escape("unknown or unhandled socket type '%u'\n",
		   this->type);
	  }
	}

      escape("invalid socket type");
    }

    ///
    /// this method receives a packet by blocking.
    ///
    template <typename O>
    Status		StreamSocket::Receive(const Event&	event,
					      O			outputs)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeLocal:
	  {
	    LocalSocket*	socket = static_cast<LocalSocket*>(this);

	    return (socket->Receive(event, outputs));
	  }
	case Socket::TypeTCP:
	  {
	    TCPSocket*		socket = static_cast<TCPSocket*>(this);

	    return (socket->Receive(event, outputs));
	  }
	default:
	  {
	    escape("unknown or unhandled socket type '%u'\n",
		   this->type);
	  }
	}

      leave();
    }

    ///
    /// this method sends and receives an associated response in a
    /// synchronous way.
    ///
    template <typename I,
	      typename O>
    Status		StreamSocket::Call(const I		inputs,
					   O			outputs)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeLocal:
	  {
	    LocalSocket*	socket = static_cast<LocalSocket*>(this);

	    return (socket->Call(inputs, outputs));
	  }
	case Socket::TypeTCP:
	  {
	    TCPSocket*		socket = static_cast<TCPSocket*>(this);

	    return (socket->Call(inputs, outputs));
	  }
	default:
	  {
	    escape("unknown or unhandled socket type '%u'\n",
		   this->type);
	  }
	}

      leave();
    }

    ///
    /// this method replies to the freshly received call.
    ///
    template <typename I>
    Status		StreamSocket::Reply(const I		inputs,
					    Session*		session)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeLocal:
	  {
	    LocalSocket*	socket = static_cast<LocalSocket*>(this);

	    return (socket->Reply(inputs, session));
	  }
	case Socket::TypeTCP:
	  {
	    TCPSocket*		socket = static_cast<TCPSocket*>(this);

	    return (socket->Reply(inputs, session));
	  }
	default:
	  {
	    escape("unknown or unhandled socket type '%u'\n",
		   this->type);
	  }
	}

      leave();
    }

  }
}

#endif

