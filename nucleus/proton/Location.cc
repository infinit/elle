#include <nucleus/proton/Location.hh>

namespace nucleus
{
  namespace proton
  {

    /*-------------.
    | Construction |
    `-------------*/

    Location::Location()
    {
    }

    Location::Location(Address const& address,
                       Revision const& revision):
      _address(address),
      _revision(revision)
    {
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    Location::operator ==(Location const& other) const
    {
      if (this == &other)
        return (true);

      return ((this->_address == other._address) &&
              (this->_revision == other._revision));
    }

    /*-----------.
    | Interfaces |
    `-----------*/

    elle::Status
    Location::Dump(const elle::Natural32    margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Location]" << std::endl;

      // dump the address.
      if (this->_address.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the address");

      // dump the revision.
      if (this->_revision.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the revision");

      return elle::Status::Ok;
    }

    void
    Location::print(std::ostream& stream) const
    {
      stream << "location{"
             << this->_address
             << ", "
             << this->_revision
             << "}";
    }

  }
}
