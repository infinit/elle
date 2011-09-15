//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [thu apr  1 22:00:03 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Trait.hh>

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
    const Trait			Trait::Null;

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
    Trait::Trait(const elle::String&				name,
		 const elle::String&				value):
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
    elle::Boolean	Trait::operator==(const Trait&		element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the name and value.
      if ((this->name != element.name) ||
	  (this->value != element.value))
	false();

      true();
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
    elle::Status	Trait::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Trait]" << std::endl;

      // dump the name.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Name] " << this->name << std::endl;

      // dump the value.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Value] " << this->value << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the trait object.
    ///
    elle::Status	Trait::Serialize(elle::Archive&		archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->name,
			    this->value) == elle::StatusError)
	escape("unable to serialize the trait");

      leave();
    }

    ///
    /// this method extracts the trait object.
    ///
    elle::Status	Trait::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract(this->name,
			  this->value) == elle::StatusError)
	escape("unable to extract the trait");

      leave();
    }

//
// ---------- rangeable -------------------------------------------------------
//

    ///
    /// this method returns the symbol of a trait i.e the name.
    ///
    elle::String&	Trait::Symbol()
    {
      return (this->name);
    }

  }
}
