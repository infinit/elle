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
// updated       julien quintard   [thu mar 25 17:42:59 2010]
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

      // dump the permissions.
      std::cout << alignment << Dumpable::Shift << "[Permissions] "
		<< this->permissions << std::endl;

      // dump the secret key.
      if (this->key.Dump(margin + 2) == StatusError)
	escape("unable to dump the secret key");

      leave();
    }

  }
}
