//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon oct 17 10:52:46 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/nest/Placement.hh>

namespace etoile
{
  namespace nest
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable holds the current placement counter.
    ///
    const elle::Natural64		Placement::Zero = 0;

    ///
    /// this variable defines an unused hence null Placement.
    ///
    const Placement			Placement::Null;

    ///
    /// this variable holds the current placement counter.
    ///
    elle::Natural64			Placement::Counter =
      Placement::Zero + 1;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Placement::Placement():
      value(Placement::Zero)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method generates a new unique placement.
    ///
    elle::Status	Placement::Generate()
    {
      enter();

      // increments the counter.
      this->value = Placement::Counter++;

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method check if two placements match.
    ///
    elle::Boolean	Placement::operator==(const Placement& element)
      const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the placement.
      if (this->value != element.value)
	false();

      true();
    }

    ///
    /// this operator compares two placements.
    ///
    elle::Boolean	Placement::operator<(const Placement&	element) const
    {
      return (this->value < element.value);
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Placement, _());

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the placement.
    ///
    elle::Status	Placement::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->value) == elle::StatusError)
	escape("unable to serialize the placement attributes");

      leave();
    };

    ///
    /// this method extracts the placement.
    ///
    elle::Status	Placement::Extract(elle::Archive&	archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract(this->value) == elle::StatusError)
	escape("unable to extract the placement attributes");

      leave();
    };

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an placement.
    ///
    elle::Status	Placement::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Placement] "
		<< std::dec << this->value << std::endl;

      leave();
    }

  }
}
