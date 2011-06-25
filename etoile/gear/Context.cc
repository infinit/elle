//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Context.cc
//
// created       julien quintard   [thu jun 16 10:37:02 2011]
// updated       julien quintard   [fri jun 24 13:15:59 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Context.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Context::Context(const Nature				nature):
      nature(nature)
    {
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an context object.
    ///
    elle::Status	Context::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      // display the name.
      std::cout << alignment << "[Context] " << std::endl;

      // display the family.
      std::cout << alignment << elle::Dumpable::Shift << "[Nature] "
		<< (elle::Natural32)this->nature << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the context object.
    ///
    elle::Status	Context::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize((elle::Natural8&)this->nature) ==
	  elle::StatusError)
	escape("unable to serialize the attributes");

      leave();
    }

    ///
    /// this method extracts the context object.
    ///
    elle::Status	Context::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract((elle::Natural8&)this->nature) == elle::StatusError)
	escape("unable to extract the attributes");

      leave();
    }

  }
}
