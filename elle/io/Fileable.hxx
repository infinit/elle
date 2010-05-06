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
// updated       julien quintard   [wed may  5 23:16:27 2010]
//

#ifndef ELLE_IO_FILEABLE_HXX
#define ELLE_IO_FILEABLE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <elle/standalone/Region.hh>

#include <elle/archive/Archivable.hh>
#include <elle/archive/Archive.hh>

#include <elle/util/Base64.hh>
#include <elle/util/Hexadecimal.hh>

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
#include <elle/idiom/Open.hh>

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
	struct ::stat	status;
	int		fd;

	enter();

	// retrieve information on the associat.
	if (::stat(path.c_str(), &status) == -1)
	  escape(::strerror(errno));

	// prepare the region.
	if (region.Prepare(status.st_size) == StatusError)
	  escape("unable to prepare the region");

	// set the correct size.
	region.size = status.st_size;

	// open the file.
	if ((fd = ::open(path.c_str(), O_RDONLY)) == -1)
	  escape(::strerror(errno));

	// read the file's content.
	if (::read(fd, region.contents, region.size) == -1)
	  escape(::strerror(errno));

	// close the file.
	::close(fd);

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
	int		fd;

	enter();

	// create an archive.
	if (archive.Create() == StatusError)
	  escape("unable to create the archive");

	// serialize the object.
	if (this->Serialize(archive) == StatusError)
	  escape("unable to serialize the object");

	// open the file.
	if ((fd = ::open(path.c_str(),
			 O_CREAT | O_TRUNC | O_WRONLY,
			 0600)) == -1)
	  escape(::strerror(errno));

	// write the text to the file.
	if (::write(fd,
		    archive.contents,
		    archive.size) != archive.size)
	  escape(::strerror(errno));

	// close the file.
	::close(fd);

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
	struct ::stat	status;
	int		fd;

	enter();

	// retrieve information on the associat.
	if (::stat(path.c_str(), &status) == -1)
	  escape(::strerror(errno));

	// prepare the region.
	if (region.Prepare(status.st_size) == StatusError)
	  escape("unable to prepare the region");

	// set the correct size.
	region.size = status.st_size;

	// open the file.
	if ((fd = ::open(path.c_str(), O_RDONLY)) == -1)
	  escape(::strerror(errno));

	// read the file's content.
	if (::read(fd, region.contents, region.size) == -1)
	  escape(::strerror(errno));

	// close the file.
	::close(fd);

	// decode and extract the object.
	if (Hexadecimal::Decode(String((char*)region.contents, region.size),
				*this) == StatusError)
	  escape("unable to decode the object");

	leave();
      }

      virtual Status	Store(const String&			path) const
      {
	String		string;
	int		fd;

	enter();

	// encode in hexadecimal.
	if (Hexadecimal::Encode(*this, string) == StatusError)
	  escape("unable to encode the object in hexadecimal");

	// open the file.
	if ((fd = ::open(path.c_str(),
			 O_CREAT | O_TRUNC | O_WRONLY,
			 0600)) == -1)
	  escape(::strerror(errno));

	// write the text to the file.
	if (::write(fd,
		    string.c_str(),
		    string.length()) != string.length())
	  escape(::strerror(errno));

	// close the file.
	::close(fd);

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
	struct ::stat	status;
	int		fd;

	enter();

	// retrieve information on the associat.
	if (::stat(path.c_str(), &status) == -1)
	  escape(::strerror(errno));

	// prepare the region.
	if (region.Prepare(status.st_size) == StatusError)
	  escape("unable to prepare the region");

	// set the correct size.
	region.size = status.st_size;

	// open the file.
	if ((fd = ::open(path.c_str(), O_RDONLY)) == -1)
	  escape(::strerror(errno));

	// read the file's content.
	if (::read(fd, region.contents, region.size) == -1)
	  escape(::strerror(errno));

	// close the file.
	::close(fd);

	// decode and extract the object.
	if (Base64::Decode(String((char*)region.contents, region.size),
			   *this) == StatusError)
	  escape("unable to decode the object");

	leave();
      }

      virtual Status	Store(const String&			path) const
      {
	String		string;
	int		fd;

	enter();

	// encode in base64.
	if (Base64::Encode(*this, string) == StatusError)
	  escape("unable to encode the object in base64");

	// open the file.
	if ((fd = ::open(path.c_str(),
			 O_CREAT | O_TRUNC | O_WRONLY,
			 0600)) == -1)
	  escape(::strerror(errno));

	// write the text to the file.
	if (::write(fd,
		    string.c_str(),
		    string.length()) != string.length())
	  escape(::strerror(errno));

	// close the file.
	::close(fd);

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
