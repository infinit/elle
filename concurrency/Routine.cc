//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Routine.cc
//
// created       julien quintard   [wed mar 24 21:12:54 2010]
// updated       julien quintard   [fri may 28 12:16:11 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Routine.hh>

#include <elle/core/String.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace concurrency
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
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Routine, _());

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
