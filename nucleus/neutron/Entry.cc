#include <nucleus/neutron/Entry.hh>

#include <elle/serialize/footprint.hh>

namespace nucleus
{
  namespace neutron
  {
//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// empty constructor.
    ///
    Entry::Entry():
      _footprint(0)
    {
    }

    ///
    /// default constructor.
    ///
    Entry::Entry(const elle::String&                            name,
                 const proton::Address&                         address):
      _name(name),
      _address(address),
      _footprint(0)
    {
      this->_footprint = elle::serialize::footprint(*this);
    }

//
// ---------- operators -------------------------------------------------------
//

    elle::Boolean
    Entry::operator ==(Entry const& other) const
    {
      if (this == &other)
        return true;

      return ((this->_name == other._name) &&
              (this->_address == other._address));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a entry.
    ///
    elle::Status        Entry::Dump(elle::Natural32             margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Entry]" << std::endl;

      // dump the name.
      std::cout << alignment << elle::io::Dumpable::Shift << "[Name] "
                << this->_name << std::endl;

      // dump the address.
      if (this->_address.Dump(margin + 2) == elle::Status::Error)
        throw Exception("unable to dump the token");

      // dump the footprint.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Footprint] " << std::dec << this->_footprint << std::endl;

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Entry::print(std::ostream& stream) const
    {
      stream << "entry{"
             << this->_name
             << ", "
             << this->_address
             << "}";
    }

    /*----------.
    | Rangeable |
    `----------*/

    elle::String const&
    Entry::symbol() const
    {
      return (this->_name);
    }

  }
}
