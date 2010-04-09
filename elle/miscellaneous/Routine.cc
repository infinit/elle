//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/miscellaneous/Routine.cc
//
// created       julien quintard   [wed mar 24 21:12:54 2010]
// updated       julien quintard   [thu mar 25 17:39:23 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/miscellaneous/Routine.hh>

///
/// these includes were placed here for avoiding pre-processing conflicts.
///
#include <elle/miscellaneous/Report.hh>
#include <elle/miscellaneous/Maid.hh>

namespace elle
{
  namespace miscellaneous
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Routine::Routine():
      type(Routine::TypeUnknown)
    {
    }

    ///
    /// specific constructor.
    ///
    Routine::Routine(const Type					type):
      type(type)
    {
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Routine);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the kind.
    ///
    Status		Routine::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Routine]" << std::endl;

      // dump the type
      std::cout << alignment << Dumpable::Shift
		<< "[Type] " << this->type << std::endl;

      leave();
    }

  }
}
