//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Door.hxx
//
// created       julien quintard   [tue feb 23 13:44:55 2010]
// updated       julien quintard   [tue feb 23 13:46:27 2010]
//

#ifndef ELLE_NETWORK_DOOR_HXX
#define ELLE_NETWORK_DOOR_HXX

namespace elle
{
  namespace network
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// these methods convert a set of values into a local packet and sends it
    /// to the given address.
    ///
    /// note that multiple template specialisations are provided here in
    /// order to make sure that the given arguments fit the types of the
    /// message they are associated with.
    ///

    ///
    /// zero-argument in, zero-argument out.
    ///
    template <const Tag G, typename... T>
    Status		Door::Send(const T&...			parameters)
    {
      Packet		packet;
      Header		header;
      Archive		archive;

      // create an archive for the parameters.
      if (archive.Create() == StatusError)
	escape("unable to create the archive");

      // serialize the parameters.
      if (archive.Serialize(parameters...) == StatusError)
	escape("unable to serialize the parameters");

      // create the header.
      if (header.Create(G, archive.size) == StatusError)
	escape("unable to create the header");

      // prepare the packet.
      if (packet.Create() == StatusError)
	escape("unable to create the packet");

      // serialize the message i.e the header followed by the parameters.
      if (packet.Serialize(header, archive) == StatusError)
	escape("unable to serialize the message");

      // push the packet to the socket.
      if (this->socket->write((const char*)packet.contents,
			      packet.size) == StatusError)
	escape("unable to write the packet");

      // flush to start sending data immediately.
      this->socket->flush();

      leave();
    }

  }
}

#endif
