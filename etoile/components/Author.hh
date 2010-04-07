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
// updated       julien quintard   [tue apr  6 12:02:59 2010]
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
      static Status	Forge(context::Object*);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/User.hh>

#include <etoile/components/Rights.hh>

#endif
