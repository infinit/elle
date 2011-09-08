//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Data.cc
//
// created       julien quintard   [tue aug  4 13:28:39 2009]
// updated       julien quintard   [wed sep  7 11:20:33 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Data.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Data::Data():
      _state(proton::StateClean)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method updates a segment of the data object.
    ///
    elle::Status	Data::Write(const Offset&		offset,
				    const elle::Region&		region)
    {
      enter();

      // expand if necessary.
      if ((offset + region.size) > this->region.capacity)
	{
	  // adjust the region capacity.
	  if (this->region.Adjust(offset + region.size) == elle::StatusError)
	    escape("unable to expand the region");
	}

      // write the data.
      if (this->region.Write(offset,
			     region.contents,
			     region.size) == elle::StatusError)
	escape("unable to write the data");

      // set the data as dirty.
      this->_state = proton::StateDirty;

      leave();
    }

    ///
    /// this method returns a region of the data object.
    ///
    elle::Status	Data::Read(const Offset&		offset,
				   const Size&			size,
				   elle::Region&		region) const
    {
      Size		length;

      enter();

      // check the operation's validity: note that since such out-of-bound
      // calls are frequent, no warning or error is raised. instead, no data
      // is simply returned.
      if (offset > this->region.size)
	leave();

      // set size to the maximum between the request size and the available
      // size.
      length = (this->region.size - offset) < size ?
	(this->region.size - offset) :
	size;

      // prepare the output region.
      if (region.Prepare(length) == elle::StatusError)
	escape("unable to prepare the output region");

      // set the current size.
      region.size = length;

      // read the data from the region.
      if (this->region.Read(offset,
			    region.contents,
			    region.size) == elle::StatusError)
	escape("unable to read the data");

      leave();
    }

    ///
    /// this method adjust the size of the data either by expanding or
    /// shrinking the region.
    ///
    elle::Status	Data::Adjust(const Size&		size)
    {
      enter();

      // first, make sure the region's capacity is large enough.
      if (this->region.Adjust(size) == elle::StatusError)
	escape("unable to adjust the region size");

      // then, manually set the region size.
      this->region.size = size;

      // set the data as dirty.
      this->_state = proton::StateDirty;

      leave();
    }

    ///
    /// this method returns the size of the data.
    ///
    elle::Status	Data::Capacity(Size&			size) const
    {
      enter();

      // set the size.
      size = this->region.size;

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Data, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    elle::Status	Data::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Data]" << std::endl;

      // dump the state.
      std::cout << alignment << elle::Dumpable::Shift << "[_State] "
		<< this->_state << std::endl;

      // dump the region attribute.
      if (this->region.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the region");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    elle::Status	Data::Serialize(elle::Archive&		archive) const
    {
      enter();

      // serialize the internal region.
      if (archive.Serialize(this->region) == elle::StatusError)
	escape("unable to serialize the internal region");

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    elle::Status	Data::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the region.
      if (archive.Extract(this->region) == elle::StatusError)
	escape("unable to extract the region");

      leave();
    }

  }
}
