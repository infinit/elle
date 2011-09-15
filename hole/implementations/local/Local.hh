//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [tue apr 13 16:05:04 2010]
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
