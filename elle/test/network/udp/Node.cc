//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri nov 27 22:04:36 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/udp/Node.hh>

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
    Status              Node::Setup(const String&               name,
                                    const String&               host,
                                    const Port                  port)
    {
      // set the attributes.
      this->name = name;
      this->host = host;
      this->port = port;

      return StatusOk;
    }

    ///
    /// this is the thread entry locus.
    ///
    Status              Node::Run()
    {
      Host              local;
      Locus             remote;

      // create an host.
      if (local.Create(this->host) == StatusError)
        escape("unable to create an host");

      // create the socket.
      if (this->socket.Create() == StatusError)
        escape("unable to create the socket");

      std::cout << "[port] " << this->socket.port << std::endl;

      // register the probe message.
      if (Network::Register(
            Procedure<TagProbe>(
              Callback<>::Infer(&Node::Handle, this))) == StatusError)
        escape("unable to register the probe message");

      // create the table.
      if (this->table.Create(this) == StatusError)
        escape("unable to create the table");

      // create an locus.
      if (remote.Create(local, this->port) == StatusError)
        escape("unable to create a location");

      // probe the first peer.
      if (this->socket.Send(remote,
                            Inputs<TagProbe>(this->name,
                                             this->table)) == StatusError)
        escape("unable to send the probe");

      return StatusOk;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this method handles probe packets.
    ///
    Status              Node::Handle(const String&              name,
                                     const Table&               table)
    {
      Session*          session;

      // retrieve the current session.
      if (Session::Instance(session) == StatusError)
        escape("unable to retrieve the instance of the current session");

      // simply add the sender to the table.
      if (this->table.Update(session->locus, name) == StatusError)
        escape("unable to add the new neighbour");

      // refresh the sender.
      if (this->table.Refresh(session->locus) == StatusError)
        escape("unable to refresh the sender's entry");

      // merge the table with the received one.
      if (this->table.Merge(table) == StatusError)
        escape("unable to update the table");

      return StatusOk;
    }

  }
}
