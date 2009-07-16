//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       GPL
//
// directory          /home/mycure/infinit/infinit/components/Directory.hh
//
// created       julien quintard   [sun mar  8 15:07:58 2009]
// updated       julien quintard   [sun mar  8 15:30:57 2009]
//

#ifndef CORE_COMPONENTS_DIRECTORY_HH
#define CORE_COMPONENTS_DIRECTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <core/components/Entity.hh>

namespace core
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Directory:
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
