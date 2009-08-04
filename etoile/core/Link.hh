//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// link          /home/mycure/infinit/etoile/core/Link.hh
//
// created       julien quintard   [sun mar  8 15:07:58 2009]
// updated       julien quintard   [tue aug  4 21:01:32 2009]
//

#ifndef ETOILE_COMPONENTS_LINK_HH
#define ETOILE_COMPONENTS_LINK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Object.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Link:
      public Object
    {
    public:
      //
      // methods
      //
      Status		Create(const KeyPair&);
    };

  }
}

#endif
