//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/Node.cc
//
// created       julien quintard   [fri nov 27 22:04:36 2009]
// updated       julien quintard   [mon nov 30 15:01:19 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/Node.hh>

namespace elle
{
  namespace test
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the node.
    ///
    Status		Node::Start(const String&		name,
				    const String&		host,
				    const Natural16		port)
    {
      Address		peer;
      Probe		probe;

      // set the node's name.
      this->name = name;

      // create an address.
      if (peer.Create(host) == StatusError)
	escape("unable to create an address");

      // create the socket.
      if (this->socket.Create() == StatusError)
	escape("unable to create the socket");

      std::cout << "[port] " << socket.port << std::endl;

      // register the probe callback.
      if (this->socket.Register<TagProbe, Probe>(this, &Node::Handle) ==
	  StatusError)
	escape("unable to register the probe packet");

      // create a new timer.
      this->timer = new ::QTimer(this);

      // connect the timeout signal to the refresh slot.
      if (this->connect(this->timer, SIGNAL(timeout()),
			this, SLOT(Refresh())) == false)
	escape("unable to connect the timeout signal");

      // start the timer.
      this->timer->start(10000);

      // create a probe.
      if (probe.Create(this->name) == StatusError)
	escape("unable to create a probe message");

      // probe the peer.
      if (socket.Send(Location(peer, port), probe) == StatusError)
	escape("unable to send the probe");

      leave();
    }

    ///
    /// this method adds a new neighbour.
    ///
    Status		Node::Add(const Location&		location,
				  const String&			name)
    {
      Node::Iterator	iterator;

      // try to locate a previous entry.
      if (this->Locate(location, iterator) == StatusOk)
	{
	  // update the name.
	  (*iterator)->name = name;

	  // re-set the timer.
	  (*iterator)->timer.stop();
	  (*iterator)->timer.start(20000);
	}
      else
	{
	  Neighbour*	neighbour;

	  // create a new neighbour.
	  neighbour = new Neighbour;

	  // assign the attributes.
	  neighbour->node = this;

	  neighbour->location = location;
	  neighbour->name = name;

	  // connect the timeout.
	  if (neighbour->connect(&neighbour->timer, SIGNAL(timeout()),
				 neighbour, SLOT(Discard())) == false)
	    escape("unable to connect the timeout signal");

	  // start the timer.
	  neighbour->timer.start(20000);

	  // add the neighbour to the list.
	  this->container.push_back(neighbour);
	}

      leave();
    }

    ///
    /// this method removes a neighbour.
    ///
    Status		Node::Remove(const Location&		location)
    {
      Node::Iterator	iterator;

      // try to locate a previous entry.
      if (this->Locate(location, iterator) == StatusError)
	escape("unable to locate this neighbour");

      // remove the element from the list.
      this->container.erase(iterator);

      leave();
    }

    ///
    /// this method updates a existing neighbour.
    ///
    Status		Node::Update(const Location&		location,
				     const String&		name)
    {
      Node::Iterator	iterator;

      // try to locate a previous entry.
      if (this->Locate(location, iterator) == StatusError)
	escape("unable to locate this neighbour");

      // update the name.
      (*iterator)->name = name;

      // re-set the timer.
      (*iterator)->timer.stop();
      (*iterator)->timer.start(20000);

      leave();
    }

    ///
    /// this method locates a neighbour in the list.
    ///
    Status		Node::Locate(const Location&		location,
				     Node::Iterator&		iterator)
    {
      // iterator over the container.
      for (iterator = this->container.begin();
	   iterator != this->container.end();
	   iterator++)
	{
	  // if the location is found, return.
	  if ((*iterator)->location == location)
	    leave();
	}

      escape("unable to locate the given neighbour");
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method handles probe packets.
    ///
    void		Node::Handle(const Location&		location,
				     const Probe&		probe)
    {
      // simply add the sender to the list of neighbours.
      if (this->Add(location, probe.name) == StatusError)
	alert("unable to add the new neighbour");
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is called whenever the state needs refreshing.
    ///
    void		Node::Refresh()
    {
      //
      // first, display the current state.
      //
      Node::Scoutor	scoutor;

      std::cout << "[State]" << std::endl;

      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  std::cout << "  [Neighbour] " << (*scoutor)->name << std::endl;

	  if ((*scoutor)->location.Dump(4) == StatusError)
	    alert("unable to dump the neighbour's location");
	}

      //
      // initiate the refreshing process.
      //
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Probe		probe;

	  // create a probe.
	  if (probe.Create(this->name) == StatusError)
	    alert("unable to create a probe message");

	  // send a probe message.
	  if (this->socket.Send((*scoutor)->location, probe) == StatusError)
	    alert("unable to send a probe");
	}
    }

    ///
    /// XXX
    ///
    void		Neighbour::Discard()
    {
      // discard the current neighbour as it has not been refreshed in time.
      if (this->node->Remove(this->location) == StatusError)
	alert("unable to remove the current neighbour");
    }

  }
}
