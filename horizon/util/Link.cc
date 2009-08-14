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
// updated       julien quintard   [tue aug 11 00:55:21 2009]
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
    Status		Link::Create(etoile::core::Object&	link,
				     const elle::crypto::KeyPair& user)
    {
      // create the link.
      if (link.Create(ComponentLink, user) == StatusError)
	escape("unable to create the link object");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Link::Store(etoile::core::Object&		link)
    {
      Address		address;

      // seal the link.
      if (link.Self(address) == StatusError)
	escape("unable to seal the link");

      // store the block.
      if (Hole::Put(address, link) == StatusError)
	escape("unable to store the link block");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Link::Set(etoile::core::Object&		link,
				  const String&			target,
				  const PrivateKey&		k)
    {
      Address		address;
      Data		data;

      // XXX[would be better to archive it properly]

      // write the data.
      if (data.Write(0,
		     (Byte*)target.c_str(),
		     (Natural64)target.length() + 1) == StatusError)
	escape("unable to write the data");

      // seal the data.
      if (data.Self(address) == StatusError)
	escape("unable to seal the data");

      // store the data.
      if (Hole::Put(address, data) == StatusError)
	escape("unable to store the data block");

      // set the new data.
      if (link.Update(k, address) == StatusError)
	escape("unable to update the link");

      // update the size.
      // XXX link.meta.status.size = target.length();

      leave();
    }

    ///
    /// XXX
    ///
    Status		Link::Get(const etoile::core::Object&	link,
				  String&			target)
    {
      Data		data;

      // check if there is a linked data.
      if (link.data.contents.type == Contents::TypeNone)
	leave();

      // get the block.
      if (Hole::Get(link.data.contents.address, data) == StatusError)
	escape("unable to load the data");

      // store the target in the variable.
      target.assign((char*)data.region.contents, data.region.size);

      leave();
    }

  }
}
