//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed mar 24 21:12:54 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Routine.hh>

#include <elle/types.hh>

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
      scheme(Routine::SchemeUnknown)
    {
    }

    ///
    /// specific constructor.
    ///
    Routine::Routine(const Scheme                               scheme):
      scheme(scheme)
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
    Status              Routine::Dump(const Natural32           margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Routine]" << std::endl;

      // dump the scheme.
      std::cout << alignment << Dumpable::Shift
                << "[Scheme] " << this->scheme << std::endl;

      return Status::Ok;
    }

  }
}
