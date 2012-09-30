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
                                         const Revision&         revision)
    {
      // set the attributes.
      this->address = address;
      this->revision = revision;

      return elle::Status::Ok;
    }

//
// ---------- operators -------------------------------------------------------
//

    elle::Boolean
    Location::operator ==(Location const& other) const
    {
      // check the address as this may actually be the same object.
      if (this == &other)
        return true;

      // compare the attributes.
      if ((this->address != other.address) ||
          (this->revision != other.revision))
        return false;

      return true;
    }

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

      // dump the revision.
      if (this->revision.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the revision");

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Location::print(std::ostream& stream) const
    {
      stream << "location{"
             << this->address
             << ", "
             << this->revision
             << "}";
    }

  }
}
