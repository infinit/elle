#include <elle/concurrency/Routine.hh>

#include <elle/types.hh>

#include <elle/standalone/Report.hh>
#include <elle/standalone/Log.hh>

#include <elle/idiom/Open.hh>

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
      std::cout << alignment << io::Dumpable::Shift
                << "[Scheme] " << this->scheme << std::endl;

      return Status::Ok;
    }

  }
}
