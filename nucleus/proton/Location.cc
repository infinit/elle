//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [wed jun 15 20:17:21 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/Location.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant represents a null location.
    ///
    const Location              Location::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Location::Location()
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a location.
    ///
    elle::Status        Location::Create(const Address&         address,
                                         const Version&         version)
    {
      // set the attributes.
      this->address = address;
      this->version = version;

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method checks if two objects match.
    ///
    elle::Boolean       Location::operator==(const Location&    element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the attributes.
      if ((this->address != element.address) ||
          (this->version != element.version))
        return false;

      return true;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Location, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the location's internals.
    ///
    elle::Status        Location::Dump(const elle::Natural32    margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Location]" << std::endl;

      // dump the address.
      if (this->address.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the address");

      // dump the version.
      if (this->version.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the version");

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the location attributes.
    ///
    //elle::Status        Location::Serialize(elle::Archive&      archive) const
    //{
    //  // serialize the attributes.
    //  if (archive.Serialize(this->address,
    //                        this->version) == elle::Status::Error)
    //    escape("unable to serialize the location's attributes");

    //  return elle::Status::Ok;
    //}

    /////
    ///// this method extracts the attributes.
    /////
    //elle::Status        Location::Extract(elle::Archive&        archive)
    //{
    //  // extracts the attributes.
    //  if (archive.Extract(this->address,
    //                      this->version) == elle::Status::Error)
    //    escape("unable to extract the location's attributes");

    //  return elle::Status::Ok;
    //}

  }
}
