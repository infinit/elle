//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Channel.hxx
//
// created       julien quintard   [thu mar 18 21:23:33 2010]
// updated       julien quintard   [wed may 25 15:30:26 2011]
//

#ifndef ELLE_NETWORK_CHANNEL_HXX
#define ELLE_NETWORK_CHANNEL_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Door.hh>
#include <elle/network/Gate.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace network
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// these classes need to be forward declared.
    ///
    class Door;
    class Gate;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sends a packet in an asynchronous manner.
    ///
    template <typename I>
    Status		Channel::Send(const I			inputs,
				      const Event&		event)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeDoor:
	  {
	    Door*	channel = static_cast<Door*>(this);

	    return (channel->Send(inputs, event));
	  }
	case Socket::TypeGate:
	  {
	    Gate*	channel = static_cast<Gate*>(this);

	    return (channel->Send(inputs, event));
	  }
	}

      escape("invalid socket type");
    }

    ///
    /// this method receives a packet by blocking.
    ///
    template <typename O>
    Status		Channel::Receive(const Event&		event,
					 O			outputs)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeDoor:
	  {
	    Door*	channel = static_cast<Door*>(this);

	    return (channel->Receive(event, outputs));
	  }
	case Socket::TypeGate:
	  {
	    Gate*	channel = static_cast<Gate*>(this);

	    return (channel->Receive(event, outputs));
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
    Status		Channel::Call(const I			inputs,
				      O				outputs)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeDoor:
	  {
	    Door*	channel = static_cast<Door*>(this);

	    return (channel->Call(inputs, outputs));
	  }
	case Socket::TypeGate:
	  {
	    Gate*	channel = static_cast<Gate*>(this);

	    return (channel->Call(inputs, outputs));
	  }
	}

      leave();
    }

    ///
    /// this method replies to the freshly received call.
    ///
    template <typename I>
    Status		Channel::Reply(const I			inputs)
    {
      enter();

      switch (this->type)
	{
	case Socket::TypeDoor:
	  {
	    Door*	channel = static_cast<Door*>(this);

	    return (channel->Reply(inputs));
	  }
	case Socket::TypeGate:
	  {
	    Gate*	channel = static_cast<Gate*>(this);

	    return (channel->Reply(inputs));
	  }
	}

      leave();
    }

  }
}

#endif

