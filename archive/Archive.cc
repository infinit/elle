//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       GPL
//
// file          /home/mycure/infinit/elle/archive/Archive.cc
//
// created       julien quintard   [fri nov  2 10:03:53 2007]
// updated       julien quintard   [wed mar 11 16:08:43 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/archive/Archive.hh>

namespace elle
{
  using namespace core;
  using namespace misc;
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
    const Natural32		Archive::Default::Capacity = 64;

    ///
    /// this definition is for internal use only.
    ///
    Void			vacuum;

//
// ---------- templates -------------------------------------------------------
//

    ArchiveName(Void);
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
// ---------- prepare ---------------------------------------------------------
//

    ///
    /// this method prepares an archive for serializing.
    ///
    Status		Archive::Create()
    {
      Natural32		i;

      // initialise the attributes.
      this->mode = ModeSerialization;
      this->endianness = System::MachineEndianness;
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
      if (this->Store((Byte)System::MachineEndianness) == StatusError)
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
      Natural32		i;

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

      this->endianness = (System::Endianness)endianness;

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
      leave();
    }

    ///
    /// this method stores a BIGNUM into the archive.
    ///
    Status		Archive::Store(const Large&		element)
    {
      Natural32		size = BN_num_bytes(&element);

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
      leave();
    }

    ///
    /// this method loads a Large.
    ///
    Status		Archive::Load(Large&			element)
    {
      Natural32		size;

      // load the data size.
      if (this->Load(size) == StatusError)
	escape("unable to load the data size");

      // align the offset if it is necessary.
      if (this->Align(sizeof(Byte)) == StatusError)
	escape("unable to align the offset");

      // initialize the big number.
      ::BN_init(&element);

      // load directly the bignum from the buffer.
      ::BN_bin2bn(this->contents + this->offset, size, &element);

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

      // load the string length.
      if (this->Load(length) == StatusError)
	escape("unable to load the string length");

      // align the offset if it is necessary.
      if (this->Align(sizeof(char)) == StatusError)
	escape("unable to align the offset");

      // assign the content to the string.
      element.assign((char*)(this->contents + this->offset), length);

      // update the offset.
      this->offset += length;

      leave();
    }

    ///
    /// this method loads a region.
    ///
    Status		Archive::Load(Region&			element)
    {
      Natural32		size;

      // load the size.
      if (this->Load(size) == StatusError)
	escape("unable to load the size");

      // align the offset if it is necessary.
      if (this->Align(sizeof(Byte)) == StatusError)
	escape("unable to align the offset");

      // assign the data.
      if (element.Assign(this->contents + this->offset, size) == StatusError)
	escape("unable to prepare the buffer");

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
      Natural32		size;

      // load the string length.
      if (this->Load(size) == StatusError)
	escape("unable to load the archive size");

      // align the offset if it is necessary.
      if (this->Align(sizeof(Byte)) == StatusError)
	escape("unable to align the offset");

      // assign the content to the buffer.
      if (buffer.Assign(this->contents + this->offset, size) == StatusError)
	escape("unable to assign the data to the buffer");

      // detach the data from the buffer.
      if (buffer.Detach() == StatusError)
	escape("unable to detach the data from the buffer");

      // build the given archive.
      if (element.Prepare(buffer) == StatusError)
	escape("unable to prepare the given archive");

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
    Status		Archive::Reserve(const Natural32	size)
    {
      // check the mode as this method makes sense only in the
      // serialization process.
      if (this->mode != Archive::ModeSerialization)
	leave();

      // expand the internal buffer.
      if (this->Expand(size) == StatusError)
	escape("unable to expand the archive");

      // set the new memory to zero.
      ::memset(this->contents + this->size, 0x0, this->capacity - this->size);

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
    Status		Archive::Align(const Natural32		size)
    {
      if (this->Reserve(size) == StatusError)
	escape("unable to reserve memory for alignment");

      switch (this->mode)
	{
	case Archive::ModeSerialization:
	  {
	    this->size = (this->size + (size - 1)) & ~(size - 1);

	    break;
	  }
	case Archive::ModeExtraction:
	  {
	    this->offset = (this->offset + (size - 1)) & ~(size - 1);

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
      Natural32		offset = this->offset;
      Byte		byte;

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
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method initialises the attributes.
    ///
    Status		Archive::New(Archive&			archive)
    {
      // initialise the attributes.
      archive.mode = Archive::ModeUnknown;
      archive.endianness = System::MachineEndianness;
      archive.offset = 0;

      leave();
    }

    ///
    /// this method releases the resources.
    ///
    Status		Archive::Delete(Archive&)
    {
      leave();
    }

    ///
    /// assign the given archive by duplicating the attributes.
    ///
    /// note that the memory is duplicated.
    ///
    Archive&		Archive::operator=(const Archive&	element)
    {
      // self-check
      if (this == &element)
	return (*this);

      // call the parent assignment.
      Region::operator=(element);

      // re-new the object.
      if ((Archive::Delete(*this) == StatusError) ||
	  (Archive::New(*this) == StatusError))
	yield("unable to re-initialize the object", *this);

      // duplicate the attributes.
      this->mode = element.mode;
      this->endianness = element.endianness;
      this->offset = element.offset;

      return (*this);
    }

    ///
    /// this method check if two archives match.
    ///
    Boolean		Archive::operator==(const Archive&	element)
    {
      // call the super-method.
      if (Region::operator==(element) == false)
	false();

      // check the mode.
      if (this->mode != element.mode)
	false();

      // check the endianness.
      if (this->endianness != element.endianness)
	false();

      true();
    }

    ///
    /// this method checks if two archive dis-match.
    ///
    Boolean		Archive::operator!=(const Archive&	element)
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps an archive i.e displays every recored element
    /// with its type and value.
    ///
    Status		Archive::Dump(const Natural32		margin)
    {
      Natural32		offset = this->offset;
      String		alignment(margin, ' ');

      std::cout << alignment
		<< "[Archive] "
		<< "mode(" << this->mode << ") "
		<< "offset(" << this->offset << ") "
		<< "size(" << this->size << ") "
		<< "capacity(" << this->capacity << ")"
		<< std::endl;

      while (this->offset != this->size)
	{
	  Archive::Type		type;

	  if (this->Fetch(type) == StatusError)
	    escape("unable to fetch the next element's type");

	  switch (type)
	    {
	    case Archive::TypeNull:
	      {
		if (this->Extract(none) == StatusError)
		  escape("unable to extract the next element");

		if (Show(none, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeBoolean:
	      {
		Boolean		value;

		if (this->Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Show(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeCharacter:
	      {
		Character	value;

		if (this->Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Show(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeReal:
	      {
		Real		value;

		if (this->Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Show(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeInteger8:
	      {
		Integer8	value;

		if (this->Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Show(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeInteger16:
	      {
		Integer16	value;

		if (this->Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Show(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeInteger32:
	      {
		Integer32	value;

		if (this->Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Show(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeInteger64:
	      {
		Integer64	value;

		if (this->Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Show(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeNatural8:
	      {
		Natural8	value;

		if (this->Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Show(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeNatural16:
	      {
		Natural16	value;

		if (this->Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Show(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeNatural32:
	      {
		Natural32	value;

		if (this->Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Show(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeNatural64:
	      {
		Natural64	value;

		if (this->Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Show(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeLarge:
	      {
		Large		value;

		if (this->Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Show(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeString:
	      {
		String		value;

		if (this->Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Show(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeRegion:
	      {
		Region		value;

		if (this->Extract(value) == StatusError)
		  escape("unable to extract the next element");

		if (Show(value, margin + 2) == StatusError)
		  escape("unable to print the element");

		break;
	      }
	    case Archive::TypeArchive:
	      {
		Archive		value;

		if (this->Extract(value) == StatusError)
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

      // put the offset right back where it was.
      this->offset = offset;

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

      // wrap the archive's content so that the data is not lost.
      if (chunk.Wrap(this->contents, this->size) == StatusError)
	escape("unable to wrap the archive's content");

      // detach the data from the archive so that nothing is freed.
      if (this->Detach() == StatusError)
	escape("unable to detach the archive's data");

      // reinitialize the object.
      this->~Archive();

      // prepare the archive for extraction.
      if (this->Prepare(chunk) == StatusError)
	escape("unable to prepare the archive for extraction");

      leave();
    }

//
// ---------- XXX -------------------------------------------------------------
//

    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    /* XXX
    ///
    /// this method unpacks a four-byte integer from the archive.
    ///
    Status		Archive::ExtractInteger(uint32_t*	variable)
    {
      uint32_t		size = sizeof(uint32_t);

      // unpack the element.
      if (this->endianness == this->Endianness)
	{
	  *variable = *((uint32_t*)(this->contents + this->offset));
	}
      else
	{
	  *variable = (uint32_t)(this->contents[this->offset + 0] << 24);
	  *variable |= (uint32_t)(this->contents[this->offset + 1] << 16);
	  *variable |= (uint32_t)(this->contents[this->offset + 2] << 8);
	  *variable |= (uint32_t)(this->contents[this->offset + 3]);
	}

      // update the offset.
      this->offset = this->offset + size;

      leave();
    }

    ///
    /// this method unpacks a eight-byte integer from the archive.
    ///
    Status		Archive::ExtractQuad(uint64_t*		variable)
    {
      uint32_t		size = sizeof(uint64_t);

      // unpack the element.
      if (this->endianness == this->Endianness)
	{
	  *variable = *((uint64_t*)(this->contents + this->offset));
	}
      else
	{
	  *variable = ((uint64_t)this->contents[this->offset + 0]) << 56;
	  *variable |= ((uint64_t)this->contents[this->offset + 1]) << 48;
	  *variable |= ((uint64_t)this->contents[this->offset + 2]) << 40;
	  *variable |= ((uint64_t)this->contents[this->offset + 3]) << 32;
	  *variable |= ((uint64_t)this->contents[this->offset + 4]) << 24;
	  *variable |= ((uint64_t)this->contents[this->offset + 5]) << 16;
	  *variable |= ((uint64_t)this->contents[this->offset + 6]) << 8;
	  *variable |= ((uint64_t)this->contents[this->offset + 7]);
	}

      // update the offset.
      this->offset = this->offset + size;

      leave();
    }
  */

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ///
  /// this operator renders an archive.
  ///
  std::ostream&		operator<<(std::ostream&		stream,
				   const elle::archive::Archive& element)
  {
    // render the archive as a region.
    stream << (elle::misc::Region&)element;

    return (stream);
  }

}
