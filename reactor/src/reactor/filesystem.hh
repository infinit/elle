#ifndef REACTOR_FILESYSTEM_HH
#define REACTOR_FILESYSTEM_HH

#include <string>
#include <boost/filesystem.hpp>

#include <elle/Buffer.hh>
#include <elle/Exception.hh>

#ifdef INFINIT_WINDOWS
struct statvfs {
  unsigned long  f_bsize;    /* filesystem block size */
  unsigned long  f_frsize;   /* fragment size */
  unsigned long  f_blocks;   /* size of fs in f_frsize units */
  unsigned long  f_bfree;    /* # free blocks */
  unsigned long  f_bavail;   /* # free blocks for unprivileged users */
  unsigned long  f_files;    /* # inodes */
  unsigned long  f_ffree;    /* # free inodes */
  unsigned long  f_favail;   /* # free inodes for unprivileged users */
  unsigned long  f_fsid;     /* filesystem ID */
  unsigned long  f_flag;     /* mount flags */
  unsigned long  f_namemax;  /* maximum filename length */
};
#else
#include <sys/statvfs.h>
#endif
struct stat;
struct statvfs;
namespace reactor
{
  namespace filesystem
  {
    typedef
    std::function<void(std::string const&, struct stat* stbuf)>
    OnDirectoryEntry;

    /** Exception type for filesystem errors. Filesystems should not throw
     *  anything else.
     */
    class Error: public elle::Error
    {
    public:
      Error(int error_code, std::string const& message)
      : elle::Error(message)
      , _error_code(error_code)
      {}
      ELLE_ATTRIBUTE_R(int, error_code);
    };

    /// Handle to an open file
    class Handle
    {
    public:
      virtual ~Handle() {}
      virtual int read(elle::WeakBuffer buffer, size_t size, off_t offset)=0;
      virtual int write(elle::WeakBuffer buffer, size_t size, off_t offset)=0;
      virtual void ftruncate(off_t offset);
      virtual void close()=0;
    };

    class Path
    {
    public:
      virtual ~Path() {}

      virtual void stat(struct stat*) = 0;
      virtual void list_directory(OnDirectoryEntry cb) = 0;
      virtual std::unique_ptr<Handle> open(int flags, mode_t mode) = 0;
      /// Will bounce to open() if not implemented
      virtual std::unique_ptr<Handle> create(int flags, mode_t mode);
      /// Delete a file. Defaults to error(not implemented)
      virtual void unlink();
      virtual void mkdir(mode_t mode);
      virtual void rmdir();
      virtual void rename(boost::filesystem::path const& where);
      virtual boost::filesystem::path readlink();
      virtual void symlink(boost::filesystem::path const& where);
      virtual void link(boost::filesystem::path const& where);
      virtual void chmod(mode_t mode);
      virtual void chown(int uid, int gid);
      virtual void statfs(struct statvfs *);
      virtual void utimens(const struct timespec tv[2]);
      virtual void truncate(off_t new_size);
      /// Return a Path for given child name.
      virtual std::unique_ptr<Path> child(std::string const& name) = 0;

    };

    class FileSystem;
    class Operations
    {
    public:
      Operations();
      virtual
      std::unique_ptr<Path>
      path(std::string const& path) = 0;
      virtual ~Operations() {}
      ELLE_ATTRIBUTE_RW(FileSystem*, filesystem);
    };

    class FileSystemImpl;
    class FileSystem
    {
    public:
      /** Create a new file system with given operations
      *
      * @param full_tree: If set, will use Operations.path('/') only and
      *        traverse the filesystem tree using Path::child().
      *        Otherwise Operations::path() will be used with full pathes
      *        and Path::child will never be called.
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

    /// Handle implementation reading/writing to the local filesystem
    class BindHandle: public Handle
    {
    public:
      BindHandle(int fd, boost::filesystem::path const&);
      int read(elle::WeakBuffer buffer, size_t size, off_t offset) override;
      int write(elle::WeakBuffer buffer, size_t size, off_t offset) override;
      void ftruncate(off_t offset) override;
      void close() override;
    protected:
      int _fd;
      boost::filesystem::path _where;
    };

    class BindOperations: public Operations
    {
    public:
      BindOperations(boost::filesystem::path const& source);
      std::unique_ptr<Path> path(std::string const& path) override;
      ELLE_ATTRIBUTE_R(boost::filesystem::path, source);
    };

    /// Default Path implementation acting on the local filesystem.
    class BindPath: public Path
    {
    public:
      BindPath(boost::filesystem::path const& where,
               BindOperations& ops);
      void stat(struct stat*) override;
      void list_directory(OnDirectoryEntry cb) override;
      std::unique_ptr<Handle> open(int flags, mode_t mode) override;
      void unlink() override;
      void mkdir(mode_t mode) override;
      void rmdir() override;
      void rename(boost::filesystem::path const& where) override;
      boost::filesystem::path readlink() override;
      void symlink(boost::filesystem::path const& where) override;
      void link(boost::filesystem::path const& where) override;
      void chmod(mode_t mode) override;
      void chown(int uid, int gid) override;
      void statfs(struct statvfs *) override;
      void utimens(const struct timespec tv[2]) override;
      void truncate(off_t new_size);
      /// Return a Path for given child name.
      std::unique_ptr<Path> child(std::string const& name) override;
      virtual std::unique_ptr<BindHandle> make_handle(boost::filesystem::path& where,
                                                      int fd);
    private:
      ELLE_ATTRIBUTE_R(boost::filesystem::path, where);
      ELLE_ATTRIBUTE_R(BindOperations&, ops);
    };
  }
}

#endif