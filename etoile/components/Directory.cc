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
// updated       julien quintard   [thu jul 23 14:05:53 2009]
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
      if (Object::Create(pair, Object::TypeDirectory) == StatusError)
	escape("unable to create the underlying object");

      leave();
    }

  }
}
