//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/util/Link.cc
//
// created       julien quintard   [sat aug  1 21:11:57 2009]
// updated       julien quintard   [wed aug  5 22:50:03 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/util/Link.hh>

namespace pig
{
  namespace util
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Link::Create(etoile::core::Link&	link,
				     const elle::crypto::KeyPair& user)
    {
      // create the link.
      if (link.Create(user) == StatusError)
	escape("unable to create the link object");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Link::Store(etoile::core::Link&		link)
    {
      // seal the link.
      if (link.Seal() == StatusError)
	escape("unable to seal the link");

      // store the block.
      if (Hole::Put(link.address, link) == StatusError)
	escape("unable to store the link block");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Link::Set(etoile::core::Link&		link,
				  const String&			target,
				  const PrivateKey&		k)
    {
      Data		data;

      // XXX[would be better to archive it properly]

      // write the data.
      if (data.Write(0,
		     (Byte*)target.c_str(),
		     (Natural64)target.length() + 1) == StatusError)
	escape("unable to write the data");

      // seal the data.
      if (data.Seal() == StatusError)
	escape("unable to seal the data");

      // store the data.
      if (Hole::Put(data.address, data) == StatusError)
	escape("unable to store the data block");

      // set the new data.
      if (link.Update(k, data.address) == StatusError)
	escape("unable to update the link");

      // update the size.
      link.meta.status.size = target.length();

      leave();
    }

    ///
    /// XXX
    ///
    Status		Link::Get(const etoile::core::Link&	link,
				  String&			target)
    {
      Data		data;

      // check if there is a linked data.
      if (link.data.references == Address::Null)
	leave();

      // get the block.
      if (Hole::Get(link.data.references, data) == StatusError)
	escape("unable to load the data");

      // store the target in the variable.
      target.assign((char*)data.region.contents, data.region.size);

      leave();
    }

  }
}
