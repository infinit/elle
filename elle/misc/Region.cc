//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /data/mycure/repositories/infinit/elle/misc/Region.cc
//
// created       julien quintard   [mon nov 12 23:26:42 2007]
// updated       julien quintard   [tue jul 28 18:43:15 2009]
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
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method wraps an already existing memory area without
    /// allocating any memory, hence creating a so-called chunk.
    ///
    Status		Region::Wrap(Byte*			contents,
				     Natural32			size)
    {
      // check if the operation is valid.
      if (this->type != Region::TypeUnknown)
	escape("wrong type");

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
	escape("wrong type");

      // set the type.
      this->type = Region::TypeBuffer;

      // set the attributes.
      this->contents = contents;
      this->size = size;

      leave();
    }

    ///
    /// this method can be used for pre-allocating the memory for
    /// an upcoming direct-copy assignment i.e without going through
    /// Assign().
    ///
    Status		Region::Prepare(const Natural32		capacity)
    {
      // check the type.
      if ((this->type != Region::TypeUnknown) &&
	  (this->type != Region::TypeBuffer))
	escape("wrong type");

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
    /// this method assigns a static data region, building a so-called
    /// static buffer.
    ///
    Status		Region::Assign(Byte*			contents,
				       Natural32		size)
    {
      // check the type.
      if (this->type != Region::TypeUnknown)
	escape("wrong type");

      // set the type.
      this->type = Region::TypeBuffer;

      // allocate the required memory.
      if ((this->contents = (Byte*)::malloc(size)) == NULL)
	escape(::strerror(errno));

      // copy the data.
      ::memcpy(this->contents, contents, size);

      // set the size and capacity that should now never be changed.
      this->size = size;
      this->capacity = size;

      leave();
    }

    ///
    /// this method expands the region. note however that, for the
    /// very first allocation, the exact size is allocated so that
    /// space is not wasted for static regions i.e regions build
    /// by just allocating memory, through Expand(), and then writing
    /// information.
    ///
    Status		Region::Expand(const Natural32		size)
    {
      // check the type.
      if ((this->type != Region::TypeUnknown) &&
	  (this->type != Region::TypeBuffer))
	escape("wrong type");

      // set the type.
      this->type = Region::TypeBuffer;

      // if the contents is NULL, then allocate it.
      if (this->contents == NULL)
	{
	  this->capacity = size;

	  if ((this->contents = (Byte*)::malloc(this->capacity)) == NULL)
	    escape(::strerror(errno));
	}

      // if there is enough space, just leave.
      if ((this->size + size) <= this->capacity)
	leave();

      // otherwise, enlarge the buffer's capacity by pre-allocating its
      // current size.
      this->capacity += this->size + size;

      if ((this->contents = (Byte*)::realloc(this->contents,
					     this->capacity)) == NULL)
	escape(::strerror(errno));

      leave();
    }

    ///
    /// this method adds data to the buffer region.
    ///
    Status		Region::Append(const Byte*		contents,
				       const Natural32		size)
    {
      // check the type.
      if ((this->type != Region::TypeUnknown) &&
	  (this->type != Region::TypeBuffer))
	escape("wrong type");

      // make sure the buffer is large enough.
      if (this->Expand(size) == StatusError)
	escape("unable to reserve enough space for the new piece");

      // copy the region into the buffer.
      if (this->Write(this->size, contents, size) == StatusError)
	escape("unable to append the data");

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
      if (offset + size > this->size)
	this->size = offset + size;

      leave();
    }

    ///
    /// this method detaches the data from the object, making
    /// the data still alive after the object's destruction.
    ///
    Status		Region::Detach()
    {
      // activate the option.
      this->options = Region::OptionDetach;

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this method initializes the attributes of the given object.
    ///
    Status		Region::New(Region&			region)
    {
      region.type = Region::TypeUnknown;
      region.options = Region::OptionNone;
      region.contents = NULL;
      region.size = 0;
      region.capacity = 0;

      leave();
    }

    ///
    /// this method releases the resources.
    ///
    Status		Region::Delete(Region&			region)
    {
      // release resources.
      if ((region.contents != NULL) &&
	  (region.type != Region::TypeChunk) &&
	  ((region.options & Region::OptionDetach) == 0))
	::free(region.contents);

      leave();
    }

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

      // delete the object.
      if ((Region::Delete(*this) == StatusError) ||
	  (Region::New(*this) == StatusError))
	yield("unable to reinitialize the object", *this);

      // according to the type...
      switch (element.type)
	{
	case TypeUnknown:
	  {
	    break;
	  }
	case TypeChunk:
	  {
	    this->type = element.type;
	    this->options = element.options;
	    this->contents = element.contents;
	    this->size = element.size;
	    this->capacity = element.capacity;

	    break;
	  }
	case TypeBuffer:
	  {
	    if (this->Assign(element.contents, element.size) == StatusError)
	      yield("unable to assign the element's data", *this);
	  }
	}

      return (*this);
    }

    ///
    /// this method check if two regions match.
    ///
    Boolean		Region::operator==(const Region&	element)
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
    Boolean		Region::operator!=(const Region&	element)
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the region's contents in hexadecimal.
    ///
    Status		Region::Dump(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      Natural32		i;

      std::cout << alignment
		<< "[Region] "
		<< "type(" << this->type << ") "
		<< "options(" << this->options << ") "
		<< "address(" << (void*)this->contents << ") "
		<< "size(" << this->size << ") "
		<< "capacity(" << this->capacity << ")" << std::endl;

      std::cout.setf(std::ios::hex, std::ios::basefield);

      for (i = 0; i < this->size; i++)
	{
	  String	shift(2, ' ');

	  if (i == 0)
	    std::cout << alignment << shift << (Natural32)this->contents[i]; // XXX[we should not have to force the case with proper io::<< functions]
	  else
	    std::cout << (Natural32)this->contents[i]; // XXX[we should not have to force the case with proper io::<< functions]

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

      std::cout.setf(std::ios::dec, std::ios::basefield);

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

    // XXX[find another solution like printing a Byte]
    stream.setf(std::ios::hex, std::ios::basefield);

    for (i = 0; i < element.size; i++)
      stream << (elle::core::Natural32)element.contents[i]; // XXX[we should not have to force the case with proper io::<< functions]

    stream.setf(std::ios::dec, std::ios::basefield);

    return (stream);
  }

}
