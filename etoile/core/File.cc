//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/File.cc
//
// created       julien quintard   [sun mar  8 15:10:35 2009]
// updated       julien quintard   [thu jul 23 14:05:58 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/File.hh>

namespace etoile
{
  namespace core
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
      if (Object::Create(pair, Object::TypeFile) == StatusError)
	escape("unable to create the underlying object");

      leave();
    }

  }
}
