#ifndef REACTOR_FILESYSTEM_HH
#define REACTOR_FILESYSTEM_HH

#include <string>
#include <boost/filesystem.hpp>

#include <elle/Buffer.hh>
#include <elle/Exception.hh>

struct stat;
namespace reactor
{
  namespace filesystem
  {
    typedef
    std::function<void(std::string const&, struct stat* stbuf)>
    OnDirectoryEntry;

    class Error: public elle::Error
    {
    public:
      Error(int error_code, std::string const& message)
      : elle::Error(message)
      , _error_code(error_code)
      {}
      ELLE_ATTRIBUTE_R(int, error_code);
    };

    class Handle
    {
    public:
      virtual ~Handle() {}
      virtual int read(elle::WeakBuffer buffer, size_t size, off_t offset)=0;
      virtual void write(elle::WeakBuffer buffer, size_t size, off_t offset)=0;
      virtual void close()=0;
    };

    class Path
    {
    public:
      virtual ~Path() {}
      virtual void stat(struct stat*) = 0;
      virtual void list_directory(OnDirectoryEntry cb) = 0;
      virtual std::unique_ptr<Path> child(std::string const& name) = 0;
      virtual std::unique_ptr<Handle> open(int flags) = 0;
    };

    class Operations
    {
    public:
      virtual
      std::unique_ptr<Path>
      path(std::string const& path) = 0;
      virtual ~Operations() {}
    };

    class FileSystemImpl;
    class FileSystem
    {
    public:
      /** Create a new file system with given operations
      *
      * @param full_tree: If set, will use Operations.path('/') only and
      *        recurse using Path::child(). Otherwise Operations::path()
      *        will be used with full pathes and Path::child will never be called.
      */
      FileSystem(std::unique_ptr<Operations> op, bool full_tree);
      ~FileSystem();
      void mount(boost::filesystem::path const& where,
                 std::vector<std::string> const& options);
      void unmount();
      Path& path(std::string const& path);
    private:
      FileSystemImpl* _impl;
    };
  }
}

#endif