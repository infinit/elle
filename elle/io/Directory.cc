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

#include <elle/io/Directory.hh>
#include <elle/io/File.hh>
#include <elle/io/Link.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/system/Platform.hh>
#include <elle/system/System.hh>

#include <vector>
#include <sstream>

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
# include <libgen.h>
# include <sys/types.h>
# include <dirent.h>
# include <QDir>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace system;

  namespace io
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method creates a directory at the given path.
    ///
    Status              Directory::Create(const Path&           path)
    {
      enter();

      if (!QDir().mkpath(QString::fromStdString(path.string)))
        escape("failed to mkpath: %s", path.string.c_str());

      leave();

#if 0
      enter();

      // does the directory already exist.
      if (Directory::Exist(path) == StatusTrue)
        escape("the directory seems to already exist");

      // dig the directory which will hold the target directory.
      if (Directory::Dig(path) == StatusError)
        escape("unable to dig the chain of directories");

      // create the directory.
#if defined(INFINIT_UNIX)
      if (::mkdir(path.string.c_str(), 0700) != 0)
        escape(::strerror(errno));
#elif defined(INFINIT_WIN32)
      if (::mkdir(path.string.c_str()) != 0)
        escape("unable to create %s: %s",
               path.string.c_str(), ::strerror(errno));
#else
# error "unsupported platform"
#endif

      leave();
#endif
    }

    ///
    /// this method removes a directory.
    ///
    Status              Directory::Remove(const Path&           path)
    {
      enter();

      // does the directory exist.
      if (Directory::Exist(path) == StatusFalse)
        escape("the directory does not seem to exist");

      // remove the directory.
      ::rmdir(path.string.c_str());

      leave();
    }

    ///
    /// this method returns true if the pointed to directory exists.
    ///
    Status              Directory::Exist(const Path&                    path)
    {
      struct ::stat             stat;

      enter();

      // does the path points to something.
      if (::stat(path.string.c_str(), &stat) != 0)
        false();

      // does the path points to a directory.
      if (!S_ISDIR(stat.st_mode))
        false();

      true();
    }

    ///
    /// this method takes a path to a directory and creates, if necessary,
    /// the intermediate directories leading to the target.
    ///
    Status              Directory::Dig(const Path&                      path)
    {
      String            directory(::dirname(const_cast<char*>(path.string.c_str())));

      enter();

      if (!QDir().mkpath(QString::fromStdString(directory)))
        escape("failed to mkpath: %s", directory.c_str());

      leave();

#if 0
      String            target(::strdup(path.string.c_str()));
      String            directory(::dirname(
                                    const_cast<char*>(target.c_str())));
      std::stringstream stream(directory);
      String            item;
      Path              chemin;

      enter();

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
#endif
    }

    ///
    /// this method recursively removes everything in the given directory.
    ///
    Status              Directory::Clear(const Path&            path)
    {
      ::DIR*            directory;
      struct ::dirent*  entry;

      enterx(slab(directory, ::closedir));

      // is the path pointing to a valid directory.
      if (Directory::Exist(path) == StatusFalse)
        escape("the path does not reference a directory");

      // open the directory.
      if ((directory = ::opendir(path.string.c_str())) == NULL)
        escape("unable to open the directory");

      // go through the entries.
      while ((entry = ::readdir(directory)) != NULL)
        {
          Path          target;
          struct ::stat stat;

          // ignore the . and ..
          if ((::strcmp(entry->d_name, ".") == 0) ||
              (::strcmp(entry->d_name, "..") == 0))
            continue;

          // create the target path.
          if (target.Create(path.string +
                            System::Path::Separator +
                            String(entry->d_name)) == StatusError)
            escape("unable to create the target path");

          // stat the entry as entry->d_type is not standard
#if defined(INFINIT_UNIX)
          if (::lstat(target.string.c_str(), &stat) == -1)
#elif defined(INFINIT_WIN32)
          if (::stat(target.string.c_str(), &stat) == -1)
#else
# error "unsupported platform"
#endif
            // the stat may fail but it's ok to continue as it's not fatal
            // and the entry may have been destroyed/moved between the readdir
            // and now.
            continue;

          // perform an action depending on the nature of the target.
          if (S_ISDIR(stat.st_mode))
            {
              // empty it as well.
              if (Directory::Clear(target) == StatusError)
                escape("unable to empty a subdirectory");

              // remove the directory.
              if (Directory::Remove(target) == StatusError)
                escape("unable to remove the subdirectory");
            }
          else if (S_ISREG(stat.st_mode))
            {
              // remove the file.
              if (File::Erase(target) == StatusError)
                escape("unable to remove the file");
            }
#if defined(INFINIT_UNIX)
          else if (S_ISLNK(stat.st_mode))
            {
              // remove the link.
              if (Link::Erase(target) == StatusError)
                escape("unable to remove the link");
            }
#endif
          else
            escape("unhandled file system object type");
        }

      // close the directory.
      ::closedir(directory);

      // waive.
      waive(directory);

      leave();
    }

    ///
    /// this method returns a list of directory entries.
    ///
    Status              Directory::List(const Path&             path,
                                        Set&                    set)
    {
      ::DIR*            directory;
      struct ::dirent*  entry;

      enterx(slab(directory, ::closedir));

      // open the directory.
      if ((directory = ::opendir(path.string.c_str())) == NULL)
        escape("unable to open the directory");

      // go through the entries.
      while ((entry = ::readdir(directory)) != NULL)
        {
          // ignore the '.' and '..' entries.
          if ((String(entry->d_name) == ".") ||
              (String(entry->d_name) == ".."))
            continue;

          // add the entry to the set.
          set.push_back(String(entry->d_name));
        }

      // close the directory.
      ::closedir(directory);

      // waive.
      waive(directory);

      leave();
    }

  }
}
