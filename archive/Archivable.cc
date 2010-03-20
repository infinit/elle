//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/archive/Archivable.cc
//
// created       julien quintard   [sat mar 20 00:02:21 2010]
// updated       julien quintard   [sat mar 20 03:24:21 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/archive/Archivable.hh>

///
/// these includes are placed here to prevent pre-processing conflicts.
///
#include <elle/archive/Archive.hh>

#include <elle/misc/Maid.hh>
#include <elle/misc/Report.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace misc;

  namespace archive
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method serializes an object.
    ///
    Status		Archivable::Serialize(Archive&) const
    {
      enter();

      escape("this method should never have been called");
    }

    ///
    /// this method extract an object.
    ///
    Status		Archivable::Extract(Archive&)
    {
      enter();

      escape("this method should never have been called");
    }

  }
}
