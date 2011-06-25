//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Chronicle.cc
//
// created       julien quintard   [thu jun 23 09:32:55 2011]
// updated       julien quintard   [sat jun 25 13:39:31 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Chronicle.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a chronicle.
    ///
    elle::Status	Chronicle::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Chronicle]" << std::endl;

      // XXX

      leave();
    }

  }
}
