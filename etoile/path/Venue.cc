//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [sat aug  8 17:51:22 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/path/Venue.hh>

namespace etoile
{
  namespace path
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant is used a null definition.
    ///
    const Venue                 Venue::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Venue::Venue()
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method records the location.
    ///
    elle::Status        Venue::Record(const nucleus::Location&  location)
    {
      ;

      // store the location in the container.
      this->elements.push_back(location);

      return elle::StatusOk;
    }

    ///
    /// this method records the next step of the venue.
    ///
    elle::Status        Venue::Record(const nucleus::Address&   address,
                                      const nucleus::Version&   version)
    {
      nucleus::Location location;

      ;

      // create the location.
      if (location.Create(address, version) == elle::StatusError)
        escape("unable to create the location");

      // record the location.
      if (this->Record(location) == elle::StatusError)
        escape("unable to record the location");

      return elle::StatusOk;
    }

    ///
    /// this method clears the venue's content.
    ///
    elle::Status        Venue::Clear()
    {
      ;

      // clear the container.
      this->elements.clear();

      return elle::StatusOk;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Venue::operator==(const Venue&          element) const
    {
      Venue::Scoutor    s;
      Venue::Scoutor    t;

      ;

      // check the address as this may actually be the same object.
      if (this == &element)
        return elle::StatusTrue;

      // compare the size.
      if (this->elements.size() != element.elements.size())
        return elle::StatusFalse;

      // for every element.
      for (s = this->elements.begin(), t = element.elements.begin();
           s != this->elements.end();
           s++, t++)
        if (*s != *t)
          return elle::StatusFalse;

      return elle::StatusTrue;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Venue, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a venue.
    ///
    elle::Status        Venue::Dump(const elle::Natural32       margin) const
    {
      elle::String      alignment(margin, ' ');
      Venue::Scoutor    scoutor;

      ;

      std::cout << alignment << "[Venue] " << this
                << " #" << std::dec
                << this->elements.size() << std::endl;

      // for every element.
      for (scoutor = this->elements.begin();
           scoutor != this->elements.end();
           scoutor++)
        {
          // dump the location.
          if (scoutor->Dump(margin + 2) == elle::StatusError)
            escape("unable to dump the address");
        }

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the object.
    ///
    elle::Status        Venue::Serialize(elle::Archive&         archive) const
    {
      Venue::Scoutor    scoutor;
      elle::Natural32   size;

      ;

      // retrieve the container size.
      size = this->elements.size();

      // serialize the size.
      if (archive.Serialize(size) == elle::StatusError)
        escape("unable to serialize the size");

      // for every element.
      for (scoutor = this->elements.begin();
           scoutor != this->elements.end();
           scoutor++)
        {
          // serialize the location.
          if (archive.Serialize(*scoutor) == elle::StatusError)
            escape("unable to serialize the location");
        }

      return elle::StatusOk;
    }

    ///
    /// this method extracts the object.
    ///
    elle::Status        Venue::Extract(elle::Archive&           archive)
    {
      elle::Natural32   size;
      elle::Natural32   i;

      ;

      // extract the size.
      if (archive.Extract(size) == elle::StatusError)
        escape("unable to extract the size");

      // for every element.
      for (i = 0; i < size; i++)
        {
          nucleus::Location     location;

          // extract the location.
          if (archive.Extract(location) == elle::StatusError)
            escape("unable to extract the location");

          // store the location in the elements.
          this->elements.push_back(location);
        }

      return elle::StatusOk;
    }

  }
}
