//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/local/Local.hh
//
// created       julien quintard   [tue apr 13 16:05:04 2010]
// updated       julien quintard   [wed aug 31 14:37:59 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_LOCAL_LOCAL_HH
#define HOLE_IMPLEMENTATIONS_LOCAL_LOCAL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/local/Machine.hh>

namespace hole
{
  namespace implementations
  {
    ///
    /// XXX
    ///
    namespace local
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// the local hole implementation stores data on the local node's
      /// operating system storage.
      ///
      class Local
      {
      public:
	//
	// static attributes
	//
	static Machine*		Computer;
      };

    }
  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/local/Implementation.hh>

#endif
