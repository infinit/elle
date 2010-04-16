//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Trait.cc
//
// created       julien quintard   [thu apr  1 22:00:03 2010]
// updated       julien quintard   [thu apr 15 15:26:00 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Trait.hh>

namespace etoile
{
  namespace kernel
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
    Trait::Trait(const String&					name,
		 const String&					value):
      name(name),
      value(value)
    {
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    Boolean		Trait::operator==(const Trait&		element) const
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
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Trait);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a trait.
    ///
    Status		Trait::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Trait]" << std::endl;

      // dump the name.
      std::cout << alignment << Dumpable::Shift
		<< "[Name] " << this->name << std::endl;

      // dump the value.
      std::cout << alignment << Dumpable::Shift
		<< "[Value] " << this->value << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the trait object.
    ///
    Status		Trait::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->name,
			    this->value) == StatusError)
	escape("unable to serialize the trait");

      leave();
    }

    ///
    /// this method extracts the trait object.
    ///
    Status		Trait::Extract(Archive&		archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract(this->name,
			  this->value) == StatusError)
	escape("unable to extract the trait");

      leave();
    }

  }
}
