//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/slot/Node.cc
//
// created       julien quintard   [fri nov 27 22:04:36 2009]
// updated       julien quintard   [wed mar 17 17:25:43 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/slot/Node.hh>

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
    Status		Node::Setup(const String&		name,
				    const String&		host,
				    const Port			port)
    {
      enter();

      // set the attributes.
      this->name = name;
      this->host = host;
      this->port = port;

      leave();
    }

    ///
    /// this is the thread entry point.
    ///
    Status		Node::Run()
    {
      Method<String, Table>	handle(this, &Node::Handle);
      Host			local;
      Address			remote;
      
      enter();

      // create an host.
      if (local.Create(this->host) == StatusError)
	escape("unable to create an host");

      // create the slot.
      if (this->slot.Create() == StatusError)
	escape("unable to create the slot");

      std::cout << "[port] " << this->slot.port << std::endl;

      // register the probe message.
      if (Network::Register<TagProbe>(handle) == StatusError)
	escape("unable to register the probe message");

      // create the table.
      if (this->table.Create(this) == StatusError)
	escape("unable to create the table");

      // create an address.
      if (remote.Create(local, this->port) == StatusError)
	escape("unable to create a location");

      // probe the first peer.
      if (this->slot.Send(remote,
			  Inputs<TagProbe>(this->name,
					   this->table)) == StatusError)
	escape("unable to send the probe");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method handles probe packets.
    ///
    Status		Node::Handle(String&			name,
				     Table&			table)
    {
      enter();

      // simply add the sender to the table.
      if (this->table.Update(context->address, name) == StatusError)
	escape("unable to add the new neighbour");

      // refresh the sender.
      if (this->table.Refresh(context->address) == StatusError)
	escape("unable to refresh the sender's entry");

      // merge the table with the received one.
      if (this->table.Merge(table) == StatusError)
	escape("unable to update the table");

      leave();
    }

  }
}
