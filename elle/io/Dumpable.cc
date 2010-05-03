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
// updated       julien quintard   [sun may  2 11:51:13 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/Dumpable.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace standalone;

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
