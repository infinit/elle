//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// directory          /home/mycure/infinit/infinit/components/Directory.cc
//
// created       julien quintard   [sun mar  8 15:10:35 2009]
// updated       julien quintard   [sun mar  8 15:31:08 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Directory.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the directory object.
    ///
    Status		Directory::Create(KeyPair&			pair)
    {
      // call the parent class.
      if (Entity::Create(pair, Entity::TypeDirectory) == StatusError)
	escape("unable to create the underlying entity");

      leave();
    }

  }
}
