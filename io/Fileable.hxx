//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri apr 30 17:43:29 2010]
//

#ifndef ELLE_IO_FILEABLE_HXX
#define ELLE_IO_FILEABLE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Byte.hh>
#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <elle/standalone/Region.hh>

#include <elle/package/Archivable.hh>
#include <elle/package/Archive.hh>

#include <elle/utility/Base64.hh>
#include <elle/utility/Hexadecimal.hh>

#include <elle/io/File.hh>
#include <elle/io/Path.hh>

namespace elle
{
  using namespace utility;

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
      virtual Status    Load(const Path&                        path)
      {
        Archive         archive;
        Region          region;

        enter();

        // read the file's content.
        if (File::Read(path, region) == StatusError)
          escape("unable to read the file's content");

        // detach the data from the region.
        if (region.Detach() == StatusError)
          escape("unable to detach the data");

        // prepare the archive.
        if (archive.Acquire(region) == StatusError)
          escape("unable to prepare the archive");

        // extract the object.
        if (this->Extract(archive) == StatusError)
          escape("unable to extract the archive");

        leave();
      }

      virtual Status    Store(const Path&                       path) const
      {
        Archive         archive;

        enter();

        // create an archive.
        if (archive.Create() == StatusError)
          escape("unable to create the archive");

        // serialize the object.
        if (this->Serialize(archive) == StatusError)
          escape("unable to serialize the object");

        // write the file's content.
        if (File::Write(path, Region(archive.contents,
                                     archive.size)) == StatusError)
          escape("unable to write the file's content");

        leave();
      }

      virtual Status    Erase(const Path&                       path) const
      {
        enter();

        // erase the file.
        if (File::Erase(path) == StatusError)
          escape("unable to erase the file");

        leave();
      }

      virtual Status    Exist(const Path&                       path) const
      {
        return (File::Exist(path));
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
      virtual Status    Load(const Path&                        path)
      {
        Region          region;
        String          string;

        enter();

        // read the file's content.
        if (File::Read(path, region) == StatusError)
          escape("unable to read the file's content");

        // decode and extract the object.
        if (Hexadecimal::Decode(
              String(reinterpret_cast<const char*>(region.contents),
                     region.size),
              *this) == StatusError)
          escape("unable to decode the object");

        leave();
      }

      virtual Status    Store(const Path&                       path) const
      {
        String          string;
        Region          region;

        enter();

        // encode in hexadecimal.
        if (Hexadecimal::Encode(*this, string) == StatusError)
          escape("unable to encode the object in hexadecimal");

        // wrap the string.
        if (region.Wrap(reinterpret_cast<const Byte*>(string.c_str()),
                        string.length()) == StatusError)
          escape("unable to wrap the string in a region");

        // write the file's content.
        if (File::Write(path, region) == StatusError)
          escape("unable to write the file's content");

        leave();
      }

      virtual Status    Erase(const Path&                       path) const
      {
        enter();

        // erase the file.
        if (File::Erase(path) == StatusError)
          escape("unable to erase the file");

        leave();
      }

      virtual Status    Exist(const Path&                       path) const
      {
        return (File::Exist(path));
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
      virtual Status    Load(const Path&                        path)
      {
        Region          region;
        String          string;

        enter();

        // read the file's content.
        if (File::Read(path, region) == StatusError)
          escape("unable to read the file's content");

        // decode and extract the object.
        if (Base64::Decode(String(reinterpret_cast<char*>(region.contents),
                                  region.size),
                           *this) == StatusError)
          escape("unable to decode the object");

        leave();
      }

      virtual Status    Store(const Path&                       path) const
      {
        String          string;
        Region          region;

        enter();

        // encode in base64.
        if (Base64::Encode(*this, string) == StatusError)
          escape("unable to encode the object in base64");

        // wrap the string.
        if (region.Wrap(reinterpret_cast<const Byte*>(string.c_str()),
                        string.length()) == StatusError)
          escape("unable to wrap the string in a region");

        // write the file's content.
        if (File::Write(path, region) == StatusError)
          escape("unable to write the file's content");

        leave();
      }

      virtual Status    Erase(const Path&                       path) const
      {
        enter();

        // erase the file.
        if (File::Erase(path) == StatusError)
          escape("unable to erase the file");

        leave();
      }

      virtual Status    Exist(const Path&                       path) const
      {
        return (File::Exist(path));
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
      virtual Status    Load(const Path&)
      {
        enter();

        escape("this method should never have been called");
      }

      virtual Status    Store(const Path&) const
      {
        enter();

        escape("this method should never have been called");
      }

      virtual Status    Erase(const Path&) const
      {
        enter();

        escape("this method should never have been called");
      }

      virtual Status    Exist(const Path&) const
      {
        enter();

        escape("this method should never have been called");
      }
    };

  }
}

#endif
