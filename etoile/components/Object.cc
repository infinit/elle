//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Object.cc
//
// created       julien quintard   [fri aug 14 19:16:10 2009]
// updated       julien quintard   [wed mar  3 16:07:33 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Object.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Object::Load(context::Object*		context,
				     const hole::Address&	address)
    {
      enter();

      // set the object address.
      context->address = address;

      // get the block.
      if (depot::Depot::Get(address, context->object) == StatusError)
	escape("unable to retrieve the block");

      leave();
    }

  }
}
