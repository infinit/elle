#include <nucleus/neutron/Trait.hh>

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
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    elle::Boolean       Trait::operator==(const Trait&          element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the name and value.
      if ((this->name != element.name) ||
          (this->value != element.value))
        return false;

      return true;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Trait, _());

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
      std::cout << alignment << elle::Dumpable::Shift
                << "[Name] " << this->name << std::endl;

      // dump the value.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Value] " << this->value << std::endl;

      return elle::Status::Ok;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the trait object.
    ///
    //elle::Status        Trait::Serialize(elle::Archive&         archive) const
    //{
    //  // serialize the attributes.
    //  if (archive.Serialize(this->name,
    //                        this->value) == elle::Status::Error)
    //    escape("unable to serialize the trait");

    //  return elle::Status::Ok;
    //}

    /////
    ///// this method extracts the trait object.
    /////
    //elle::Status        Trait::Extract(elle::Archive&           archive)
    //{
    //  // extract the attributes.
    //  if (archive.Extract(this->name,
    //                      this->value) == elle::Status::Error)
    //    escape("unable to extract the trait");

    //  return elle::Status::Ok;
    //}

//
// ---------- rangeable -------------------------------------------------------
//

    ///
    /// this method returns the symbol of a trait i.e the name.
    ///
    elle::String&       Trait::Symbol()
    {
      return (this->name);
    }

  }
}
