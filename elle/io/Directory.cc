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

# include <boost/noncopyable.hpp>
# include <QDir>

# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
# include <libgen.h>
# include <sys/types.h>
# include <dirent.h>
#include <elle/idiom/Open.hh>

namespace // usefull classes
{

  ///
  /// This class abstracts low level calls to
  /// iterate through directories
  ///
  class _Directory : private boost::noncopyable
  {
  public:
    class iterator
    {
    private:
      ::DIR*            _dir;
      struct ::dirent*  _entry;
      elle::String      _current;

    public:
      iterator(::DIR* dir) :
        _dir(dir), _entry(nullptr), _current()
      {
        if (dir != nullptr)
        {
          this->_entry = ::readdir(this->_dir);
          if (this->_entry != nullptr)
            this->_current.assign(this->_entry->d_name);
        }
      }
      iterator(iterator const& other) :
        _dir(other._dir), _entry(other._entry), _current(other._current)
      {}

      iterator& operator ++()
      {
        if (this->_entry == nullptr)
          throw std::range_error("No more entry in this directory");
        this->_entry = ::readdir(this->_dir);
        if (this->_entry != nullptr)
          this->_current.assign(this->_entry->d_name);
        return *this;
      }

      bool operator ==(iterator const& other) const
      { return this->_entry == other._entry; }

      bool operator !=(iterator const& other) const
      { return this->_entry != other._entry; }

      elle::String const& operator*() const
      {
        if (this->_entry == nullptr)
          throw std::runtime_error("Access to an invalid directory iterator");
        return this->_current;
      }

      elle::String const* operator ->() const
      {
        if (this->_entry == nullptr)
          throw std::runtime_error("Access to an invalid directory iterator");
        return &this->_current;
      }

    private:
      iterator& operator=(iterator const&); // yes, I'm lazy
    };

  private:
    ::DIR* _dir;

  public:
    _Directory(elle::io::Path const& path) :
      _dir(::opendir(path.string.c_str()))
    {}
    ~_Directory()
    {
      if (_dir != nullptr)
        ::closedir(this->_dir);
    }
    operator bool() const { return this->_dir != nullptr; }
    iterator begin() { return iterator(this->_dir); }
    iterator end() { return iterator(nullptr); }
  };

} // !ns anonymous

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
      ;

      if (!QDir().mkpath(QString::fromStdString(path.string)))
        escape("failed to mkpath: %s", path.string.c_str());

      return elle::StatusOk;

#if 0
      ;

      // does the directory already exist.
      if (Directory::Exist(path) == StatusTrue)
        escape("the directory seems to already exist");

      // dig the directory which will hold the target directory.
      if (Directory::Dig(path) == StatusError)
        escape("unable to dig the chain of directories");

      // create the directory.
#if defined(INFINIT_UNIX) || defined(INFINIT_MACOSX)
      if (::mkdir(path.string.c_str(), 0700) != 0)
        escape(::strerror(errno));
#elif defined(INFINIT_WINDOWS)
      if (::mkdir(path.string.c_str()) != 0)
        escape("unable to create %s: %s",
               path.string.c_str(), ::strerror(errno));
#else
# error "unsupported platform"
#endif

      return elle::StatusOk;
#endif
    }

    ///
    /// this method removes a directory.
    ///
    Status              Directory::Remove(const Path&           path)
    {
      ;

      // does the directory exist.
      if (Directory::Exist(path) == StatusFalse)
        escape("the directory does not seem to exist");

      // remove the directory.
      ::rmdir(path.string.c_str());

      return elle::StatusOk;
    }

    ///
    /// this method returns true if the pointed to directory exists.
    ///
    Status              Directory::Exist(const Path&                    path)
    {
      struct ::stat             stat;

      ;

      // does the path points to something.
      if (::stat(path.string.c_str(), &stat) != 0)
        return elle::StatusFalse;

      // does the path points to a directory.
      if (!S_ISDIR(stat.st_mode))
        return elle::StatusFalse;

      return elle::StatusTrue;
    }

    ///
    /// this method takes a path to a directory and creates, if necessary,
    /// the intermediate directories leading to the target.
    ///
    Status              Directory::Dig(const Path&                      path)
    {
      String            directory(::dirname(const_cast<char*>(path.string.c_str())));

      ;

      if (!QDir().mkpath(QString::fromStdString(directory)))
        escape("failed to mkpath: %s", directory.c_str());

      return elle::StatusOk;

#if 0
      String            target(::strdup(path.string.c_str()));
      String            directory(::dirname(
                                    const_cast<char*>(target.c_str())));
      std::stringstream stream(directory);
      String            item;
      Path              chemin;

      ;

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

      return elle::StatusOk;
#endif
    }


    ///
    /// this method recursively removes everything in the given directory.
    ///
    Status              Directory::Clear(const Path&            path)
    {
      // is the path pointing to a valid directory.
      if (Directory::Exist(path) == StatusFalse)
        escape("the path does not reference a directory");

      _Directory directory(path);

      // open the directory.
      if (!directory)
        escape("unable to open the directory");

      auto it = directory.begin();
      auto end = directory.end();
      for (; it != end; ++it)
        {
          Path          target;
          struct ::stat stat;

          // ignore the . and ..
          if (*it == "." || *it == "..")
            continue;

          // create the target path.
          String path_str(path.string + System::Path::Separator + *it);
          if (target.Create(path_str) == StatusError)
            escape("unable to create the target path");

          // stat the entry as entry->d_type is not standard
#if defined(INFINIT_UNIX) || defined(INFINIT_MACOSX)
          if (::lstat(target.string.c_str(), &stat) == -1)
#elif defined(INFINIT_WINDOWS)
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
#if defined(INFINIT_UNIX) || defined(INFINIT_MACOSX)
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

      return elle::StatusOk;
    }

    ///
    /// this method returns a list of directory entries.
    ///
    Status              Directory::List(const Path&             path,
                                        Set&                    set)
    {
      _Directory directory(path);

      // open the directory.
      if(!directory)
        escape("unable to open the directory");

      // go through the entries.
      auto it = directory.begin(),
           end = directory.end();
      for (; it != end; ++it)
        {
          // ignore the '.' and '..' entries.
          if (*it != "." && *it != "..")
            set.push_back(*it);
        }

      return elle::StatusOk;
    }

  }
}
