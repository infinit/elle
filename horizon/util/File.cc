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
// updated       julien quintard   [thu aug  6 00:05:54 2009]
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
    Status		File::Create(etoile::core::File&	file,
				     const elle::crypto::KeyPair& user)
    {
      // create the file.
      if (file.Create(user) == StatusError)
	escape("unable to create the file object");

      leave();
    }

    ///
    /// XXX
    ///
    Status		File::Destroy(etoile::core::File&	file)
    {
      // seal the object to obtain its address.
      if (file.Seal() == StatusError)
	escape("unable to seal the file");

      // destroy the object block.
      if (Hole::Destroy(file.address) == StatusError)
	escape("unable to destroy the file object block");

      leave();
    }

    ///
    /// XXX
    ///
    Status		File::Store(etoile::core::File&		file)
    {
      // seal the file.
      if (file.Seal() == StatusError)
	escape("unable to seal the file");

      // store the block.
      if (Hole::Put(file.address, file) == StatusError)
	escape("unable to store the file block");

      leave();
    }

    ///
    /// XXX
    ///
    Status		File::Read(const etoile::core::File&	file,
				   const Natural64		offset,
				   Byte*			buffer,
				   Natural64&			size,
				   const PrivateKey&		k)
    {
      Data		data;

      // check if there is a linked data. if not, the default local
      // data object is used.
      if (file.data.references != Address::Null)
	{
	  // load the data.
	  if (Hole::Get(file.data.references, data) == StatusError)
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
    Status		File::Write(etoile::core::File&		file,
				    const Natural64		offset,
				    const Byte*			buffer,
				    const Natural64		size,
				    const PrivateKey&		k)
    {
      Data		data;

      // check if there is a linked data. if not, the default local
      // data object is used.
      if (file.data.references != Address::Null)
	{
	  // load the data.
	  if (Hole::Get(file.data.references, data) == StatusError)
	    escape("unable to load the data");
	}

      // write the buffer.
      if (data.Write(offset, buffer, size) == StatusError)
	escape("unable to write the data");

      // seal the data.
      if (data.Seal() == StatusError)
	escape("unable to seal the data");

      // store the new data.
      if (Hole::Put(data.address, data) == StatusError)
	escape("unable to store the data");

      // set the new data.
      if (file.Update(k, data.address) == StatusError)
	escape("unable to update the file");

      // update the size.
      file.meta.status.size = data.region.size;

      leave();
    }

    ///
    /// XXX
    ///
    Status		File::Adjust(etoile::core::File&	file,
				     const Natural64		size,
				     const PrivateKey&		k)
    {
      Address		address;
      Data		data;

      // check if there is a linked data. if not, the default local
      // data object is used.
      if (file.data.references != Address::Null)
	{
	  // load the data.
	  if (Hole::Get(file.data.references, data) == StatusError)
	    escape("unable to load the data");
	}

      // check the size.
      if (size != 0)
	{
	  // adjust the data.
	  if (data.Adjust(size) == StatusError)
	    escape("unable to adjust the data size");

	  // seal the data.
	  if (data.Seal() == StatusError)
	    escape("unable to seal the data");

	  // store the new data.
	  if (Hole::Put(data.address, data) == StatusError)
	    escape("unable to store the data");

	  // set the address.
	  address = data.address;
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
      file.meta.status.size = size;

      leave();
    }

  }
}
