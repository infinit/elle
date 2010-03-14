//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Application.hh
//
// created       julien quintard   [thu mar 11 17:09:50 2010]
// updated       julien quintard   [thu mar 11 17:12:46 2010]
//

#ifndef ETOILE_USER_APPLICATION_HH
#define ETOILE_USER_APPLICATION_HH

namespace etoile
{
  namespace user
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an application connected to Etoile
    /// and requesting operations.
    ///
    class Application
    {
    public:
      //
      // attributes
      //
      Socket*		socket;
    };

  }
}

#endif
