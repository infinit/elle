//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Fileable.hxx
//
// created       julien quintard   [fri apr 30 17:43:29 2010]
// updated       julien quintard   [fri may 28 13:28:38 2010]
//

#ifndef ELLE_IO_FILEABLE_HXX
#define ELLE_IO_FILEABLE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>
#include <elle/core/Byte.hh>

#include <elle/standalone/Region.hh>

#include <elle/archive/Archivable.hh>
#include <elle/archive/Archive.hh>

#include <elle/util/Base64.hh>
#include <elle/util/Hexadecimal.hh>

#include <elle/io/File.hh>

namespace elle
{
  using namespace util;

  namespace io
  {

//
// ---------- raw -------------------------------------------------------------
//

    ///
    /// this specialization is for the raw format.
    ///
    template <>
    class Fileable<FormatRaw>:
      public virtual Archivable
    {
    public:
      //
      // methods
      //
      virtual Status	Load(const String&			path)
      {
	Archive		archive;
	Region		region;

	enter();

	// read the file's content.
	if (File::Read(path, region) == StatusError)
	  escape("unable to read the file's content");

	// detach the data from the region.
	if (region.Detach() == StatusError)
	  escape("unable to detach the data");

	// prepare the archive.
	if (archive.Prepare(region) == StatusError)
	  escape("unable to prepare the archive");

	// extract the object.
	if (this->Extract(archive) == StatusError)
	  escape("unable to extract the archive");

	leave();
      }

      virtual Status	Store(const String&			path) const
      {
	Archive		archive;

	enter();

	// create an archive.
	if (archive.Create() == StatusError)
	  escape("unable to create the archive");

	// serialize the object.
	if (this->Serialize(archive) == StatusError)
	  escape("unable to serialize the object");

	// write the file's content.
	if (File::Write(path, archive) == StatusError)
	  escape("unable to write the file's content");

	leave();
      }
    };

//
// ---------- hexadecimal -----------------------------------------------------
//

    ///
    /// this specialization is for the hexadecimal format.
    ///
    template <>
    class Fileable<FormatHexadecimal>:
      public virtual Archivable
    {
    public:
      //
      // methods
      //
      virtual Status	Load(const String&			path)
      {
	Region		region;
	String		string;

	enter();

	// read the file's content.
	if (File::Read(path, region) == StatusError)
	  escape("unable to read the file's content");

	// decode and extract the object.
	if (Hexadecimal::Decode(String((char*)region.contents, region.size),
				*this) == StatusError)
	  escape("unable to decode the object");

	leave();
      }

      virtual Status	Store(const String&			path) const
      {
	String		string;
	Region		region;

	enter();

	// encode in hexadecimal.
	if (Hexadecimal::Encode(*this, string) == StatusError)
	  escape("unable to encode the object in hexadecimal");

	// wrap the string.
	if (region.Wrap((Byte*)string.c_str(),
			string.length()) == StatusError)
	  escape("unable to wrap the string in a region");

	// write the file's content.
	if (File::Write(path, region) == StatusError)
	  escape("unable to write the file's content");

	leave();
      }
    };

//
// ---------- base64 ----------------------------------------------------------
//

    ///
    /// this specialization is for the base64 format.
    ///
    template <>
    class Fileable<FormatBase64>:
      public virtual Archivable
    {
    public:
      //
      // methods
      //
      virtual Status	Load(const String&			path)
      {
	Region		region;
	String		string;

	enter();

	// read the file's content.
	if (File::Read(path, region) == StatusError)
	  escape("unable to read the file's content");

	// decode and extract the object.
	if (Base64::Decode(String((char*)region.contents, region.size),
			   *this) == StatusError)
	  escape("unable to decode the object");

	leave();
      }

      virtual Status	Store(const String&			path) const
      {
	String		string;
	Region		region;

	enter();

	// encode in base64.
	if (Base64::Encode(*this, string) == StatusError)
	  escape("unable to encode the object in base64");

	// wrap the string.
	if (region.Wrap((Byte*)string.c_str(),
			string.length()) == StatusError)
	  escape("unable to wrap the string in a region");

	// write the file's content.
	if (File::Write(path, region) == StatusError)
	  escape("unable to write the file's content");

	leave();
      }
    };

//
// ---------- custom ----------------------------------------------------------
//

    ///
    /// this specialization is for the custom format.
    ///
    template <>
    class Fileable<FormatCustom>:
      public virtual Archivable
    {
    public:
      //
      // methods
      //
      virtual Status	Load(const String&)
      {
	enter();

	escape("this method should never have been called");
      }

      virtual Status	Store(const String&) const
      {
	enter();

	escape("this method should never have been called");
      }
    };

  }
}

#endif
