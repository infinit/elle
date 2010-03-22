//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Author.hh
//
// created       julien quintard   [sun aug 23 15:30:06 2009]
// updated       julien quintard   [sun mar 21 18:18:16 2010]
//

#ifndef ETOILE_COMPONENTS_AUTHOR_HH
#define ETOILE_COMPONENTS_AUTHOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/context/Context.hh>
#include <etoile/user/User.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Author
    {
    public:
      //
      // static methods
      //
      static Status	Forge(context::Object*);
    };

  }
}

#endif
