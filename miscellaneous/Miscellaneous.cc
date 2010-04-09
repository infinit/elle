//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/miscellaneous/Miscellaneous.cc
//
// created       julien quintard   [thu apr  8 00:04:19 2010]
// updated       julien quintard   [thu apr  8 14:49:12 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/miscellaneous/Miscellaneous.hh>

namespace elle
{
  namespace miscellaneous
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the module.
    ///
    Status		Miscellaneous::Initialize()
    {
      enter();

      // initialize the report.
      if (Report::Initialize() == StatusError)
	escape("unable to initialize the report");

      leave();
    }

    ///
    /// this method cleans the module.
    ///
    Status		Miscellaneous::Clean()
    {
      enter();

      // clean the report.
      if (Report::Clean() == StatusError)
	escape("unable to clean the report");

      leave();
    }

  }
}
