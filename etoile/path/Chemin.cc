//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [tue jun 14 22:25:57 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Chemin.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines a null chemin.
    ///
    const Chemin                        Chemin::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Chemin::Chemin()
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a chemin.
    ///
    elle::Status        Chemin::Create(const Route&             route,
                                       const Venue&             venue,
                                       const nucleus::Size      size)
    {
      // clear the route because the chemin may have been used for
      // something else before.
      if (this->route.Clear() == elle::Status::Error)
        escape("unable to clear the route");

      // do the same for the venue.
      if (this->venue.Clear() == elle::Status::Error)
        escape("unable to clear the venue");

      //
      // import the route.
      //
      {
        Route::Scoutor  scoutor;
        nucleus::Size   i;

        // go through the route.
        for (scoutor = route.elements.begin(), i = 0;
             (scoutor != route.elements.end()) && (i < size);
             scoutor++, i++)
          {
            // record the route element.
            this->route.elements.push_back(*scoutor);
          }
      }

      //
      // import the venue.
      //
      {
        Venue::Scoutor  scoutor;
        nucleus::Size   i;

        // go through the venue.
        for (scoutor = venue.elements.begin(), i = 0;
             (scoutor != venue.elements.end()) && (i < size);
             scoutor++, i++)
          {
            // record the venue element.
            this->venue.elements.push_back(*scoutor);
          }
      }

      return elle::Status::Ok;
    }

    ///
    /// this method returns true if the current chemin derives the
    /// given base.
    ///
    /// let us imagine two chemins A and B. A is said to derive B
    /// if both its route and venue are included in B, plus potential
    /// other entries. in other words A's route and venue can be equal
    /// or longer.
    ///
    elle::Status        Chemin::Derives(const Chemin&            base) const
    {
      if ((this->route.Derives(base.route) == elle::Status::True) &&
          (this->venue.Derives(base.venue) == elle::Status::True))
        return (elle::Status::True);

      return (elle::Status::False);
    }

    ///
    /// this method generates a Location based on the route and venue.
    ///
    elle::Status        Chemin::Locate(nucleus::Location&       location) const
    {
      // check the size of the venue.
      if (this->venue.elements.size() == 0)
        escape("the venue seems to be empty");

      // set the location's attributes according to the venue last element.
      location.address =
        this->venue.elements[this->venue.elements.size() - 1].address;
      location.version =
        this->venue.elements[this->venue.elements.size() - 1].version;

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Chemin::operator==(const Chemin&        element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the attributes.
      if ((this->route != element.route) ||
          (this->venue != element.venue))
        return false;

      return true;
    }

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Chemin::operator<(const Chemin&         element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return false;

      // compare the route only.
      if (this->route < element.route)
        return true;

      return false;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Chemin, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a chemin.
    ///
    elle::Status        Chemin::Dump(const elle::Natural32      margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Chemin] " << this << std::endl;

      // dump the route.
      if (this->route.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the route");

      // dump the venue.
      if (this->venue.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the venue");

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the object.
    ///
    //elle::Status        Chemin::Serialize(elle::Archive&        archive) const
    //{
    //  // serialize the attributes.
    //  if (archive.Serialize(this->route,
    //                        this->venue) == elle::Status::Error)
    //    escape("unable to serialize the attribtues");

    //  return elle::Status::Ok;
    //}

    /////
    ///// this method extracts the object.
    /////
    //elle::Status        Chemin::Extract(elle::Archive&  archive)
    //{
    //  // extract the attributes.
    //  if (archive.Extract(this->route,
    //                      this->venue) == elle::Status::Error)
    //    escape("unable to extract the attributes");

    //  return elle::Status::Ok;
    //}

  }
}
