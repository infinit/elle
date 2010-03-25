//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Dumpable.cc
//
// created       julien quintard   [sat mar 20 00:43:22 2010]
// updated       julien quintard   [thu mar 25 17:36:47 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/Dumpable.hh>

///
/// these includes are placed here in order to prevent pre-processing
/// conflicts.
///
#include <elle/misc/Report.hh>
#include <elle/misc/Maid.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace io
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this string represents the shifting used when displaying
    /// information.
    ///
    const String		Dumpable::Shift(2, ' ');

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method dumps an object.
    ///
    Status		Dumpable::Dump(const Natural32) const
    {
      enter();

      escape("this method should never have been called");
    }

  }
}
