//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// directory          /home/mycure/infinit/infinit/components/Directory.hh
//
// created       julien quintard   [sun mar  8 15:07:58 2009]
// updated       julien quintard   [thu jul 23 14:05:46 2009]
//

#ifndef ETOILE_COMPONENTS_DIRECTORY_HH
#define ETOILE_COMPONENTS_DIRECTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/components/Object.hh>

namespace etoile
{
  namespace components
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
      Status		Create(KeyPair&);
    };

  }
}

#endif
