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
// updated       julien quintard   [thu sep  8 17:19:55 2011]
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
    /// this namespace contains everything related to the local implementation.
    ///
    /// this implementation basically stores every block on the node's
    /// local storage device.
    ///
    namespace local
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// this class controls the local implementation.
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
