#include <nucleus/neutron/Entry.hh>

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
    Entry::Entry()
    {
    }

    ///
    /// default constructor.
    ///
    Entry::Entry(const elle::String&                            name,
                 const proton::Address&                         address):
      name(name),
      address(address)
    {
    }

//
// ---------- operators -------------------------------------------------------
//

    elle::Boolean
    Entry::operator ==(Entry const& other) const
    {
      // check the address as this may actually be the same object.
      if (this == &other)
        return true;

      // compare the attributes.
      if ((this->name != other.name) ||
          (this->address != other.address))
        return false;

      return true;
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
                << this->name << std::endl;

      // dump the address.
      if (this->address.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the token");

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Entry::print(std::ostream& stream) const
    {
      stream << "entry{"
             << this->name
             << ", "
             << this->address
             << "}";
    }

//
// ---------- rangeable -------------------------------------------------------
//

    ///
    /// this method returns the symbol of an entry i.e the name.
    ///
    elle::String&
    Entry::symbol()
    {
      return (this->name);
    }

  }
}
