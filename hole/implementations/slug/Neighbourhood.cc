//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [thu aug 25 12:19:28 2011]
//

//
// ---------- includes --------------------------------------------------------
//

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
      elle::Status      Neighbourhood::Add(const elle::Locus&   locus,
                                           Host*                host)
      {
        std::pair<Neighbourhood::Iterator, elle::Boolean>       result;

        enter();

        // insert the host in the container.
        result = this->container.insert(Neighbourhood::Value(locus, host));

        // check if the insertion was successful.
        if (result.second == false)
          escape("unable to insert the host in the container");

        leave();
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbourhood::Exist(const elle::Locus& locus) const
      {
        Neighbourhood::Scoutor  scoutor;

        enter();

        // try to locate the locus.
        if (this->Locate(locus, scoutor) == elle::StatusTrue)
          true();

        false();
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbourhood::Retrieve(const elle::Locus& locus,
                                                Host*&          host) const
      {
        Neighbourhood::Scoutor  scoutor;

        enter();

        // try to locate the locus.
        if (this->Locate(locus, scoutor) == elle::StatusFalse)
          escape("unable to locate the given locus");

        // return the associated host.
        host = scoutor->second;

        leave();
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbourhood::Remove(const elle::Locus& locus)
      {
        Neighbourhood::Iterator iterator;

        enter();

        // try to locate the locus.
        if (this->Locate(locus, iterator) == elle::StatusFalse)
          escape("unable to locate the given locus");

        // erase the iterator.
        this->container.erase(iterator);

        leave();
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbourhood::Locate(const elle::Locus& locus,
                                              Scoutor&          scoutor) const
      {
        Neighbourhood::Scoutor  s;

        enter();

        // try to locate the host.
        if ((s = this->container.find(locus)) != this->container.end())
          {
            // return the scoutor.
            scoutor = s;

            true();
          }

        false();
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbourhood::Locate(const elle::Locus& locus,
                                              Iterator&         iterator)
      {
        Neighbourhood::Iterator i;

        enter();

        // try to locate the host.
        if ((i = this->container.find(locus)) != this->container.end())
          {
            // return the iterator.
            iterator = i;

            true();
          }

        false();
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

        enter();

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
            if (scoutor->first.Dump(margin + 4) == elle::StatusError)
              escape("unable to dump the locus");

            // dump the host.
            if (scoutor->second->Dump(margin + 4) == elle::StatusError)
              escape("unable to dump the host");
          }

        leave();
      }

    }
  }
}
