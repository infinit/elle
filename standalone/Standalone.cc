//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/standalone/Standalone.cc
//
// created       julien quintard   [sun may  2 11:00:51 2010]
// updated       julien quintard   [sun may  2 11:02:34 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Standalone.hh>

namespace elle
{
  namespace standalone
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the module.
    ///
    Status		Standalone::Initialize()
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
    Status		Standalone::Clean()
    {
      enter();

      // clean the report.
      if (Report::Clean() == StatusError)
	escape("unable to clean the report");

      leave();
    }

  }
}
