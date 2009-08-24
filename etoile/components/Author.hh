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
// updated       julien quintard   [sun aug 23 16:29:13 2009]
//

#ifndef ETOILE_COMPONENTS_AUTHOR_HH
#define ETOILE_COMPONENTS_AUTHOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/context/Context.hh>

#include <etoile/agent/Agent.hh>

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
      static Status	Create(context::Object&);
    };

  }
}

#endif
