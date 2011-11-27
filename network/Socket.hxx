//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun jul 17 15:39:02 2011]
//

#ifndef ELLE_NETWORK_SOCKET_HXX
#define ELLE_NETWORK_SOCKET_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/AbstractSocket.hh>
#include <elle/network/UDPSocket.hh>

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
    Status		Socket::Send(const I			inputs,
				     const Event&		event,
				     const Locus&		locus)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeLocal:
	case Socket::TypeTCP:
	  {
	    AbstractSocket*	socket = static_cast<AbstractSocket*>(this);

	    return (socket->Send(inputs, event));
	  }
	case Socket::TypeUDP:
	  {
	    UDPSocket*		socket = static_cast<UDPSocket*>(this);

	    return (socket->Send(locus, inputs, event));
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
    Status		Socket::Receive(const Event&		event,
					O			outputs)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeLocal:
	case Socket::TypeTCP:
	  {
	    AbstractSocket*	socket = static_cast<AbstractSocket*>(this);

	    return (socket->Receive(event, outputs));
	  }
	case Socket::TypeUDP:
	  {
	    UDPSocket*		socket = static_cast<UDPSocket*>(this);

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
    Status		Socket::Call(const I			inputs,
				     O				outputs,
				     const Locus&		locus)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeLocal:
	case Socket::TypeTCP:
	  {
	    AbstractSocket*	socket = static_cast<AbstractSocket*>(this);

	    return (socket->Call(inputs, outputs));
	  }
	case Socket::TypeUDP:
	  {
	    UDPSocket*		socket = static_cast<UDPSocket*>(this);

	    return (socket->Call(locus, inputs, outputs));
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
    Status		Socket::Reply(const I			inputs,
				      Session*			session)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeLocal:
	case Socket::TypeTCP:
	  {
	    AbstractSocket*	socket = static_cast<AbstractSocket*>(this);

	    return (socket->Reply(inputs, session));
	  }
	case Socket::TypeUDP:
	  {
	    UDPSocket*		socket = static_cast<UDPSocket*>(this);

	    return (socket->Reply(inputs, session));
	  }
	default:
	  {
	    escape("unknown or unhandled socket type '%u'",
		   this->type);
	  }
	}

      leave();
    }

  }
}

#endif
