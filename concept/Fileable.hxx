#ifndef  ELLE_CONCEPT_FILEABLE_HXX
# define ELLE_CONCEPT_FILEABLE_HXX

# include <fstream>

# include <elle/serialize/BinaryArchive.hxx>
# include <elle/log.hh>

# include <elle/io/File.hh>
# include "Fileable.hh"

# include <elle/idiom/Open.hh>

namespace elle { namespace concept {

    template<__ECS_ARCHIVE_TPL(Archive)>
      Status Fileable<Archive>::Load(elle::io::Path const& path)
      {
        ELLE_LOG_TRACE_COMPONENT("elle.concept");
        ELLE_LOG_TRACE("Loading %p to from file %s", this, path.str())
        {
          std::ifstream in(path.str(), std::ios_base::in | std::ios_base::binary);
          if (!in.good())
            {
              escape("Cannot open in file '%s'", path.str().c_str());
              //throw std::runtime_error("Cannot read '"+  path.str().c_str() +"'");
            }

          try
            {
              Archive<elle::serialize::ArchiveMode::Input> archive(in);
              this->deserialize(archive);
            }
          catch (std::exception const& err)
            {
              escape("%s", (
                  "Cannot load from '" + path.str() + "': " +
                  std::string(err.what())
              ).c_str());
              return elle::Status::Error;
              //throw std::runtime_error(
              //    "Cannot load from '" + path.str() + "': " +
              //    std::string(err.what())
              //);
            }
        }
      }

    template<__ECS_ARCHIVE_TPL(Archive)>
      Status Fileable<Archive>::Store(elle::io::Path const& path) const
      {
        ELLE_LOG_TRACE_COMPONENT("elle.concept");
        ELLE_LOG_TRACE("Saving %p to file %s", this, path.str())
        {
          if (File::Dig(path) == elle::Status::Error)
            escape("unable to dig the chain of directories");

          std::ofstream out(path.str(), std::ios_base::out | std::ios_base::binary);
          if (!out.good())
            {
              escape("Cannot open out file '%s'", path.str().c_str());

              //throw std::runtime_error("Cannot read '"+  path.str().c_str() +"'");
            }

          try
            {
              Archive<elle::serialize::ArchiveMode::Output> archive(out);
              this->serialize(archive);
            }
          catch (std::exception const& err)
            {
              escape("%s", (
                  "Cannot store to '" + path.str() + "': " +
                  std::string(err.what())
              ).c_str());
              return elle::Status::Error;
              //throw std::runtime_error(
              //    "Cannot load from '" + path.str() + "': " +
              //    std::string(err.what())
              //);
            }
        }
      }

////
//// ---------- raw -------------------------------------------------------------
////
//
//    ///
//    /// this specialization is for the raw format.
//    ///
//    template <>
//    class Fileable<FormatRaw>:
//      public virtual Archivable
//    {
//    public:
//      //
//      // methods
//      //
//      virtual Status    Load(const Path&                        path)
//      {
//        Archive         archive;
//        Region          region;
//
//        // read the file's content.
//        if (File::Read(path, region) == Status::Error)
//          escape("unable to read the file's content");
//
//        // detach the data from the region.
//        if (region.Detach() == Status::Error)
//          escape("unable to detach the data");
//
//        // prepare the archive.
//        if (archive.Acquire(region) == Status::Error)
//          escape("unable to prepare the archive");
//
//        // extract the object.
//        if (this->Extract(archive) == Status::Error)
//          escape("unable to extract the archive");
//
//        return Status::Ok;
//      }
//
//      virtual Status    Store(const Path&                       path) const
//      {
//        Archive         archive;
//
//        // create an archive.
//        if (archive.Create() == Status::Error)
//          escape("unable to create the archive");
//
//        // serialize the object.
//        if (this->Serialize(archive) == Status::Error)
//          escape("unable to serialize the object");
//
//        // write the file's content.
//        if (File::Write(path, Region(archive.contents,
//                                     archive.size)) == Status::Error)
//          escape("unable to write the file's content");
//
//        return Status::Ok;
//      }
//
//      virtual Status    Erase(const Path&                       path) const
//      {
//        // erase the file.
//        if (File::Erase(path) == Status::Error)
//          escape("unable to erase the file");
//
//        return Status::Ok;
//      }
//
//      virtual Status    Exist(const Path&                       path) const
//      {
//        return (File::Exist(path));
//      }
//    };
//
////
//// ---------- hexadecimal -----------------------------------------------------
////
//
//    ///
//    /// this specialization is for the hexadecimal format.
//    ///
//    template <>
//    class Fileable<FormatHexadecimal>:
//      public virtual Archivable
//    {
//    public:
//      //
//      // methods
//      //
//      virtual Status    Load(const Path&                        path)
//      {
//        Region          region;
//        String          string;
//
//        // read the file's content.
//        if (File::Read(path, region) == Status::Error)
//          escape("unable to read the file's content");
//
//        // decode and extract the object.
//        if (Hexadecimal::Decode(
//              String(reinterpret_cast<const char*>(region.contents),
//                     region.size),
//              *this) == Status::Error)
//          escape("unable to decode the object");
//
//        return Status::Ok;
//      }
//
//      virtual Status    Store(const Path&                       path) const
//      {
//        String          string;
//        Region          region;
//
//        // encode in hexadecimal.
//        if (Hexadecimal::Encode(*this, string) == Status::Error)
//          escape("unable to encode the object in hexadecimal");
//
//        // wrap the string.
//        if (region.Wrap(reinterpret_cast<const Byte*>(string.c_str()),
//                        string.length()) == Status::Error)
//          escape("unable to wrap the string in a region");
//
//        // write the file's content.
//        if (File::Write(path, region) == Status::Error)
//          escape("unable to write the file's content");
//
//        return Status::Ok;
//      }
//
//      virtual Status    Erase(const Path&                       path) const
//      {
//        // erase the file.
//        if (File::Erase(path) == Status::Error)
//          escape("unable to erase the file");
//
//        return Status::Ok;
//      }
//
//      virtual Status    Exist(const Path&                       path) const
//      {
//        return (File::Exist(path));
//      }
//    };
//
////
//// ---------- base64 ----------------------------------------------------------
////
//
//    ///
//    /// this specialization is for the base64 format.
//    ///
//    template <>
//    class Fileable<FormatBase64>:
//      public virtual Archivable
//    {
//    public:
//      //
//      // methods
//      //
//      virtual Status    Load(const Path&                        path)
//      {
//        Region          region;
//        String          string;
//
//        // read the file's content.
//        if (File::Read(path, region) == Status::Error)
//          escape("unable to read the file's content");
//
//        // decode and extract the object.
//        if (Base64::Decode(String(reinterpret_cast<char*>(region.contents),
//                                  region.size),
//                           *this) == Status::Error)
//          escape("unable to decode the object");
//
//        return Status::Ok;
//      }
//
//      virtual Status    Store(const Path&                       path) const
//      {
//        String          string;
//        Region          region;
//
//        // encode in base64.
//        if (Base64::Encode(*this, string) == Status::Error)
//          escape("unable to encode the object in base64");
//
//        // wrap the string.
//        if (region.Wrap(reinterpret_cast<const Byte*>(string.c_str()),
//                        string.length()) == Status::Error)
//          escape("unable to wrap the string in a region");
//
//        // write the file's content.
//        if (File::Write(path, region) == Status::Error)
//          escape("unable to write the file's content");
//
//        return Status::Ok;
//      }
//
//      virtual Status    Erase(const Path&                       path) const
//      {
//        // erase the file.
//        if (File::Erase(path) == Status::Error)
//          escape("unable to erase the file");
//
//        return Status::Ok;
//      }
//
//      virtual Status    Exist(const Path&                       path) const
//      {
//        return (File::Exist(path));
//      }
//    };
//
////
//// ---------- custom ----------------------------------------------------------
////
//
//    ///
//    /// this specialization is for the custom format.
//    ///
//    template <>
//    class Fileable<FormatCustom>:
//      public virtual Archivable
//    {
//    public:
//      //
//      // methods
//      //
//      virtual Status    Load(const Path&)
//      {
//        escape("this method should never have been called");
//      }
//
//      virtual Status    Store(const Path&) const
//      {
//        escape("this method should never have been called");
//      }
//
//      virtual Status    Erase(const Path&) const
//      {
//        escape("this method should never have been called");
//      }
//
//      virtual Status    Exist(const Path&) const
//      {
//        escape("this method should never have been called");
//      }
//    };
//

  } // !concept
} // !elle;

#endif

