//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/misc/Routine.cc
//
// created       julien quintard   [wed mar 24 21:12:54 2010]
// updated       julien quintard   [thu mar 25 00:40:15 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Routine.hh>

namespace elle
{
  namespace misc
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
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Routine]" << std::endl;

      // dump the type
      std::cout << alignment << shift << "[Type] " << this->type << std::endl;

      leave();
    }

  }
}
