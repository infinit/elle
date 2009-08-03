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
// updated       julien quintard   [mon aug  3 20:50:55 2009]
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
    Status		File::Create(const KeyPair&		pair)
    {
      // call the parent class.
      if (Object::Create(pair, Object::TypeFile) == StatusError)
	escape("unable to create the underlying object");

      leave();
    }

  }
}
