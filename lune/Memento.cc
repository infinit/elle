//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Memento.cc
//
// created       julien quintard   [sat may  1 21:19:13 2010]
// updated       julien quintard   [mon may  3 12:23:20 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <lune/Memento.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this constant defines the name of the section in which the
  /// assignments are located.
  ///
  const elle::String		Memento::Section = "general";

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method creates a memento.
  ///
  elle::Status		Memento::Create(const elle::String&	name,
					const etoile::Address&	address)
  {
    elle::Unique	unique;

    enter();

    // generate the address' unique.
    address >> unique;

    // set the name assignment.
    if (this->settings.Set(Memento::Section,
			   "name",
			   name) == elle::StatusError)
      escape("unable to set the name assignment in the settings");

    // set the address assignment.
    if (this->settings.Set(Memento::Section,
			   "address",
			   unique) == elle::StatusError)
      escape("unable to set the address assignment in the settings");

    leave();
  }

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps a memento.
  ///
  elle::Status		Memento::Dump(const elle::Natural32	margin) const
  {
    elle::String	alignment(margin, ' ');

    enter();

    std::cout << alignment << "[Memento] " << std::endl;

    // dump the settings.
    if (this->settings.Dump(margin + 2) == elle::StatusError)
      escape("unable to dump the settings");

    leave();
  }

}
