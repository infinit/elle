//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Directory.cc
//
// created       julien quintard   [sun aug  9 16:33:09 2009]
// updated       julien quintard   [mon aug 10 14:31:15 2009]
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
    /// this method takes an address and returns a live directory object.
    ///
    Status		Directory::Load(const Address&		address,
					Object&			object)
    {
      // retrieve the block.
      if (Hole::Get(address, object) == StatusError)
	escape("unable to retrieve the block");

      // check the object type.
      if (object.meta.status.component != ComponentDirectory)
	escape("this object is not a directory");

      leave();
    }

    ///
    /// this method takes a directory object, extract its catalog and
    /// look up the given entry name.
    ///
    Status		Directory::Lookup(const Object&		directory,
					  const String&		name,
					  Address&		address)
    {
      // retrieve the catalog block.
      // XXX

      // decrypt it.
      // XXX

      leave();
    }

  }
}
