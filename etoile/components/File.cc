//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/infinit/components/File.cc
//
// created       julien quintard   [sun mar  8 15:10:35 2009]
// updated       julien quintard   [sun mar  8 15:31:03 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/File.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the file object.
    ///
    Status		File::Create(KeyPair&			pair)
    {
      // call the parent class.
      if (Entity::Create(pair, Entity::TypeFile) == StatusError)
	escape("unable to create the underlying entity");

      leave();
    }

  }
}
