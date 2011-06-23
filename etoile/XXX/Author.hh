//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/XXX/Author.hh
//
// created       julien quintard   [sun aug 23 15:30:06 2009]
// updated       julien quintard   [mon jun 20 14:12:43 2011]
//

#ifndef ETOILE_COMPONENTS_AUTHOR_HH
#define ETOILE_COMPONENTS_AUTHOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/context/Object.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for manipulating the author
    /// object.
    ///
    class Author
    {
    public:
      //
      // static methods
      //
      static elle::Status	Forge(context::Object*);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/User.hh>

#include <etoile/components/Rights.hh>

#endif
