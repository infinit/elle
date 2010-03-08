//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Individual.hh
//
// created       julien quintard   [thu mar  4 12:00:38 2010]
// updated       julien quintard   [thu mar  4 12:51:14 2010]
//

#ifndef ETOILE_USER_INDIVIDUAL_HH
#define ETOILE_USER_INDIVIDUAL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a user individual along with its connections
    /// and status.
    ///
    class Individual
    {
    public:
      //
      // enumerations
      //
      enum Status
	{
	  StatusUnknown,
	  StatusReady
	};

      //
      // constructors & destructors
      //
      Individual();

      //
      // attributes
      //
      Status		status;

      Door*		client;
      Door*		agent;
    };

  }
}

#endif
