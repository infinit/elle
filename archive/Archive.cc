//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       infinit
//
// file          /home/mycure/infinit/elle/archive/Archive.cc
//
// created       julien quintard   [fri nov  2 10:03:53 2007]
// updated       julien quintard   [sun may  2 15:31:03 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/archive/Archive.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace system;

  namespace archive
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this magic number is inserted at the head of each archive.
    ///
    const Character		Archive::Magic[3] = { 'E', 'L', 'L' };

    ///
    /// archives are initialised with this default capacity.
    ///
    const Natural64		Archive::Default::Capacity = 64;

//
// ---------- templates -------------------------------------------------------
//

    ArchiveName(Null);
    ArchiveName(Boolean);
    ArchiveName(Character);
    ArchiveName(Real);
    ArchiveName(Integer8);
    ArchiveName(Integer16);
    ArchiveName(Integer32);
    ArchiveName(Integer64);
    ArchiveName(Natural8);
    ArchiveName(Natural16);
    ArchiveName(Natural32);
    ArchiveName(Natural64);
    ArchiveName(String);
    ArchiveName(Large);
    ArchiveName(Region);
    ArchiveName(Archive);

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initialises the attributes.
    ///
    Archive::Archive():
      mode(Archive::ModeUnknown),
      endianness(System::Endianness),
      offset(0)
    {
    }

    ///
    /// this is the copy constructor.
    ///
    Archive::Archive(const Archive&				archive):
      Region::Region(archive),

      mode(archive.mode),
      endianness(archive.endianness),
      offset(archive.offset)
    {
    }

    ///
    /// this method releases the resources.
    ///
    Archive::~Archive()
    {
    }

//
// ---------- prepare ---------------------------------------------------------
//

    ///
    /// this method prepares an archive for serializing.
    ///
    Status		Archive::Create()
    {
      Natural64		i;

      enter();

      // initialise the attributes.
      this->mode = ModeSerialization;
      this->endianness = System::Endianness;
      this->offset = 0;
      this->size = 0;
      this->capacity = 0;

      // initialise the default capacity.
      if (this->Reserve(Archive::Default::Capacity) == StatusError)
	escape("cannot allocate the default archive capacity");

      // pack the magic.
      for (i = 0; i < (sizeof(Archive::Magic) / sizeof(Character)); i++)
	{
	  if (this->Store(Archive::Magic[i]) == StatusError)
	    escape("unable to store the archive magic number");
	}

      // pack the endianness of the archive.
      if (this->Store((Natural8&)System::Endianness) == StatusError)
	escape("unable to store the archive endianness");

      leave();
    }

    ///
    /// this method prepares an archive for extraction.
    ///
    /// note that the given region is not duplicated such that it should
    /// not be freed by the caller. indeed, the archive takes over
    /// the region's ownership.
    ///
    /// as such the given region contents should have been detached
    /// before calling this method.
    ///
    Status		Archive::Prepare(const Region&		region)
    {
      Byte		endianness;
      Natural64		i;

      enter();

      // initialise the attributes.
      this->mode = Archive::ModeExtraction;
      this->offset = 0;
      this->capacity = region.size;

      // take over the region's ownership.
      if (this->Acquire(region.contents, region.size) == StatusError)
	escape("unable to take over the given region");

      // extract and verify the magic number.
      for (i = 0; i < (sizeof(Archive::Magic) / sizeof(Character)); i++)
	{
	  Character	magic;

	  if (this->Load(magic) == StatusError)
	    escape("unable to load the magic number from the archive");

	  if (Archive::Magic[i] != magic)
	    escape("this archive has a wrong magic number");
	}

      // extract the endianness.
      if (this->Load(endianness) == StatusError)
	escape("unable to load the archive endianness");

      // set the endianness.
      this->endianness = (System::Order)endianness;

      leave();
    }

//
// ---------- store -----------------------------------------------------------
//

    ///
    /// this method stores a Null-type Nil into the archive.
    ///
    /// since the purpose of the Null type is only to save space
    /// by storing the type only, this method does not do anything.
    ///
    Status		Archive::Store(const Null&)
    {
      enter();

      leave();
    }

    ///
    /// this method stores a BIGNUM into the archive.
    ///
    Status		Archive::Store(const Large&		element)
    {
      Natural32		size = BN_num_bytes(&element);

      enter();

      // store the length.
      if (this->Store(size) == StatusError)
	escape("unable to store the size");

      // prepare the buffer so that it can receive size of data.
      // that means enlarging the buffer and aligning the offset.
      if (this->Align(sizeof(Byte)) == StatusError)
	escape("unable to align the size");

      if (this->Reserve(size) == StatusError)
	escape("unable to enlarge the archive");

      // finally, directory copy the data into the archive.
      ::BN_bn2bin(&element, this->contents + this->size);

      // update the buffer size since we wrote in the
      // buffer directly without using the Store() method.
      this->size += size;

      leave();
    }

    ///
    /// this normal method does the same but for strings.
    ///
    Status		Archive::Store(const String&		element)
    {
      Natural32		length = element.length();

      enter();

      // start by storing the string length.
      if (this->Store(length) == StatusError)
	escape("unable to store the length");

      // align the size if it is necessary.
      if (this->Align(sizeof(char)) == StatusError)
	escape("unable to align the size");

      // possibly enlarge the archive and align the size.
      if (this->Reserve(length) == StatusError)
	escape("unable to reserve space for an upcoming serialization");

      // store the string contents.
      element.copy((char*)(this->contents + this->size), length);

      // update the size.
      this->size += length;

      leave();
    }

    ///
    /// this method stores a region.
    ///
    Status		Archive::Store(const Region&		element)
    {
      enter();

      // store the size.
      if (this->Store(element.size) == StatusError)
	escape("unable to store the size");

      // align the archive though, aligning on a Byte should
      // not be a problem...
      if (this->Align(sizeof(Byte)) == StatusError)
	escape("unable to align the size");

      // enlarge the buffer.
      if (this->Reserve(element.size) == StatusError)
	escape("unable to enlarge the archive");

      // copy the data into the archive.
      ::memcpy(this->contents + this->size, element.contents, element.size);

      // update the buffer size.
      this->size += element.size;

      leave();
    }

    ///
    /// this method stores an archive.
    ///
    Status		Archive::Store(const Archive&		element)
    {
      enter();

      // start by storing the string length.
      if (this->Store(element.size) == StatusError)
	escape("unable to store the size");

      // align the size if it is necessary.
      if (this->Align(sizeof(Byte)) == StatusError)
	escape("unable to align the size");

      // possibly enlarge the archive and align the size.
      if (this->Reserve(element.size) == StatusError)
	escape("unable to reserve space for an upcoming serialization");

      // store the string contents.
      ::memcpy(this->contents + this->size, element.contents, element.size);

      // update the size.
      this->size += element.size;

      leave();
    }

//
// ---------- load ------------------------------------------------------------
//

    ///
    /// this method loads a Null-type object.
    ///
    /// since nothing as been stored, this method does not return anything.
    ///
    Status		Archive::Load(Null&)
    {
      enter();

      leave();
    }

    ///
    /// this method loads a Large.
    ///
    Status		Archive::Load(Large&			element)
    {
      Natural32		size;

      enter();

      // load the data size.
      if (this->Load(size) == StatusError)
	escape("unable to load the data size");

      // initialize the big number.
      ::BN_init(&element);

      // if there is data.
      if (size > 0)
	{
	  // align the offset if it is necessary.
	  if (this->Align(sizeof(Byte)) == StatusError)
	    escape("unable to align the offset");

	  // load directly the bignum from the buffer.
	  ::BN_bin2bn(this->contents + this->offset, size, &element);
	}

      // update the offset.
      this->offset += size;

      leave();
    }

    ///
    /// this method loads a string.
    ///
    Status		Archive::Load(String&			element)
    {
      Natural32		length;

      enter();

      // load the string length.
      if (this->Load(length) == StatusError)
	escape("unable to load the string length");

      // if there is data.
      if (length > 0)
	{
	  // align the offset if it is necessary.
	  if (this->Align(sizeof(char)) == StatusError)
	    escape("unable to align the offset");

	  // assign the content to the string.
	  element.assign((char*)(this->contents + this->offset), length);
	}

      // update the offset.
      this->offset += length;

      leave();
    }

    ///
    /// this method loads a region.
    ///
    Status		Archive::Load(Region&			element)
    {
      Natural64		size;

      enter();

      // load the size.
      if (this->Load(size) == StatusError)
	escape("unable to load the size");

      // if there is data.
      if (size > 0)
	{
	  // align the offset if it is necessary.
	  if (this->Align(sizeof(Byte)) == StatusError)
	    escape("unable to align the offset");

	  // assign the data.
	  if (element.Duplicate(this->contents + this->offset,
				size) == StatusError)
	    escape("unable to prepare the buffer");
	}

      // update the offset.
      this->offset += size;

      leave();
    }

    ///
    /// this method loads an archive.
    ///
    Status		Archive::Load(Archive&			element)
    {
      Region		buffer;
      Natural64		size;

      enter();

      // load the string length.
      if (this->Load(size) == StatusError)
	escape("unable to load the archive size");

      // if there is data.
      if (size > 0)
	{
	  // align the offset if it is necessary.
	  if (this->Align(sizeof(Byte)) == StatusError)
	    escape("unable to align the offset");

	  // assign the content to the buffer.
	  if (buffer.Duplicate(this->contents + this->offset,
			       size) == StatusError)
	    escape("unable to assign the data to the buffer");

	  // detach the data from the buffer.
	  if (buffer.Detach() == StatusError)
	    escape("unable to detach the data from the buffer");

	  // build the given archive.
	  if (element.Prepare(buffer) == StatusError)
	    escape("unable to prepare the given archive");
	}

      // update the offset.
      this->offset += size;

      leave();
    }

//
// ---------- util ------------------------------------------------------------
//

    ///
    /// this method makes sure that there is enough space for storing
    /// an element of the given type.
    ///
    Status		Archive::Reserve(const Natural64	size)
    {
      Natural64		capacity;

      enter();

      // check the mode as this method makes sense only in the
      // serialization process.
      if (this->mode != Archive::ModeSerialization)
	leave();

      // save the capacity.
      capacity = this->capacity;

      // expand the internal buffer.
      if (this->Adjust(this->size + size) == StatusError)
	escape("unable to expand the archive");

      // set the new memory to zero, only if the capacity has changed.
      if (this->capacity != capacity)
	::memset(this->contents + this->size,
		 0x0,
		 this->capacity - this->size);

      leave();
    }

    ///
    /// this method aligns the offset so that archiving or extracting
    /// an element of the given type does not cause any trouble.
    ///
    /// note that this method should be called before Reserve().
    /// indeed, otherwise, reserving and aligning could result
    /// with a remaining space smaller that the one required
    /// through the Reserve() call.
    ///
    /// as such, the Align() method could need to reserve space.
    /// so that the archive is aligned.
    ///
    Status		Archive::Align(const Natural64		size)
    {
      enter();

      // align depending on the mode.
      switch (this->mode)
	{
	case Archive::ModeSerialization:
	  {
	    // reserve more memory if necessary.
	    if (this->Reserve(size) == StatusError)
	      escape("unable to reserve memory for alignment");

	    // align the size in order to serialize the next element
	    // of _size_ bytes.
	    this->size = (this->size + (size - 1)) & ~(size - 1);

	    break;
	  }
	case Archive::ModeExtraction:
	  {
	    // align the offset so that the next element of _size_ bytes
	    // can be extract.
	    this->offset = (this->offset + (size - 1)) & ~(size - 1);

	    // verify that the offset plus the about-to-be-extracted
	    // element is not going to exceed the size.
	    if ((this->offset + size) > this->size)
	      escape("the offset has reached or exceeded the archive size");

	    break;
	  }
	case Archive::ModeUnknown:
	  {
	    escape("this code should not have been reached");

	    break;
	  }
	}

      leave();
    }

//
// ---------- fetch -----------------------------------------------------------
//

    ///
    /// this method fetches the type of the upcoming element.
    ///
    /// note that this method does not advance the offset.
    ///
    Status		Archive::Fetch(Archive::Type&		type)
    {
      Natural64		offset = this->offset;
      Byte		byte;

      enter();

      // load the next byte which should be a type.
      if (this->Load(byte) == StatusError)
	escape("unable to load the next type");

      // set the type.
      type = (Archive::Type)byte;

      // setting back the original offset.
      this->offset = offset;

      leave();
    }

//
// ---------- rewind ----------------------------------------------------------
//

    ///
    /// this method rewinds the offset to the beginning of the archive.
    ///
    Status		Archive::Rewind()
    {
      Region		chunk;

      enter();

      // detach the data from the archive so that nothing is freed.
      if (this->Detach() == StatusError)
	escape("unable to detach the archive's data");

      // wrap the archive's content so that the data is not lost.
      if (chunk.Wrap(this->contents, this->size) == StatusError)
	escape("unable to wrap the archive's content");

      // recycle the archive.
      if (this->Recycle<Archive>() == StatusError)
	escape("unable to recycle the archive");

      // prepare the archive for extraction.
      if (this->Prepare(chunk) == StatusError)
	escape("unable to prepare the archive for extraction");

      leave();
    }

//
// ---------- seal ------------------------------------------------------------
//

    ///
    /// this method seals the serialization archive and transforms it into
    /// into an extraction archive.
    ///
    /// this method should not be used but for debugging purposes.
    ///
    Status		Archive::Seal()
    {
      Region		chunk;

      enter();

      // detach the data from the archive so that nothing is freed.
      if (this->Detach() == StatusError)
	escape("unable to detach the archive's data");

      // wrap the archive's content so that the data is not lost.
      if (chunk.Wrap(this->contents, this->size) == StatusError)
	escape("unable to wrap the archive's content");

      // recycle the archive.
      if (this->Recycle<Archive>() == StatusError)
	escape("unable to recycle the archive");

      // prepare the archive for extraction.
      if (this->Prepare(chunk) == StatusError)
	escape("unable to prepare the archive for extraction");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an archive i.e displays every recored element
    /// with its type and value.
    ///
    Status		Archive::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      Archive		archive;

      enter();

      // first copy the archive because we cannot modify it.
      archive = *this;

      std::cout << alignment
		<< "[Archive] "
		<< "mode(" << std::dec << (Natural8)archive.mode << ") "
		<< "offset(" << std::dec << archive.offset << ") "
		<< "size(" << std::dec << archive.size << ") "
		<< "capacity(" << std::dec << archive.capacity << ")"
		<< std::endl;

      // rewind the archive.
      if (archive.Rewind() == StatusError)
	escape("unable to rewind the archive");

      while (archive.offset != archive.size)
	{
	  Archive::Type		type;

	  if (archive.Fetch(type) == StatusError)
	    escape("unable to fetch the next element's type");

	  if (this->offset == archive.offset)
	    std::cout << alignment << "> " << std::endl;

	  switch (type)
	    {
	    case Archive::TypeNull:
	      {
		if (archive.Extract(none) == StatusError)
		  escape("unable to extract the next element");

		if (Print(none, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeBoolean:
	      {
		Boolean		value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Print(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeCharacter:
	      {
		Character	value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Print(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeReal:
	      {
		Real		value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Print(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeInteger8:
	      {
		Integer8	value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Print(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeInteger16:
	      {
		Integer16	value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Print(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeInteger32:
	      {
		Integer32	value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Print(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeInteger64:
	      {
		Integer64	value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Print(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeNatural8:
	      {
		Natural8	value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Print(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeNatural16:
	      {
		Natural16	value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Print(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeNatural32:
	      {
		Natural32	value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Print(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeNatural64:
	      {
		Natural64	value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Print(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeLarge:
	      {
		Large		value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Print(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		::BN_clear_free(&value);

		break;
	      }
	    case Archive::TypeString:
	      {
		String		value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Print(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeRegion:
	      {
		Region		value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Print(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeArchive:
	      {
		Archive		value;

		if (archive.Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (value.Dump(margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeUnknown:
	    default:
	      {
		escape("unknown element type");

		break;
	      }
	    }
	}

      leave();
    }

//
// ---------- object-like -----------------------------------------------------
//

    ///
    /// this method returns the size of the archive object.
    ///
    Status		Archive::Imprint(Natural32&		size) const
    {
      enter();

      // return the size.
      size = sizeof(Archive);

      leave();
    }

    ///
    /// this method clones the current archive.
    ///
    Status		Archive::Clone(Archive*&		object) const
    {
      enter();

      // allocate the object.
      object = new Archive(*this);

      leave();
    }

    ///
    /// this method copies a archive.
    ///
    Archive&		Archive::operator=(const Archive&	element)
    {
      enter();

      // test if the archives are identical.
      if (this == &element)
	return (*this);

      // recycle the object.
      if (this->Recycle(&element) == StatusError)
	yield("unable to recycle the object", *this);

      return (*this);
    }

    ///
    /// this method check if two archives match.
    ///
    Boolean		Archive::operator==(const Archive&	element) const
    {
      enter();

      // call the super-method.
      if (Archive::operator==(element) == false)
	false();

      true();
    }

    ///
    /// this method compares two archives.
    ///
    Boolean		Archive::operator!=(const Archive&	element) const
    {
      return (!(*this == element));
    }

  }
}
