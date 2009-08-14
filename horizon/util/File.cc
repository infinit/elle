//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/util/File.cc
//
// created       julien quintard   [sat aug  1 21:11:57 2009]
// updated       julien quintard   [tue aug 11 00:57:15 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/util/File.hh>

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
    Status		File::Create(etoile::core::Object&	file,
				     const elle::crypto::KeyPair& user)
    {
      // create the file.
      if (file.Create(ComponentFile, user) == StatusError)
	escape("unable to create the file object");

      leave();
    }

    ///
    /// XXX
    ///
    Status		File::Destroy(etoile::core::Object&	file)
    {
      Address		address;

      // seal the object to obtain its address.
      if (file.Self(address) == StatusError)
	escape("unable to seal the file");

      // destroy the object block.
      if (Hole::Destroy(address) == StatusError)
	escape("unable to destroy the file object block");

      leave();
    }

    ///
    /// XXX
    ///
    Status		File::Store(etoile::core::Object&		file)
    {
      Address		address;

      // seal the file.
      if (file.Self(address) == StatusError)
	escape("unable to seal the file");

      // store the block.
      if (Hole::Put(address, file) == StatusError)
	escape("unable to store the file block");

      leave();
    }

    ///
    /// XXX
    ///
    Status		File::Read(const etoile::core::Object&	file,
				   const Natural64		offset,
				   Byte*			buffer,
				   Natural64&			size,
				   const PrivateKey&		k)
    {
      Data		data;

      // check if there is a linked data. if not, the default local
      // data object is used.
      if (file.data.contents.type != Contents::TypeNone)
	{
	  // load the data.
	  if (Hole::Get(file.data.contents.address, data) == StatusError)
	    escape("unable to load the data");
	}

      // read the buffer.
      if (data.Read(offset, buffer, size) == StatusError)
	escape("unable to read the data");

      leave();
    }

    ///
    /// XXX
    ///
    Status		File::Write(etoile::core::Object&		file,
				    const Natural64		offset,
				    const Byte*			buffer,
				    const Natural64		size,
				    const PrivateKey&		k)
    {
      Data		data;
      Address		address;

      // check if there is a linked data. if not, the default local
      // data object is used.
      if (file.data.contents.type != Contents::TypeNone)
	{
	  // load the data.
	  if (Hole::Get(file.data.contents.address, data) == StatusError)
	    escape("unable to load the data");
	}

      // write the buffer.
      if (data.Write(offset, buffer, size) == StatusError)
	escape("unable to write the data");

      // seal the data.
      if (data.Self(address) == StatusError)
	escape("unable to seal the data");

      // store the new data.
      if (Hole::Put(address, data) == StatusError)
	escape("unable to store the data");

      // XXX
      Contents		contents;

      contents.Create(Contents::TypeRaw);
      XXX

      // set the new data.
      if (file.Update(k, address) == StatusError)
	escape("unable to update the file");

      // update the size.
      // XXX file.meta.status.size = data.region.size;

      leave();
    }

    ///
    /// XXX
    ///
    Status		File::Adjust(etoile::core::Object&	file,
				     const Natural64		size,
				     const PrivateKey&		k)
    {
      Address		address;
      Data		data;

      // check if there is a linked data. if not, the default local
      // data object is used.
      if (file.data.contents.type != Contents::Address::Null)
	{
	  // load the data.
	  if (Hole::Get(file.data.contents.address, data) == StatusError)
	    escape("unable to load the data");
	}

      // check the size.
      if (size != 0)
	{
	  Address	a;

	  // adjust the data.
	  if (data.Adjust(size) == StatusError)
	    escape("unable to adjust the data size");

	  // seal the data.
	  if (data.Self(a) == StatusError)
	    escape("unable to seal the data");

	  // store the new data.
	  if (Hole::Put(a, data) == StatusError)
	    escape("unable to store the data");

	  // set the address.
	  address = a;
	}
      else
	{
	  // set the address to null.
	  address = Address::Null;
	}

      // set the new data.
      if (file.Update(k, address) == StatusError)
	escape("unable to update the file");

      // update the size.
      // XXX file.meta.status.size = size;

      leave();
    }

  }
}
