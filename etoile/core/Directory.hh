//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// directory          /home/mycure/infinit/infinit/core/Directory.hh
//
// created       julien quintard   [sun mar  8 15:07:58 2009]
// updated       julien quintard   [mon aug  3 20:51:02 2009]
//

#ifndef ETOILE_COMPONENTS_DIRECTORY_HH
#define ETOILE_COMPONENTS_DIRECTORY_HH

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

    class Directory:
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
