//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/infinit/components/File.hh
//
// created       julien quintard   [sun mar  8 15:07:58 2009]
// updated       julien quintard   [sun mar  8 15:30:51 2009]
//

#ifndef ETOILE_COMPONENTS_FILE_HH
#define ETOILE_COMPONENTS_FILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/components/Entity.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    class File:
      public Entity
    {
    public:
      //
      // methods
      //
      Status		Create(KeyPair&);
    };

  }
}

#endif
