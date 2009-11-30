//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/misc/Region.cc
//
// created       julien quintard   [mon nov 12 23:26:42 2007]
// updated       julien quintard   [sat nov 28 16:45:40 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Region.hh>

namespace elle
{
  using namespace core;
  using namespace misc;

  namespace misc
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
      Entity::Entity(region),

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
	      alert("unable to assign the element's data");
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
	  (this->options & Region::OptionDetach) == 0)
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
				     Natural32			size)
    {
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
					Natural32		size)
    {
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
    Status		Region::Prepare(const Natural32		capacity)
    {
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
					  Natural32		size)
    {
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
    Status		Region::Adjust(const Natural32		size)
    {
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
				       const Natural32		size)
    {
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
    Status		Region::Read(const Natural32		offset,
				     Byte*			contents,
				     const Natural32		size) const
    {
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
    Status		Region::Write(const Natural32		offset,
				      const Byte*		contents,
				      const Natural32		size)
    {
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
      // check the type.
      if (this->type != Region::TypeBuffer)
	escape("unable to detach non-buffer regions");

      // activate the option.
      this->options = Region::OptionDetach;

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// assign the given region by duplicating the attributes.
    ///
    /// note that for a chunk, the memory is not copied.
    ///
    Region&		Region::operator=(const Region&		element)
    {
      // self-check
      if (this == &element)
	return (*this);

      // recycle the object.
      if (this->Recycle<Region>(&element) == StatusError)
	yield("unable to recycle the object", *this);

      return (*this);
    }

    ///
    /// this method check if two regions match.
    ///
    Boolean		Region::operator==(const Region&	element) const
    {
      // check the size.
      if (this->size != element.size)
	false();

      // check the content.
      if (::memcmp(this->contents, element.contents, element.size) != 0)
	false();

      true();
    }

    ///
    /// this method checks if two regions dis-match.
    ///
    Boolean		Region::operator!=(const Region&	element) const
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the region's contents in hexadecimal.
    ///
    Status		Region::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      Natural32		i;

      std::cout << alignment
		<< "[Region] "
		<< "type(" << this->type << ") "
		<< "options(" << this->options << ") "
		<< "address(" << (void*)this->contents << ") "
		<< "size(" << std::dec << this->size << ") "
		<< "capacity(" << std::dec << this->capacity << ")"
		<< std::endl;

      for (i = 0; i < this->size; i++)
	{
	  String	shift(2, ' ');

	  if (i == 0)
	    std::cout << alignment << shift << std::nouppercase << std::hex
		      << (Natural32)this->contents[i];
	  else
	    std::cout << std::hex << std::nouppercase
		      << (Natural32)this->contents[i];

	  if (((i + 1) % 20) == 0)
	    {
	      std::cout << std::endl;

	      if ((i + 1) < this->size)
		std::cout << alignment << shift;
	    }
	  else
	    {
	      if ((i + 1) < this->size)
		std::cout << " ";
	      else
		std::cout << std::endl;
	    }
	}

      leave();
    }

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ///
  /// this operator renders a region.
  ///
  std::ostream&		operator<<(std::ostream&		stream,
				   const elle::misc::Region&	element)
  {
    elle::core::Natural32	i;

    for (i = 0; i < element.size; i++)
      stream << std::nouppercase << std::hex
	     << (elle::core::Natural32)element.contents[i];

    return (stream);
  }

}
