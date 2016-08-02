#ifndef REACTOR_FILESYSTEM_HH
# define REACTOR_FILESYSTEM_HH

# define _FILE_OFFSET_BITS 64

# include <string>

# include <unordered_map>
# include <boost/filesystem.hpp>

# include <elle/Buffer.hh>
# include <elle/Exception.hh>
# include <elle/filesystem.hh>

# include <reactor/waitable.hh>

static_assert(sizeof(off_t) == 8, "off_t is 32 bits long, define _FILE_OFFSET_BITS to 64");

# if defined(INFINIT_WINDOWS) || defined(INFINIT_ANDROID)
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
# else
#  include <sys/statvfs.h>
# endif

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
    class Error
      : public elle::Error
    {
    public:
      Error(int error_code, std::string const& message)
        : elle::Error(message)
        , _error_code(error_code)
      {}

      Error(int error_code, std::string const& message, elle::Backtrace const& bt)
        : elle::Error(bt, message)
        , _error_code(error_code)
      {}
      ELLE_ATTRIBUTE_R(int, error_code);
    };

    /// Handle to an open file
    class Handle
    {
    public:
      virtual
      ~Handle()
      {}

      virtual
      int
      read(elle::WeakBuffer buffer, size_t size, off_t offset)=0;

      virtual
      int
      write(elle::ConstWeakBuffer buffer, size_t size, off_t offset)=0;

      virtual
      void
      ftruncate(off_t offset);

      virtual
      void
      fsync(int datasync);

      virtual
      void
      fsyncdir(int datasync);

      virtual
      void
      close() = 0;

    };

    class Path
      : public std::enable_shared_from_this<Path>
    {
    public:
      virtual
      ~Path()
      {}

      virtual
      void
      stat(struct stat*) = 0;

      virtual
      void
      list_directory(OnDirectoryEntry cb) = 0;

      virtual
      std::unique_ptr<Handle>
      open(int flags, mode_t mode) = 0;

      /// Will bounce to open() if not implemented
      virtual
      std::unique_ptr<Handle>
      create(int flags, mode_t mode);

      /// Delete a file. Defaults to error(not implemented)
      virtual
      void
      unlink();

      virtual
      void
      mkdir(mode_t mode);

      virtual
      void
      rmdir();

      virtual
      void
      rename(boost::filesystem::path const& where);

      virtual
      boost::filesystem::path
      readlink();

      virtual
      void
      symlink(boost::filesystem::path const& where);

      virtual
      void
      link(boost::filesystem::path const& where);

      virtual
      void
      chmod(mode_t mode);

      virtual
      void
      chown(int uid, int gid);

      virtual
      void
      statfs(struct statvfs*);

      virtual
      void
      utimens(const struct timespec tv[2]);

      virtual
      void
      truncate(off_t new_size);

      virtual
      void
      setxattr(std::string const& name, std::string const& value, int flags);

      virtual
      std::string
      getxattr(std::string const& name);

      virtual
      std::vector<std::string> listxattr();

      virtual
      void
      removexattr(std::string const& name);

      /// Return a Path for given child name.
      virtual
      std::shared_ptr<Path>
      child(std::string const& name) = 0;

      /// Return true to allow the filesystem to keep this Path in cache.
      virtual
      bool
      allow_cache()
      {
        return true;
      }

      virtual std::shared_ptr<Path> unwrap()
      {
        return shared_from_this();
      }
    };

    class FileSystem;
    class Operations
    {
    public:
      Operations();

      virtual
      ~Operations() {}

      virtual
      std::shared_ptr<Path>
      path(std::string const& path) = 0;

      virtual
      std::shared_ptr<Path>
      wrap(std::string const& path, std::shared_ptr<Path> source)
      {
        return source;
      }
      virtual
      void
      filesystem(FileSystem* fs) { _filesystem = fs;}
      ELLE_ATTRIBUTE_R(FileSystem*, filesystem, protected);
    };

    class FileSystemImpl;
    class FileSystem
      : public reactor::Waitable
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

      void
      mount(boost::filesystem::path const& where,
            std::vector<std::string> const& options);
      /// Unmount waiting gracefully for current operations to end.
      void
      unmount();
      /// Kill current operations and the process loop. Use only while
      /// unmounting.
      void
      kill();

      std::shared_ptr<Path>
      path(std::string const& path);

    /*-----------------.
    | Cache operations |
    `-----------------*/
    public:
      std::shared_ptr<Path>
      extract(std::string const& path);

      std::shared_ptr<Path>
      set(std::string const& path,
          std::shared_ptr<Path> new_content);

      std::shared_ptr<Path>
      get(std::string const& path);

    /*---------.
    | Waitable |
    `---------*/
    protected:
      virtual
      bool
      _wait(Thread* thread, Waker const& waker) override;

    /*--------.
    | Details |
    `--------*/
    private:
      ELLE_ATTRIBUTE_R(FileSystemImpl*, impl);
      std::shared_ptr<Path>
      fetch_recurse(std::string path);
      ELLE_ATTRIBUTE_RX(std::unique_ptr<Operations>, operations);
      ELLE_ATTRIBUTE_R(std::vector<std::string>, mount_options);
      ELLE_ATTRIBUTE_RW(bool, full_tree);
      std::string _where;
      std::unordered_map<std::string, std::shared_ptr<Path>> _cache;
    };


    /// Handle implementation reading/writing to the local filesystem
    class BindHandle: public Handle
    {
    public:
      BindHandle(int fd, boost::filesystem::path const&);

      int
      read(elle::WeakBuffer buffer, size_t size, off_t offset) override;
      int
      write(elle::ConstWeakBuffer buffer, size_t size, off_t offset) override;
      void
      ftruncate(off_t offset) override;
      void
      close() override;

    protected:
      int _fd;
      boost::filesystem::path _where;
    };

    class BindOperations
      : public Operations
    {
    public:
      BindOperations(boost::filesystem::path const& source);

      std::shared_ptr<Path>
      path(std::string const& path) override;

      ELLE_ATTRIBUTE_R(boost::filesystem::path, source);
    };

    /// Default Path implementation acting on the local filesystem.
    class BindPath
      : public Path
    {
    public:
      BindPath(boost::filesystem::path const& where,
               BindOperations& ops);

      void
      stat(struct stat*) override;

      void
      list_directory(OnDirectoryEntry cb) override;

      std::unique_ptr<Handle>
      open(int flags, mode_t mode) override;

      void
      unlink() override;

      void
      mkdir(mode_t mode) override;

      void
      rmdir() override;

      void
      rename(boost::filesystem::path const& where) override;

      boost::filesystem::path
      readlink() override;

      void
      symlink(boost::filesystem::path const& where) override;

      void
      link(boost::filesystem::path const& where) override;

      void
      chmod(mode_t mode) override;

      void
      chown(int uid, int gid) override;

      void
      statfs(struct statvfs *) override;

      void
      utimens(const struct timespec tv[2]) override;

      void
      truncate(off_t new_size) override;

      /// Return a Path for given child name.
      std::shared_ptr<Path>
      child(std::string const& name) override;

      virtual
      std::unique_ptr<BindHandle>
      make_handle(boost::filesystem::path& where, int fd);

    private:
      ELLE_ATTRIBUTE_R(boost::filesystem::path, where);
      ELLE_ATTRIBUTE_R(BindOperations&, ops);
    };

    std::unique_ptr<Operations> install_journal(std::unique_ptr<Operations> backend,
                                                std::string const& path);
  }
}

#endif
