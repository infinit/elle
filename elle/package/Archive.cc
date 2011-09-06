//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/package/Archive.cc
//
// created       julien quintard   [fri nov  2 10:03:53 2007]
// updated       julien quintard   [tue sep  6 20:43:12 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/package/Archive.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace system;

  namespace package
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Archive::Archive():
      mode(ModeUnknown),
      control(ControlUnknown),
      contents((Byte*&)static_cast<msgpack_sbuffer*>(&buffer)->data),
      size(static_cast<msgpack_sbuffer*>(&buffer)->size),
      offset(0)
    {
    }

    ///
    /// this is the copy constructor.
    ///
    Archive::Archive(const Archive&				archive):
      mode(archive.mode),
      control(archive.control),
      contents((Byte*&)static_cast<msgpack_sbuffer*>(&buffer)->data),
      size(static_cast<msgpack_sbuffer*>(&buffer)->size),
      offset(archive.offset)
    {
      // duplicate the buffer.
      this->buffer.write((const char*)archive.contents, archive.size);
    }

    ///
    /// this method releases the resources.
    ///
    Archive::~Archive()
    {
      Region	region;

      enter();

      // depending on the control policy.
      switch (this->control)
	{
	case Archive::ControlAcquired:
	  {
	    //
	    // if the control has been acquired, release the memory.
	    //

	    // create a region from the acquired buffer so that the region gets
	    // deleted at the end of this scope.
	    if (region.Acquire(
		  (Byte*)static_cast<msgpack_sbuffer*>(&this->buffer)->data,
		  static_cast<msgpack_sbuffer*>(&this->buffer)->size) ==
		StatusError)
	      yield(_(), "unable to acquire the buffer");

	    // reset the buffer's attributes in order to prevent double frees.
	    static_cast<msgpack_sbuffer*>(&buffer)->data = NULL;
	    static_cast<msgpack_sbuffer*>(&buffer)->size = 0;

	    break;
	  }
	case Archive::ControlWrapped:
	  {
	    //
	    // if the region to extract has been wrapped, reset the
	    // buffer's attributes as the memory region should not be
	    // released.
	    //

	    // reset the buffer's attributes in order to prevent double frees.
	    static_cast<msgpack_sbuffer*>(&buffer)->data = NULL;
	    static_cast<msgpack_sbuffer*>(&buffer)->size = 0;

	    break;
	  }
	default:
	  {
	    //
	    // nothing to do: the buffer will get deleted normally.
	    //

	    break;
	  }
	}

      release();
    }

//
// ---------- prepare ---------------------------------------------------------
//

    ///
    /// this method prepares the archive for serialization.
    ///
    Status		Archive::Create()
    {
      enter();

      // set the mode.
      this->mode = Archive::ModeSerialization;

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
    Status		Archive::Acquire(const Region&		region)
    {
      enter();

      // set the mode.
      this->mode = Archive::ModeExtraction;

      // release the existing buffer.
      if (static_cast<msgpack_sbuffer*>(&buffer)->data != NULL)
	::free(static_cast<msgpack_sbuffer*>(&buffer)->data);

      // set the buffer's attributes.
      static_cast<msgpack_sbuffer*>(&buffer)->data = (char*)region.contents;
      static_cast<msgpack_sbuffer*>(&buffer)->size = (size_t)region.size;

      // set the control: in this case, the ownership is acquired.
      this->control = Archive::ControlAcquired;

      leave();
    }

    ///
    /// this method prepares the archive by wrapping the given region.
    ///
    /// note that should the given region be deleted, the archive would
    /// be automatically rendered invalid.
    ///
    Status		Archive::Wrap(const Region&		region)
    {
      enter();

      // set the mode.
      this->mode = Archive::ModeExtraction;

      // release the existing buffer.
      if (static_cast<msgpack_sbuffer*>(&buffer)->data != NULL)
	::free(static_cast<msgpack_sbuffer*>(&buffer)->data);

      // set the buffer's attributes.
      static_cast<msgpack_sbuffer*>(&buffer)->data = (char*)region.contents;
      static_cast<msgpack_sbuffer*>(&buffer)->size = (size_t)region.size;

      // set the control: in this case, the archive simply wraps the given
      // region.
      this->control = Archive::ControlWrapped;

      leave();
    }

//
// ---------- serialize/extract -----------------------------------------------
//

    ///
    /// this method serializes nothing.
    ///
    Status		Archive::Serialize()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method extracts nothing.
    ///
    Status		Archive::Extract()
    {
      enter();

      // nothing to do.

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

      // store the nil type.
      ::msgpack::pack(this->buffer, ::msgpack::type::nil());

      leave();
    }

    ///
    /// this method stores a boolean.
    ///
    Status		Archive::Store(const Boolean&		element)
    {
      enter();

      // pack the element.
      ::msgpack::pack(this->buffer, element);

      leave();
    }

    ///
    /// this method stores a character.
    ///
    Status		Archive::Store(const Character&		element)
    {
      String		string(1, element);

      return (this->Store(string));
    }

    ///
    /// this method stores a real.
    ///
    Status		Archive::Store(const Real&		element)
    {
      enter();

      // pack the element.
      ::msgpack::pack(this->buffer, element);

      leave();
    }

    ///
    /// this method stores a BIGNUM into the archive.
    ///
    Status		Archive::Store(const Large&		element)
    {
      Natural32		size = BN_num_bytes(&element);
      Region		region;

      enter();

      // prepare the region.
      if (region.Prepare(size) == StatusError)
	escape("unable to prepare the region");

      // finally, directory copy the data into the region.
      ::BN_bn2bin(&element, region.contents);

      // set the size.
      region.size = size;

      // store the region.
      if (this->Store(region) == StatusError)
	escape("unable to store the region");

      leave();
    }

    ///
    /// this method stores a string.
    ///
    Status		Archive::Store(const String&		element)
    {
      enter();

      // pack the element.
      ::msgpack::pack(this->buffer, element);

      leave();
    }

    ///
    /// this method stores a region.
    ///
    Status		Archive::Store(const Region&		element)
    {
      enter();

      // store the region as a raw.
      msgpack::pack(this->buffer,
		    msgpack::type::raw_ref((const char*)element.contents,
					   element.size));

      leave();
    }

    ///
    /// this method stores an archive.
    ///
    Status		Archive::Store(const Archive&		element)
    {
      enter();

      // make sure the given archive is in serialization mode.
      if (element.mode != Archive::ModeSerialization)
	escape("unable to store a non-serializing archive");

      // store the archive's buffer as a region.
      if (this->Store(Region((Byte*)element.contents,
			     element.size)) == StatusError)
	escape("unable to store the region");

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
      ::msgpack::unpacked	message;
      ::msgpack::object		object;

      enter();

      // extract the unpacked message.
      ::msgpack::unpack(&message,
                        (const char*)this->contents, this->size,
                        &this->offset);

      // retrieve the object.
      object = message.get();

      // check the object's type.
      if (object.type != ::msgpack::type::NIL)
	escape("the next element does not seem to be a nil type");

      leave();
    }

    ///
    /// this method loads a boolean.
    ///
    Status		Archive::Load(Boolean&			element)
    {
      ::msgpack::unpacked	message;
      ::msgpack::object		object;

      enter();

      // extract the unpacked message.
      ::msgpack::unpack(&message,
                        (const char*)this->contents, this->size,
                        &this->offset);

      // retrieve the object.
      object = message.get();

      // check the object's type.
      if (object.type != ::msgpack::type::BOOLEAN)
	escape("the next element does not seem to be a boolean type");

      // extract the element.
      object >> element;

      leave();
    }

    ///
    /// this method loads a character.
    ///
    Status		Archive::Load(Character&		element)
    {
      String		string;

      enter();

      // load the string.
      if (this->Load(string) == StatusError)
	escape("unable to load the string");

      // test the string length.
      if (string.length() != 1)
	escape("the string should have contained a single character");

      // return the character.
      element = string[0];

      leave();
    }

    ///
    /// this method loads a real.
    ///
    Status		Archive::Load(Real&			element)
    {
      ::msgpack::unpacked	message;
      ::msgpack::object		object;

      enter();

      // extract the unpacked message.
      ::msgpack::unpack(&message,
                        (const char*)this->contents, this->size,
                        &this->offset);

      // retrieve the object.
      object = message.get();

      // check the object's type.
      if (object.type != ::msgpack::type::DOUBLE)
	escape("the next element does not seem to be a double type");

      // extract the element.
      object >> element;

      leave();
    }

    ///
    /// this method loads a Large.
    ///
    Status		Archive::Load(Large&			element)
    {
      Region		region;

      enter();

      // initialize the big number.
      ::BN_init(&element);

      // load a region.
      if (this->Load(region) == StatusError)
	escape("unable to load the region");

      // load directly the bignum from the buffer.
      ::BN_bin2bn(region.contents, region.size, &element);

      leave();
    }

    ///
    /// this method loads a string.
    ///
    Status		Archive::Load(String&			element)
    {
      ::msgpack::unpacked	message;
      ::msgpack::object		object;

      enter();

      // extract the unpacked message.
      ::msgpack::unpack(&message,
                        (const char*)this->contents, this->size,
                        &this->offset);

      // retrieve the object.
      object = message.get();

      // check the object's type.
      if (object.type != ::msgpack::type::RAW)
	escape("the next element does not seem to be a raw type");

      // extract the element.
      object >> element;

      leave();
    }

    ///
    /// this method loads a region.
    ///
    Status		Archive::Load(Region&			element)
    {
      ::msgpack::unpacked	message;
      ::msgpack::object		object;
      ::msgpack::type::raw_ref	ref;

      enter();

      // extract the unpacked message.
      ::msgpack::unpack(&message,
                        (const char*)this->contents, this->size,
                        &this->offset);

      // retrieve the object.
      object = message.get();

      // check the object's type.
      if (object.type != ::msgpack::type::RAW)
	escape("the next element does not seem to be a raw type");

      // extract the element.
      object >> ref;

      // assign the data.
      if (element.Duplicate((Byte*)ref.ptr, ref.size) == StatusError)
	escape("unable to prepare the buffer");

      leave();
    }

    ///
    /// this method loads an archive.
    ///
    Status		Archive::Load(Archive&			element)
    {
      Region		region;

      enter();

      // make sure the given archive is not in serialization mode.
      if (element.mode == Archive::ModeSerialization)
	escape("unable to load to a serializing archive");

      // store the archive's buffer as a region.
      if (this->Load(region) == StatusError)
	escape("unable to load the region");

      // prepare the given archive.
      if (element.Acquire(region) == StatusError)
	escape("unable to prepare the archive");

      // detach the region.
      if (region.Detach() == StatusError)
	escape("unable to detach the region");

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

      // prepare the archive depending on the current mode.
      if (archive.Wrap(Region(this->contents, this->size)) == StatusError)
	escape("unable to prepare the archive");

      std::cout << alignment
		<< "[Archive] "
		<< "mode(" << std::dec << (Natural8)archive.mode << ") "
		<< "contents(" << (Void*)archive.contents << ") "
		<< "size(" << std::dec << archive.size << ") "
		<< "offset(" << std::dec << archive.offset << ")"
		<< std::endl;

      // go through the archive elements.
      while (archive.offset != archive.size)
	{
	  Archive::Type		type;

	  if (archive.Fetch(type) == StatusError)
	    escape("unable to fetch the next element's type");

	  if (archive.offset == this->offset)
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
		escape("unknown element type '%u'",
		       type);

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
      size = sizeof (Archive);

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
	yield(*this, "unable to recycle the object");

      return (*this);
    }

    ///
    /// this method check if two archives match.
    ///
    Boolean		Archive::operator==(const Archive&	element) const
    {
      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // check the attributes.
      if ((this->mode != element.mode) ||
	  (this->offset != element.offset))
	false();

      // finally, compare the buffers.
      if ((this->size != element.size) ||
	  (::memcmp(this->contents, element.contents, this->size) != 0))
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
