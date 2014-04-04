#include <elle/archive/zip.hh>

#include <archive.h>
#include <archive_entry.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <elle/Exception.hh>
#include <elle/printf.hh>

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
    _zip_file(::archive* archive,
              boost::filesystem::path const& file,
              boost::filesystem::path const& relative_path)
    {
      std::unique_ptr<archive_entry, archive_entry_deleter> entry
        (archive_entry_new());
      archive_entry_set_pathname(entry.get(), relative_path.string().c_str());
      struct stat st;
      ::stat(file.native().c_str(), &st);
      archive_entry_copy_stat(entry.get(), &st);
      check_call(archive, archive_write_header(archive, entry.get()));
      boost::filesystem::ifstream input(file);
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

    void
    zip(std::vector<boost::filesystem::path> const& files,
        boost::filesystem::path const& path)
    {
      std::unique_ptr<::archive, archive_deleter> archive(archive_write_new());
      check_call(archive.get(), archive_write_set_format_zip(archive.get()));
      check_call(archive.get(),
                 archive_write_open_filename(archive.get(), path.native().c_str()));
      for (auto const& path: files)
      {
        if (boost::filesystem::is_directory(path))
          for (auto it = boost::filesystem::recursive_directory_iterator(path);
               it != boost::filesystem::recursive_directory_iterator();
               ++it)
          {
            boost::filesystem::path absolute(*it);
            if (boost::filesystem::is_directory(absolute))
              continue;
            boost::filesystem::path relative;
            {
              auto start = std::begin(absolute);
              for (auto count = std::begin(path); ++count != std::end(path);)
                ++start;
              for (; start != std::end(absolute); ++start)
                relative /= *start;
            }
            _zip_file(archive.get(), absolute, relative);
          }
        else
          _zip_file(archive.get(), path, path.filename());
      }
    }
  }
}
