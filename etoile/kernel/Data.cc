//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Data.cc
//
// created       julien quintard   [tue aug  4 13:28:39 2009]
// updated       julien quintard   [tue apr 20 21:30:58 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Data.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Data::Data():
      state(StateClean)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method updates a segment of the data object.
    ///
    Status		Data::Write(const Offset&		offset,
				    const Region&		region)
    {
      enter();

      // expand if necessary.
      if ((offset + region.size) > this->region.capacity)
	{
	  // adjust the region capacity.
	  if (this->region.Adjust(offset + region.size) == StatusError)
	    escape("unable to expand the region");
	}

      // write the data.
      if (this->region.Write(offset,
			     region.contents,
			     region.size) == StatusError)
	escape("unable to write the data");

      // set the data as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// this method returns a region of the data object.
    ///
    Status		Data::Read(const Offset&		offset,
				   const Size&			size,
				   Region&			region) const
    {
      Size		length;

      enter();

      // check the operation's validity.
      if (offset > this->region.size)
	escape("the offset is out of bound");

      // set size to the maximum between the request size and the available
      // size.
      length = (this->region.size - offset) < size ?
	(this->region.size - offset) :
	size;

      // prepare the output region.
      if (region.Prepare(length) == StatusError)
	escape("unable to prepare the output region");

      // set the current size.
      region.size = length;

      // read the data from the region.
      if (this->region.Read(offset,
			    region.contents,
			    region.size) == StatusError)
	escape("unable to read the data");

      leave();
    }

    ///
    /// this method adjust the size of the data either by expanding or
    /// shrinking the region.
    ///
    Status		Data::Adjust(const Size&		size)
    {
      enter();

      // first, make sure the region's capacity is large enough.
      if (this->region.Adjust(size) == StatusError)
	escape("unable to adjust the region size");

      // then, manually set the region size.
      this->region.size = size;

      // set the data as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// this method returns the size of the data.
    ///
    Status		Data::Capacity(Size&			size) const
    {
      enter();

      // set the size.
      size = this->region.size;

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Data);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		Data::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Data]" << std::endl;

      // dump the state.
      std::cout << alignment << Dumpable::Shift << "[State] "
		<< this->state << std::endl;

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
      enter();

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
      enter();

      // extract the region.
      if (archive.Extract(this->region) == StatusError)
	escape("unable to extract the region");

      leave();
    }

  }
}
