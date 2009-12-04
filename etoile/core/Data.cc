//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Data.cc
//
// created       julien quintard   [tue aug  4 13:28:39 2009]
// updated       julien quintard   [tue dec  1 03:18:00 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Data.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// XXX
    ///
    Data::Data():
      Contents(hole::Block::KindData)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns a segment of the data object.
    ///
    Status		Data::Read(const Natural64		offset,
				   Byte*			buffer,
				   Natural64&			size) const
    {
      // check the offset.
      if (offset > this->region.size)
	escape("the offset is out of bound");

      // set size to the maximum between the request size and the available
      // size.
      size = (this->region.size - offset) < size ?
	(this->region.size - offset) : size;

      // read the data from the region.
      if (this->region.Read(offset, buffer, size) == StatusError)
	escape("unable to read the data");

      leave();
    }

    ///
    /// this method updates a segment of the data object.
    ///
    Status		Data::Write(const Natural64		offset,
				    const Byte*			buffer,
				    const Natural64		size)
    {
      // check if the region needs expanding.
      if ((offset + size) > region.size)
	{
	  if (this->region.Adjust(offset + size) == StatusError)
	    escape("unable to expand the region");
	}

      // write the data.
      if (this->region.Write(offset, buffer, size) == StatusError)
	escape("unable to write the data");

      leave();
    }

    ///
    /// this method adjust the size of the data either by expanding or
    /// shrinking the region.
    ///
    Status		Data::Adjust(const Natural64		size)
    {
      // first, make sure the region's capacity is large enough.
      if (this->region.Adjust(size) == StatusError)
	escape("unable to adjust the region size");

      // then, manually set the region size.
      this->region.size = size;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Data::Size(Natural64&			size) const
    {
      // set the size.
      size = this->region.size;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		Data::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Data]" << std::endl;

      // dump the underlying block.
      if (ContentHashBlock::Dump(margin + 2) == StatusError)
	escape("unable to dump the underlying block");

      // dump the region attribute.
      if (this->region.Dump(margin + 2) == StatusError)
	escape("unable to dump the region");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    Status		Data::Serialize(Archive&		archive) const
    {
      // call the parent class.
      if (ContentHashBlock::Serialize(archive) == StatusError)
	escape("unable to serialize the block");

      // serialize the internal region.
      if (archive.Serialize(this->region) == StatusError)
	escape("unable to serialize the internal region");

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		Data::Extract(Archive&			archive)
    {
      // call the parent class.
      if (ContentHashBlock::Extract(archive) == StatusError)
	escape("unable to extract the block");

      // extract the region.
      if (archive.Extract(this->region) == StatusError)
	escape("unable to extract the region");

      leave();
    }

  }
}
