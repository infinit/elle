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

#ifndef CORE_COMPONENTS_FILE_HH
#define CORE_COMPONENTS_FILE_HH

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
