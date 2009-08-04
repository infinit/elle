//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// link          /home/mycure/infinit/etoile/core/Link.cc
//
// created       julien quintard   [sun mar  8 15:10:35 2009]
// updated       julien quintard   [tue aug  4 21:01:49 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Link.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the link object.
    ///
    Status		Link::Create(const KeyPair&		pair)
    {
      // call the parent class.
      if (Object::Create(pair, Object::TypeLink) == StatusError)
	escape("unable to create the underlying object");

      leave();
    }

  }
}
