//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       GPL
//
// file          /home/mycure/infinit/infinit/components/File.hh
//
// created       julien quintard   [sun mar  8 15:07:58 2009]
// updated       julien quintard   [sun mar  8 15:30:51 2009]
//

#ifndef INFINIT_COMPONENTS_FILE_HH
#define INFINIT_COMPONENTS_FILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <infinit/components/Entity.hh>

namespace infinit
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
