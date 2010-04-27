//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/core/Core.cc
//
// created       julien quintard   [tue apr 27 12:11:16 2010]
// updated       julien quintard   [tue apr 27 12:13:39 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

///
/// these includes are placed here in order to prevent pre-processing
/// conflicts.
///
#include <elle/miscellaneous/Report.hh>
#include <elle/miscellaneous/Maid.hh>

namespace elle
{
  namespace core
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the core module.
    ///
    Status		Core::Initialize()
    {
      enter();

      // initialize the meta class.
      if (Meta::Initialize() == StatusError)
	escape("unable to initialize the meta class");

      leave();
    }

    ///
    /// this method cleans the core module.
    ///
    Status		Core::Clean()
    {
      enter();

      // clean the meta class.
      if (Meta::Clean() == StatusError)
	escape("unable to clean the meta class");

      leave();
    }

  }
}
