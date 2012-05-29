
#include <elle/network/Locus.hh>

#include <hole/implementations/slug/Neighbourhood.hh>

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
      Neighbourhood::~Neighbourhood()
      {
        // clear the container.
        this->container.clear();
      }

//
// ---------- methods ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      elle::Status      Neighbourhood::Add(const elle::network::Locus&   locus,
                                           Host*                host)
      {
        std::pair<Neighbourhood::Iterator, elle::Boolean>       result;

        // insert the host in the container.
        result = this->container.insert(Neighbourhood::Value(locus, host));

        // check if the insertion was successful.
        if (result.second == false)
          escape("unable to insert the host in the container");

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbourhood::Exist(const elle::network::Locus& locus) const
      {
        Neighbourhood::Scoutor  scoutor;

        // try to locate the locus.
        if (this->Locate(locus, scoutor) == elle::Status::True)
          return elle::Status::True;

        return elle::Status::False;
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbourhood::Retrieve(const elle::network::Locus& locus,
                                                Host*&          host) const
      {
        Neighbourhood::Scoutor  scoutor;

        // try to locate the locus.
        if (this->Locate(locus, scoutor) == elle::Status::False)
          escape("unable to locate the given locus");

        // return the associated host.
        host = scoutor->second;

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbourhood::Remove(const elle::network::Locus& locus)
      {
        Neighbourhood::Iterator iterator;

        // try to locate the locus.
        if (this->Locate(locus, iterator) == elle::Status::False)
          escape("unable to locate the given locus");

        // erase the iterator.
        this->container.erase(iterator);

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbourhood::Locate(const elle::network::Locus& locus,
                                              Scoutor&          scoutor) const
      {
        Neighbourhood::Scoutor  s;

        // try to locate the host.
        if ((s = this->container.find(locus)) != this->container.end())
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
      elle::Status      Neighbourhood::Locate(const elle::network::Locus& locus,
                                              Iterator&         iterator)
      {
        Neighbourhood::Iterator i;

        // try to locate the host.
        if ((i = this->container.find(locus)) != this->container.end())
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
      elle::Status      Neighbourhood::Dump(elle::Natural32     margin) const
      {
        elle::String            alignment(margin, ' ');
        Neighbourhood::Scoutor  scoutor;

        // display the name.
        std::cout << alignment << "[Neighbourhood]" << std::endl;

        // go through the entries.
        for (scoutor = this->container.begin();
             scoutor != this->container.end();
             scoutor++)
          {
            std::cout << alignment << elle::Dumpable::Shift
                      << "[Neighbour]" << std::endl;

            // dump the locus.
            if (scoutor->first.Dump(margin + 4) == elle::Status::Error)
              escape("unable to dump the locus");

            // dump the host.
            if (scoutor->second->Dump(margin + 4) == elle::Status::Error)
              escape("unable to dump the host");
          }

        return elle::Status::Ok;
      }

    }
  }
}
