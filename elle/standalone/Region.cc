//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/standalone/Region.cc
//
// created       julien quintard   [mon nov 12 23:26:42 2007]
// updated       julien quintard   [wed mar 23 15:11:50 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Region.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;

  namespace standalone
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Region::Region():
      type(Region::TypeUnknown),
      options(Region::OptionNone),
      contents(NULL),
      size(0),
      capacity(0)
    {
    }

    ///
    /// copy constructor.
    ///
    Region::Region(const Region&				region):
      type(region.type),
      options(region.options),
      contents(NULL),
      size(0),
      capacity(0)
    {
      // according to the type...
      switch (region.type)
	{
	case TypeUnknown:
	  {
	    break;
	  }
	case TypeChunk:
	  {
	    this->contents = region.contents;
	    this->size = region.size;
	    this->capacity = region.capacity;

	    break;
	  }
	case TypeBuffer:
	  {
	    this->options = Region::OptionNone;

	    if (this->Duplicate(region.contents, region.size) == StatusError)
	      fail("unable to assign the element's data");
	  }
	}
    }

    ///
    /// default destructor.
    ///
    Region::~Region()
    {
      // release resources.
      if ((this->contents != NULL) &&
	  (this->type == Region::TypeBuffer) &&
	  ((this->options & Region::OptionDetach) == 0))
	::free(this->contents);
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method wraps an already existing memory area without
    /// allocating any memory, hence creating a so-called chunk.
    ///
    Status		Region::Wrap(Byte*			contents,
				     Natural64			size)
    {
      enter();

      // check if the operation is valid.
      if (this->type != Region::TypeUnknown)
	escape("region already in use");

      // set the type.
      this->type = Region::TypeChunk;

      // set the attributes.
      this->contents = contents;
      this->size = size;

      leave();
    }

    ///
    /// this method takes over the ownership of the given memory area.
    ///
    Status		Region::Acquire(Byte*			contents,
					Natural64		size)
    {
      enter();

      // check if the operation is valid.
      if (this->type != Region::TypeUnknown)
	escape("region already in use");

      // set the type.
      this->type = Region::TypeBuffer;

      // set the attributes.
      this->contents = contents;
      this->size = size;
      this->capacity = size;

      leave();
    }

    ///
    /// this method can be used for pre-allocating the memory for
    /// an upcoming direct-copy assignment.
    ///
    Status		Region::Prepare(const Natural64		capacity)
    {
      enter();

      // check the type.
      if (this->type == Region::TypeChunk)
	escape("unable to prepare an already in use chunk region");

      // set the type.
      this->type = Region::TypeBuffer;

      // allocate memory.
      if ((this->contents = (Byte*)::realloc(this->contents,
					     capacity)) == NULL)
	escape(::strerror(errno));

      // update the capacity. note that the size should be updated
      // manually when the direct copy is made.
      this->size = 0;
      this->capacity = capacity;

      leave();
    }

    ///
    /// this method assigns a data region, building a so-called buffer.
    ///
    Status		Region::Duplicate(Byte*			contents,
					  Natural64		size)
    {
      enter();

      // check the type.
      if (this->type == Region::TypeChunk)
	escape("unable to prepare an already in use chunk region");

      // set the type.
      this->type = Region::TypeBuffer;

      // allocate the required memory.
      if ((this->contents = (Byte*)::realloc(this->contents,
					     size)) == NULL)
	escape(::strerror(errno));

      // copy the data.
      ::memcpy(this->contents, contents, size);

      // set the size and capacity that should now never be changed.
      this->size = size;
      this->capacity = size;

      leave();
    }

    ///
    /// this method adjusts the region. note however that, for the
    /// very first allocation, the exact size is allocated so that
    /// space is not wasted for static regions i.e regions build
    /// by just allocating memory, through Adjust(), and then writing
    /// information.
    ///
    /// note that this function is used to both truncate and extend the
    /// region. besides, note that this method does not set the size but
    /// only deals with making sure the capacity is sufficient.
    ///
    Status		Region::Adjust(const Natural64		size)
    {
      enter();

      // check the type.
      if (this->type == Region::TypeChunk)
	escape("unable to prepare an already in use chunk region");

      // if the region is a virgin.
      if (this->type == Region::TypeUnknown)
	{
	  if (this->Prepare(size) == StatusError)
	    escape("unable to prepare the region for the first time");
	}
      else
	{
	  // if there is enough space, just leave.
	  if (size <= this->capacity)
	    leave();

	  // otherwise, enlarge the buffer's capacity.
	  this->capacity = size;

	  if ((this->contents = (Byte*)::realloc(this->contents,
						 this->capacity)) == NULL)
	    escape(::strerror(errno));
	}

      leave();
    }

    ///
    /// this method adds data to the buffer region.
    ///
    Status		Region::Append(const Byte*		contents,
				       const Natural64		size)
    {
      enter();

      // check the type.
      if (this->type == Region::TypeChunk)
	escape("unable to prepare an already in use chunk region");

      // make sure the buffer is large enough.
      if (this->Adjust(this->size + size) == StatusError)
	escape("unable to reserve enough space for the new piece");

      // copy the region into the buffer.
      if (this->Write(this->size, contents, size) == StatusError)
	escape("unable to append the data");

      leave();
    }

    ///
    /// this method allows the caller to read data anywhere in the
    /// region.
    ///
    Status		Region::Read(const Natural64		offset,
				     Byte*			contents,
				     const Natural64		size) const
    {
      enter();

      // check that the copy stays in the bounds.
      if ((offset + size) > this->size)
	escape("this operation is out of bounds");

      // copy the data.
      ::memcpy(contents, this->contents + offset, size);

      leave();
    }

    ///
    /// this method allows the caller to write data anywhere in the
    /// region.
    ///
    Status		Region::Write(const Natural64		offset,
				      const Byte*		contents,
				      const Natural64		size)
    {
      enter();

      // check that the copy stays in the bounds.
      if ((offset + size) > this->capacity)
	escape("this operation is out of bounds");

      // copy the data.
      ::memcpy(this->contents + offset, contents, size);

      // update the size.
      if ((offset + size) > this->size)
	this->size = offset + size;

      leave();
    }

    ///
    /// this method detaches the data from the object, making
    /// the data still alive after the object's destruction.
    ///
    Status		Region::Detach()
    {
      enter();

      // check the type.
      if (this->type != Region::TypeBuffer)
	escape("unable to detach non-buffer regions");

      // activate the option.
      this->options = Region::OptionDetach;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the region's contents in hexadecimal.
    ///
    /// note that the Hexadecimal utility class is not used to avoid
    /// dependencies since this class is critical.
    ///
    Status		Region::Dump(const Natural32		margin) const
    {
      Natural32		space = 78 - margin - Dumpable::Shift.length();
      String		alignment(margin, ' ');
      Natural32		i;
      Natural32		j;

      enter();

      std::cout << alignment
		<< "[Region] "
		<< "type(" << this->type << ") "
		<< "options(" << this->options << ") "
		<< "address(" << static_cast<Void*>(this->contents) << ") "
		<< "size(" << std::dec << this->size << ") "
		<< "capacity(" << std::dec << this->capacity << ")"
		<< std::endl;

      // since a byte is represented by two characters.
      space = space / 2;

      // set the fill to '0'.
      std::cout.fill('0');

      // display the region.
      for (i = 0; i < (this->size / space); i++)
	{
	  std::cout << alignment << Dumpable::Shift;

	  for (j = 0; j < space; j++)
	    std::cout << std::nouppercase
		      << std::hex
		      << std::setw(2)
		      << (Natural32)this->contents[i * space + j];

	  std::cout << std::endl;
	}

      if ((this->size % space) != 0)
	{
	  std::cout << alignment << Dumpable::Shift;

	  for (j = 0; j < (this->size % space); j++)
	    std::cout << std::nouppercase
		      << std::hex
		      << std::setw(2)
		      << (Natural32)this->contents[i * space + j];

	  std::cout << std::endl;
	}

      leave();
    }

//
// ---------- object-like -----------------------------------------------------
//

    ///
    /// this method returns the size of the region object.
    ///
    Status		Region::Imprint(Natural32&		size) const
    {
      enter();

      // return the size.
      size = sizeof(Region);

      leave();
    }

    ///
    /// this method clones the current region.
    ///
    Status		Region::Clone(Region*&			object) const
    {
      enter();

      // allocate the object.
      object = new Region(*this);

      leave();
    }

    ///
    /// this method check if two regions match.
    ///
    Boolean		Region::operator==(const Region&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // check the size.
      if (this->size != element.size)
	false();

      // check the content.
      if (::memcmp(this->contents, element.contents, element.size) != 0)
	false();

      true();
    }

    ///
    /// this operator compares two regions.
    ///
    Boolean		Region::operator<(const Region&) const
    {
      enter();

      flee("this method should never have been called");
    }

    ///
    /// this operator compares two regions.
    ///
    Boolean		Region::operator>(const Region&) const
    {
      enter();

      flee("this method should never have been called");
    }

    ///
    /// this method copies a region.
    ///
    Region&		Region::operator=(const Region&		element)
    {
      enter();

      // test if the regions are identical.
      if (this == &element)
	return (*this);

      // recycle the region.
      if (this->Recycle(&element) == StatusError)
	yield("unable to recycle the region", *this);

      return (*this);
    }

    ///
    /// this operator compares two regions. the source code of this
    /// function can be automatically generated through the Embed(Region, T)
    /// macro function.
    ///
    Boolean		Region::operator!=(const Region&	element) const
    {
      return (!(*this == element));
    }

    ///
    /// this operator compares two regions.
    ///
    Boolean		Region::operator<=(const Region&) const
    {
      enter();

      flee("this method should never have been called");
    }

    ///
    /// this operator compares two regions.
    ///
    Boolean		Region::operator>=(const Region&) const
    {
      enter();

      flee("this method should never have been called");
    }

  }
}
