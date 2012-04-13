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

#include <hole/implementations/cirkle/Neighbourhood.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- constructors & destructors --------------------------------------
//

      ///
      /// XXX
      ///
      Neighbourhood::~Neighbourhood()
      {
        Neighbourhood::Scoutor  scoutor;

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
      elle::Status      Neighbourhood::Add(const elle::Locus&   locus,
                                           Neighbour*           neighbour)
      {
        std::pair<Neighbourhood::Iterator, elle::Boolean>       result;

        // insert the neighbour in the container.
        result = this->container.insert(std::pair<const elle::Locus,
                                                  Neighbour*>(locus,
                                                              neighbour));

        // check if the insertion was successful.
        if (result.second == false)
          escape("unable to insert the neighbour in the container");

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbourhood::Exist(const elle::Locus& locus)
      {
        // try to locate the locus.
        if (this->Locate(locus) == elle::Status::True)
          return elle::Status::True;

        return elle::Status::False;
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbourhood::Retrieve(const elle::Locus& locus,
                                                Neighbour*&     neighbour)
      {
        Neighbourhood::Iterator iterator;

        // try to locate the locus.
        if (this->Locate(locus, &iterator) == elle::Status::False)
          escape("unable to locate the given locus");

        // return the associated neighbour.
        neighbour = iterator->second;

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbourhood::Remove(const elle::Locus& locus)
      {
        Neighbourhood::Iterator iterator;

        // try to locate the locus.
        if (this->Locate(locus, &iterator) == elle::Status::False)
          escape("unable to locate the given locus");

        // erase the iterator.
        this->container.erase(iterator);

        return elle::Status::Ok;
      }

      ///
      /// XXX
      ///
      elle::Status      Neighbourhood::Locate(const elle::Locus& locus,
                                              Iterator*         iterator)
      {
        Neighbourhood::Iterator i;

        // try to locate the neighbour.
        if ((i = this->container.find(locus)) != this->container.end())
          {
            if (iterator != NULL)
              *iterator = i;

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
            // dump the neighbour.
            if (scoutor->second->Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the neighbour");
          }

        return elle::Status::Ok;
      }

    }
  }
}
