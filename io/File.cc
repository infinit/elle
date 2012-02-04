//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu may 27 16:18:11 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/Platform.hh>

#include <elle/io/File.hh>
#include <elle/io/Directory.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/system/System.hh>

#include <vector>
#include <sstream>

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
# include <libgen.h>

# if defined(INFINIT_WINDOWS)
#  include <windows.h>
# endif

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace system;

  namespace io
  {

//
// ---------- static methods --------------------------------------------------
//

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
    ///
    /// this method reads the given file's content.
    ///
    Status              File::Read(const Path&                  path,
                                   Region&                      data)
    {
      struct ::stat     status;
      int               fd;
      Natural32         roffset = 0;

      enter();

      // does the file exist.
      if (File::Exist(path) == StatusFalse)
        escape("the file does not seem to exist");

      // retrieve information.
      if (::stat(path.string.c_str(), &status) == -1)
        escape(::strerror(errno));

      // prepare the data.
      if (data.Prepare(static_cast<Natural32>(status.st_size)) == StatusError)
        escape("unable to prepare the region");

      // open the file.
      if ((fd = ::open(path.string.c_str(), O_RDONLY)) == -1)
        escape("failed to open %s: %s",
               path.string.c_str(), ::strerror(errno));

      // read the file's content.
      while (roffset < data.capacity)
        {
          int rbytes = ::read(fd,
                              data.contents + roffset,
                              data.capacity - roffset);

          if (rbytes == 0)
            break;

          if (rbytes < 0)
            {
              if (errno == EAGAIN ||
                  errno == EINTR)
                continue;

              ::close(fd);

              escape("read error: %s", ::strerror(errno));
            }

          roffset += rbytes;
        }

      data.size = roffset;

      // close the file.
      ::close(fd);

      leave();
    }

    ///
    /// this method writes the given data into the given file.
    ///
    Status              File::Write(const Path&                 path,
                                    const Region&               data)
    {
      int               fd;
      Natural32         woffset = 0;

      enter();

      // dig the directory which will hold the target file.
      if (File::Dig(path) == StatusError)
        escape("unable to dig the chain of directories");

      // open the file.
      if ((fd = ::open(path.string.c_str(),
                       O_CREAT | O_TRUNC | O_WRONLY,
                       0600)) == -1)
        escape(::strerror(errno));

      // write the text to the file.
      while (woffset < data.size)
        {
          int           wbytes;

          wbytes = ::write(fd, data.contents + woffset, data.size - woffset);

          if (wbytes < 0)
            {
              if (errno == EAGAIN ||
                  errno == EINTR)
                continue;

              ::close(fd);
              escape(::strerror(errno));
            }

          if (wbytes == 0)
            break;

          woffset += wbytes;
        }

      // close the file.
      ::close(fd);

      leave();
    }
#elif defined(INFINIT_WINDOWS)
    ///
    /// this method reads the given file's content.
    ///
    Status              File::Read(const Path&                  path,
                                   Region&                      data)
    {
      struct ::stat     status;
      HANDLE            fd;
      DWORD             roffset = 0;

      enter();

      // does the file exist.
      if (File::Exist(path) == StatusFalse)
        escape("the file does not seem to exist");

      // retrieve information.
      if (::stat(path.string.c_str(), &status) == -1)
        escape(::strerror(errno));

      // prepare the data.
      if (data.Prepare(static_cast<Natural32>(status.st_size)) == StatusError)
        escape("unable to prepare the region");

      // open the file.
      fd = ::CreateFile(path.string.c_str(), GENERIC_READ,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                        NULL);

      if (fd == INVALID_HANDLE_VALUE)
        escape("failed to open %s", path.string.c_str());

      // read the file's content.
      while (roffset < data.capacity)
        {
          DWORD rbytes;

          BOOL succeed = ::ReadFile(fd,
                                    data.contents + roffset,
                                    data.capacity - roffset,
                                    &rbytes,
                                    NULL);

          if (!succeed)
            {
              ::CloseHandle(fd);
              escape("read error");
            }

          if (rbytes == 0)
            break;

          roffset += rbytes;
        }

      data.size = roffset;

      // close the file.
      ::CloseHandle(fd);

      leave();
    }

    ///
    /// this method writes the given data into the given file.
    ///
    Status              File::Write(const Path&                 path,
                                    const Region&               data)
    {
      HANDLE            fd;
      DWORD             woffset = 0;

      enter();

      // dig the directory which will hold the target file.
      if (File::Dig(path) == StatusError)
        escape("unable to dig the chain of directories");

      // open the file.
      fd = ::CreateFile(path.string.c_str(), GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                        NULL);

      if (fd == INVALID_HANDLE_VALUE)
        escape("failed to open %s", path.string.c_str());

      // write the text to the file.
      while (woffset < data.size)
        {
          DWORD         wbytes;
          BOOL          succeed;

          succeed = ::WriteFile(fd, data.contents + woffset,
                                data.size - woffset, &wbytes, NULL);

          if (!succeed)
            {
              ::CloseHandle(fd);

              escape("write error");
            }

          if (wbytes == 0)
            break;

          woffset += wbytes;
        }

      // close the file.
      ::CloseHandle(fd);

      leave();
    }
#else
# error "unsupported platform"
#endif

    ///
    /// this method erases the given file path.
    ///
    Status              File::Erase(const Path&                 path)
    {
      enter();

      // does the file exist.
      if (File::Exist(path) == StatusFalse)
        escape("the file does not seem to exist");

      // unlink the file.
      ::unlink(path.string.c_str());

      leave();
    }

    ///
    /// this method returns true if the pointed to file exists.
    ///
    Status              File::Exist(const Path&                 path)
    {
      struct ::stat             stat;

      enter();

      // does the path points to something.
      if (::stat(path.string.c_str(), &stat) != 0)
        false();

      // does the path points to a regular file.
      if (!S_ISREG(stat.st_mode))
        false();

      true();
    }

    ///
    /// this method takes a path to a file and creates, if necessary,
    /// the intermediate directory leading to the file.
    ///
    Status              File::Dig(const Path&                   path)
    {
      String            target(path.string);
      char *            tmp_str = ::strdup(path.string.c_str());
      String            directory(::dirname(tmp_str));
      std::stringstream stream(directory);
      String            item;
      Path              chemin;

      enter();

      // free the temporary string used for directory
      free(tmp_str);

      // go through the components of the path.
      while (std::getline(stream, item, System::Path::Separator))
        {
          // update the intermediate chemin.
          if (chemin.string.empty() && item.empty())
            chemin.string = System::Path::Separator;
          else
            {
              chemin.string.append(item);
              chemin.string.append(1, System::Path::Separator);
            }

          // retrieve information on the path. should this operation fail
          // would mean that the target directory does not exist.
          if (Directory::Exist(chemin) == StatusFalse)
            {
              // create the intermediate directory.
              if (Directory::Create(chemin) == StatusError)
                escape("unable to create the intermediate directory");
            }
        }

      leave();
    }

  }
}
