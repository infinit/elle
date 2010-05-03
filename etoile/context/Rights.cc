//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/context/Rights.cc
//
// created       julien quintard   [sun mar 21 16:55:17 2010]
// updated       julien quintard   [mon may  3 19:46:46 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Rights.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Rights::Rights():
      role(kernel::RoleUnknown)
    {
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the rights.
    ///
    elle::Status	Rights::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Rights]" << std::endl;

      // dump the role.
      std::cout << alignment << elle::Dumpable::Shift << "[Role] "
		<< (elle::Natural32)this->role << std::endl;

      // dump the record.
      if (this->record.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the record");

      // dump the secret key.
      if (this->key.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the secret key");

      leave();
    }

  }
}
