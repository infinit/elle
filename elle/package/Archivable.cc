//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sat mar 20 00:02:21 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/package/Archivable.hh>
#include <elle/package/Archive.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace standalone;

  namespace package
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method serializes an object.
    ///
    Status              Archivable::Serialize(Archive&) const
    {
      enter();

      escape("this method should never have been called");
    }

    ///
    /// this method extract an object.
    ///
    Status              Archivable::Extract(Archive&)
    {
      enter();

      escape("this method should never have been called");
    }

  }
}
