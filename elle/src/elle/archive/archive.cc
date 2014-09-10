#include <elle/archive/archive.hh>

#include <unordered_set>

#include <archive.h>
#include <archive_entry.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <elle/Exception.hh>
#include <elle/container/vector.hh>
#include <elle/log.hh>
#include <elle/printf.hh>

ELLE_LOG_COMPONENT("elle.archive.archive");

namespace elle
{
  namespace archive
  {
    static
    void
    check_call(::archive* archive, int code, int expected = ARCHIVE_OK)
    {
      if (code != expected)
        throw elle::Exception(archive_error_string(archive));
    }

    struct archive_deleter
    {
      void
      operator()(::archive* archive)
      {
        check_call(archive, archive_write_close(archive));
        check_call(archive, archive_write_free(archive));
      }
    };

    struct archive_read_deleter
    {
      void
      operator()(::archive* archive)
      {
        check_call(archive, archive_read_close(archive));
        check_call(archive, archive_read_free(archive));
      }
    };

    struct archive_entry_deleter
    {
      void
      operator()(archive_entry* entry)
      {
        archive_entry_free(entry);
      }
    };

    static
    void
    _archive_file(::archive* archive,
              boost::filesystem::path const& file,
              boost::filesystem::path const& relative_path)
    {
      ELLE_DEBUG_SCOPE("add %s as %s", file, relative_path);
      std::unique_ptr<archive_entry, archive_entry_deleter> entry
        (archive_entry_new());
      archive_entry_set_pathname(entry.get(), relative_path.string().c_str());
      archive_entry_copy_sourcepath(entry.get(), file.string().c_str());
      struct stat st;
#ifdef INFINIT_WINDOWS
      #define S_ISLNK(a) false
      ::stat
#else
      ::lstat
#endif
             (file.string().c_str(), &st);

      std::unique_ptr< ::archive, archive_read_deleter> read_disk
        (archive_read_disk_new());
      archive_read_disk_set_symlink_physical(read_disk.get());
      archive_read_disk_entry_from_file(read_disk.get(), entry.get(), -1, 0/*&st*/);
      check_call(archive, archive_write_header(archive, entry.get()));
      ELLE_DEBUG("Will write %s bytes for %s, islink:%s mode:%s",
        archive_entry_size(entry.get()), file, S_ISLNK(st.st_mode), st.st_mode);
      // An archive_entry_size of 0 means data is not required (hardlink)
      if (archive_entry_size(entry.get()) > 0 && !S_ISLNK(st.st_mode))
      {
        boost::filesystem::ifstream input(file, std::ios_base::in | std::ios_base::binary);
        if (!input.good())
          throw elle::Exception(elle::sprintf("unable to read file %s", file));
        char buffer[BUFSIZ];
        while (!input.eof())
        {
          input.read(buffer, BUFSIZ);
          size_t read = input.gcount();
          check_call(archive, archive_write_data(archive, buffer, read), read);
        }
      }
    }

    void
    archive(Format format,
            std::vector<boost::filesystem::path> const& files,
            boost::filesystem::path const& path,
            Renamer const& renamer,
            Excluder const& excluder)
    {
      ELLE_TRACE_SCOPE("archive %s", path);
      ELLE_DEBUG("files: %s", files);
      std::unordered_set<std::string> root_entries;
      std::unique_ptr< ::archive, archive_deleter> archive(archive_write_new());
      int (*format_setter)(::archive*) = nullptr;
      int (*compression_setter)(::archive*) = nullptr;
      switch (format)
      {
        case Format::tar:
          format_setter = archive_write_set_format_gnutar;
          break;
        case Format::tar_bzip2:
          format_setter = archive_write_set_format_gnutar;
          compression_setter = archive_write_add_filter_bzip2;
          break;
        case Format::tar_gzip:
          format_setter = archive_write_set_format_gnutar;
          compression_setter = archive_write_add_filter_gzip;
          break;
        case Format::zip:
          format_setter = archive_write_set_format_zip;
          break;
        case Format::zip_uncompressed:
          format_setter = archive_write_set_format_zip;
          compression_setter = archive_write_zip_set_compression_store;
          break;
        default:
          elle::unreachable();
      }
      check_call(archive.get(), format_setter(archive.get()));
      if (compression_setter)
        check_call(archive.get(), compression_setter(archive.get()));
      check_call(archive.get(),
                 archive_write_open_filename(archive.get(),
                                             path.string().c_str()));
      for (auto const& path: files)
      {
        auto root = path.filename();
        while (root_entries.find(root.string()) != root_entries.end())
          root = renamer(root);
        root_entries.insert(root.string());
        ELLE_DEBUG("Renamed %s to %s", path, root);
        if (!boost::filesystem::exists(path))
          throw elle::Exception(elle::sprintf("Path %s does not exist", path));
        if (boost::filesystem::is_directory(path))
          for (auto it = boost::filesystem::recursive_directory_iterator(path);
               it != boost::filesystem::recursive_directory_iterator();
               ++it)
          {
            boost::filesystem::path absolute(*it);
            if (boost::filesystem::is_directory(absolute))
              continue;
            boost::filesystem::path relative = root;
            {
              auto start = std::begin(absolute);
              for (auto count = std::begin(path);
                   count != std::end(path);
                   ++count)
                ++start;
              for (; start != std::end(absolute); ++start)
                relative /= *start;
            }
            if (excluder && excluder(absolute))
            {
              ELLE_DEBUG("Skipping %s", absolute);
              continue;
            }
            ELLE_DEBUG("archiving from directory %s as %s", absolute, relative);
            _archive_file(archive.get(), absolute, relative);
          }
        else
        {
          if (excluder && excluder(path))
          {
            ELLE_DEBUG("Skipping %s", path);
            continue;
          }
          ELLE_DEBUG("archiving %s as %s", path, root);
          _archive_file(archive.get(), path, root);
        }
      }
    }
  }
}
