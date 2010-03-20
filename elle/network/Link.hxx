//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Link.hxx
//
// created       julien quintard   [thu mar 18 21:23:33 2010]
// updated       julien quintard   [thu mar 18 23:30:22 2010]
//

///
/// XXX
///
#include <elle/network/Door.hh>
#include <elle/network/Gate.hh>

#ifndef ELLE_NETWORK_LINK_HXX
#define ELLE_NETWORK_LINK_HXX

//
// ---------- includes --------------------------------------------------------
//

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
    Status		Link::Send(const I			inputs,
				   const Identifier&		identifier)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeDoor:
	  {
	    Door*	link = static_cast<Door*>(this);

	    return (link->Send(inputs, identifier));
	  }
	case Socket::TypeGate:
	  {
	    Gate*	link = static_cast<Gate*>(this);

	    return (link->Send(inputs, identifier));
	  }
	}

      escape("invalid socket type");
    }

    ///
    /// this method sends a packet in a synchronous way.
    ///
    template <typename I>
    Status		Link::Transmit(const I			inputs,
				       const Identifier&	identifier)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeDoor:
	  {
	    Door*	link = static_cast<Door*>(this);

	    return (link->Transmit(inputs, identifier));
	  }
	case Socket::TypeGate:
	  {
	    Gate*	link = static_cast<Gate*>(this);

	    return (link->Transmit(inputs, identifier));
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
    Status		Link::Call(const I			inputs,
				   O				outputs)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeDoor:
	  {
	    Door*	link = static_cast<Door*>(this);

	    return (link->Call(inputs, outputs));
	  }
	case Socket::TypeGate:
	  {
	    Gate*	link = static_cast<Gate*>(this);

	    return (link->Call(inputs, outputs));
	  }
	}

      leave();
    }

    ///
    /// this method receives a packet by blocking.
    ///
    template <typename O>
    Status		Link::Receive(const Identifier&		identifier,
				      O				outputs)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeDoor:
	  {
	    Door*	link = static_cast<Door*>(this);

	    return (link->Receive(identifier, outputs));
	  }
	case Socket::TypeGate:
	  {
	    Gate*	link = static_cast<Gate*>(this);

	    return (link->Receive(identifier, outputs));
	  }
	}

      leave();
    }

    ///
    /// this method replies to the freshly received call.
    ///
    template <typename I>
    Status		Link::Reply(const I			inputs)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeDoor:
	  {
	    Door*	link = static_cast<Door*>(this);

	    return (link->Reply(inputs));
	  }
	case Socket::TypeGate:
	  {
	    Gate*	link = static_cast<Gate*>(this);

	    return (link->Reply(inputs));
	  }
	}

      leave();
    }

  }
}

#endif

