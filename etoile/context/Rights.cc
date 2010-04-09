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
// updated       julien quintard   [wed apr  7 19:29:01 2010]
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
    Status		Rights::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Rights]" << std::endl;

      // dump the role.
      std::cout << alignment << Dumpable::Shift << "[Role] "
		<< (Natural32)this->role << std::endl;

      // dump the permissions.
      std::cout << alignment << Dumpable::Shift << "[Permissions] "
		<< (Natural32)this->permissions << std::endl;

      // dump the secret key.
      if (this->key.Dump(margin + 2) == StatusError)
	escape("unable to dump the secret key");

      leave();
    }

  }
}
