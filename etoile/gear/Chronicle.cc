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
// updated       julien quintard   [thu jun 23 09:33:28 2011]
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
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Chronicle, _());

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
