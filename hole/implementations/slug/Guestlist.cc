#include <hole/implementations/slug/Guestlist.hh>

#include <elle/idiom/Open.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Guestlist::~Guestlist()
      {
        Guestlist::Scoutor      scoutor;

        // go through the entries.
        for (scoutor = this->container.begin();
             scoutor != this->container.end();
             scoutor++)
          {
            // delete the neighbour.
            delete scoutor->second;
          }

        // clear the container.
        this->container.clear();
      }

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status      Guestlist::Add(elle::network::TCPSocket*         socket,
                                       Host*                    host)
      {
        std::pair<Guestlist::Iterator, elle::Boolean>   result;

        // insert the host in the container.
        result = this->container.insert(Guestlist::Value(socket, host));

        // check if the insertion was successful.
        if (result.second == false)
          escape("unable to insert the host in the container");

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Guestlist::Exist(elle::network::TCPSocket*       socket) const
      {
        Guestlist::Scoutor      scoutor;

        // try to locate the socket.
        if (this->Locate(socket, scoutor) == elle::Status::True)
          return elle::Status::True;

        return elle::Status::False;
      }

      ///
      /// XXX
      ///
      elle::Status      Guestlist::Retrieve(elle::network::TCPSocket*    socket,
                                            Host*&              host)
        const
      {
        Guestlist::Scoutor      scoutor;

        // try to locate the socket.
        if (this->Locate(socket, scoutor) == elle::Status::False)
          escape("unable to locate the given socket");

        // return the associated host.
        host = scoutor->second;

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Guestlist::Remove(elle::network::TCPSocket*      socket)
      {
        Guestlist::Iterator     iterator;

        // try to locate the socket.
        if (this->Locate(socket, iterator) == elle::Status::False)
          escape("unable to locate the given socket");

        // erase the iterator.
        this->container.erase(iterator);

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Guestlist::Locate(elle::network::TCPSocket*      socket,
                                          Scoutor&              scoutor) const
      {
        Guestlist::Scoutor      s;

        // try to locate the host.
        if ((s = this->container.find(socket)) != this->container.end())
          {
            // return the scoutor.
            scoutor = s;

            return elle::Status::True;
          }

        return elle::Status::False;
      }

      ///
      /// XXX
      ///
      elle::Status      Guestlist::Locate(elle::network::TCPSocket*      socket,
                                          Iterator&             iterator)
      {
        Guestlist::Iterator     i;

        // try to locate the host.
        if ((i = this->container.find(socket)) != this->container.end())
          {
            // return the iterator.
            iterator = i;

            return elle::Status::True;
          }

        return elle::Status::False;
      }

//
// ---------- dumpable --------------------------------------------------------
//

      ///
      /// this function dumps a routing table object.
      ///
      elle::Status      Guestlist::Dump(elle::Natural32         margin) const
      {
        elle::String            alignment(margin, ' ');
        Guestlist::Scoutor      scoutor;

        // display the name.
        std::cout << alignment << "[Guestlist]" << std::endl;

        // go through the entries.
        for (scoutor = this->container.begin();
             scoutor != this->container.end();
             scoutor++)
          {
            std::cout << alignment << elle::io::Dumpable::Shift
                      << "[Guest]" << std::endl;

            // dump the socket's address.
            std::cout << alignment << elle::io::Dumpable::Shift
                      << elle::io::Dumpable::Shift
                      << "[TCPSocket] " << scoutor->first << std::endl;

            // dump the host.
            if (scoutor->second->Dump(margin + 4) == elle::Status::Error)
              escape("unable to dump the host");
          }

        return elle::Status::Ok;
      }

    }
  }
}
