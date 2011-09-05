//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Radix.cc
//
// created       julien quintard   [tue apr 27 12:11:16 2010]
// updated       julien quintard   [sat sep  3 19:27:32 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Radix.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace radix
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the radix module.
    ///
    Status		Radix::Initialize()
    {
      enter();

      // initialize the meta class.
      if (Meta::Initialize() == StatusError)
	escape("unable to initialize the meta class");

      leave();
    }

    ///
    /// this method cleans the radix module.
    ///
    Status		Radix::Clean()
    {
      enter();

      // clean the meta class.
      if (Meta::Clean() == StatusError)
	escape("unable to clean the meta class");

      leave();
    }

  }
}
