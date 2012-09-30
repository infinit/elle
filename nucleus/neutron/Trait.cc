#include <nucleus/neutron/Trait.hh>

#include <elle/standalone/Report.hh>
#include <elle/standalone/Log.hh>
#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this defines an unexisting trait.
    ///
    const Trait                 Trait::Null;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// empty constructor.
    ///
    Trait::Trait()
    {
    }

    ///
    /// default constructor.
    ///
    Trait::Trait(const elle::String&                            name,
                 const elle::String&                            value):
      name(name),
      value(value)
    {
    }

//
// ---------- operators -------------------------------------------------------
//

    elle::Boolean
    Trait::operator ==(Trait const& other) const
    {
      // check the address as this may actually be the same object.
      if (this == &other)
        return true;

      // compare the name and value.
      if ((this->name != other.name) ||
          (this->value != other.value))
        return false;

      return true;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a trait.
    ///
    elle::Status        Trait::Dump(elle::Natural32             margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Trait]" << std::endl;

      // dump the name.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Name] " << this->name << std::endl;

      // dump the value.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Value] " << this->value << std::endl;

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Trait::print(std::ostream& stream) const
    {
      stream << "trait("
             << this->name
             << ", "
             << this->value
             << ")";
    }

//
// ---------- rangeable -------------------------------------------------------
//

    ///
    /// this method returns the symbol of a trait i.e the name.
    ///
    elle::String&
    Trait::symbol()
    {
      return (this->name);
    }

  }
}
