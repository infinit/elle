#include <elle/archive/archive.hh>

#include <unordered_set>

#include <archive.h>
#include <archive_entry.h>

#include <boost/filesystem.hpp>

#include <elle/Error.hh>
#include <elle/system/system.hh>
#include <elle/log.hh>
#include <elle/printf.hh>

#ifdef INFINIT_WINDOWS
# define stat _stat
#endif

ELLE_LOG_COMPONENT("elle.archive.archive");

namespace elle
{
  namespace archive
  {
    static
    void
    check_call(::archive* archive, int code, int expected = ARCHIVE_OK)
    {
      ELLE_ASSERT(archive != nullptr);
      if (code != expected)
      {
        auto error = archive_error_string(archive);
        if (error == nullptr)
          error = "unknown archiving error";
        throw elle::Error(
          elle::sprintf("%s (%s)", error, archive_errno(archive)));
      }
    }

    static
    int
    copy_data(::archive* ar, ::archive* aw)
    {
      int r;
      const void* buff;
      size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
      int64_t offset;
#else
      off_t offset;
#endif
      for (;;)
      {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
          return ARCHIVE_OK;
        if (r != ARCHIVE_OK)
          return r;
        r = archive_write_data_block(aw, buff, size, offset);
        if (r != ARCHIVE_OK)
          return r;
      }
    }

    struct archive_deleter
    {
      void
      operator()(::archive* archive)
      {
        ELLE_DEBUG_SCOPE("archive deleter: %s", (void*)(archive));
        check_call(archive, archive_write_close(archive));
        check_call(archive, archive_write_free(archive));
      }
    };
    using ArchivePtr = std::unique_ptr< ::archive, archive_deleter >;

    struct archive_read_deleter
    {
      void
      operator()(::archive* archive)
      {
        ELLE_DEBUG_SCOPE("archive read deleter: %s", (void*)(archive));
        check_call(archive, archive_read_close(archive));
        check_call(archive, archive_read_free(archive));
      }
    };
    using ArchiveReadPtr = std::unique_ptr< ::archive, archive_read_deleter >;

    struct archive_entry_deleter
    {
      void
      operator()(archive_entry* entry)
      {
        ELLE_DEBUG_SCOPE("archive entry deleter: %s", (void*)(entry));
        archive_entry_free(entry);
      }
    };
    using EntryPtr = std::unique_ptr< ::archive_entry, archive_entry_deleter >;

    static
    void
    _archive_file(::archive* archive,
                  boost::filesystem::path const& file,
                  boost::filesystem::path const& relative_path)
    {
      ELLE_TRACE_SCOPE("add %s as %s", file, relative_path);
      EntryPtr entry(archive_entry_new());
      // XXX: Convert path to native windows encoding.
      archive_entry_copy_pathname(entry.get(), relative_path.string().c_str());
      struct stat st;
#ifdef INFINIT_WINDOWS
      archive_entry_copy_sourcepath_w(entry.get(), file.native().c_str());
#define S_ISLNK(a) false
      ::_wstat(file.native().c_str(), &st);
#else
      archive_entry_copy_sourcepath(entry.get(), file.string().c_str());
      ::lstat(file.string().c_str(), &st);
#endif
      ArchiveReadPtr read_disk(archive_read_disk_new());
      archive_read_disk_set_symlink_physical(read_disk.get());
      archive_read_disk_entry_from_file(read_disk.get(), entry.get(), -1, 0/*&st*/);
      ELLE_DEBUG("will write %s bytes for %s, islink:%s mode:%s",
        archive_entry_size(entry.get()), file, S_ISLNK(st.st_mode), st.st_mode);
      check_call(archive, archive_write_header(archive, entry.get()));

      // An archive_entry_size of 0 means data is not required (hardlink).
      if (archive_entry_size(entry.get()) > 0 && !S_ISLNK(st.st_mode))
      {
        uint64_t offset = 0;
        size_t chunck_size = 5 * 1024 * 1024;
        while (true)
        {
          ELLE_DEBUG("size: %s, offset: %s", chunck_size, offset);
          auto buffer = elle::system::read_file_chunk(file, offset, chunck_size);
          ELLE_DEBUG("buffer size %s", buffer.size());
          if (buffer.empty())
            break;
          check_call(archive, archive_write_data(archive, buffer.contents(), buffer.size()), buffer.size());
          if (buffer.size() < chunck_size)
            break;
          offset += buffer.size();
        }
      }
      ELLE_TRACE("file %s archived into %s", file, (void*)(archive));
    }

    void
    archive(Format format,
            std::vector<boost::filesystem::path> const& files,
            boost::filesystem::path const& path,
            Renamer const& renamer,
            Excluder const& excluder,
            bool ignore_failure)
    {
      ELLE_TRACE_SCOPE("archive %s", path);
      ELLE_DEBUG("files: %s", files);
      std::unordered_set<std::string> root_entries;
      ArchivePtr archive(archive_write_new());
      ELLE_TRACE("archive: %s", (void*)(archive.get()));
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
#ifdef INFINIT_WINDOWS
        archive_write_open_filename_w(archive.get(), path.native().c_str()));
#else
        archive_write_open_filename(archive.get(), path.string().c_str()));
#endif
      auto do_archiving = [ignore_failure] (
        ::archive* archive,
        boost::filesystem::path const& absolute,
        boost::filesystem::path const& relative)
        {
          try
          {
            _archive_file(archive, absolute, relative);
          }
          catch (elle::Error const& e)
          {
            if (ignore_failure)
              ELLE_ERR("ignore %s: %s", absolute, e);
            else
              throw;
          }
        };

      for (auto const& path: files)
      {
        auto root = path.filename();
        while (root_entries.find(root.string()) != root_entries.end())
          root = renamer(root);
        root_entries.insert(root.string());
        ELLE_DEBUG("renamed %s to %s", path, root);
        if (!boost::filesystem::exists(path))
        {
          if (ignore_failure)
            continue;
          throw elle::Error(elle::sprintf("path %s does not exist", path));
        }
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
              ELLE_DEBUG("skipping %s", absolute);
              continue;
            }
            ELLE_DEBUG("archiving from directory %s as %s", absolute, relative);
            do_archiving(archive.get(), absolute, relative);
          }
        else
        {
          if (excluder && excluder(path))
          {
            ELLE_DEBUG("skipping %s", path);
            continue;
          }
          ELLE_DEBUG("archiving %s as %s", path, root);
          do_archiving(archive.get(), path, root);
        }
      }
    }

    void extract(boost::filesystem::path const& archive,
                 boost::optional<boost::filesystem::path> const& output)
    {
      ELLE_TRACE("[Archive] extracting %s", archive.string());
      ArchiveReadPtr a(archive_read_new());
      ArchivePtr out(archive_write_disk_new());
      int r;

      archive_read_support_filter_all(a.get());
      archive_read_support_format_all(a.get());
      check_call(a.get(), archive_read_open_filename(a.get(),
                 archive.string().c_str(), 10240));
      for (;;)
      {
        ::archive_entry* entry;
        r = archive_read_next_header(a.get(), &entry);
        if (r == ARCHIVE_EOF)
          break;
        check_call(a.get(), r);
        const char* cur_file = archive_entry_pathname(entry);
        auto dest = output ? output.get() : archive.parent_path();
        const std::string fullpath = (dest / cur_file).string();
        ELLE_TRACE("[Archive] extracting %s", fullpath);
        archive_entry_set_pathname(entry, fullpath.c_str());
        check_call(a.get(), archive_write_header(out.get(), entry));
        check_call(a.get(), copy_data(a.get(), out.get()));
        check_call(a.get(), archive_write_finish_entry(out.get()));
      }
    }
  }
}
