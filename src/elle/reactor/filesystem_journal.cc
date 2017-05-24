#define _FILE_OFFSET_BITS 64

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <utility>

#include <elle/reactor/filesystem.hh>
#include <elle/serialization/json.hh>

ELLE_LOG_COMPONENT("elle.reactor.filesystem.journal");

namespace elle
{
  namespace reactor
  {
    namespace filesystem
    {
      static bool True = true;

#define REACTOR_FILESYSTEM_ERROR(name)                                  \
      catch (Error const& e)                                            \
      {                                                                 \
        _owner.push_fail(_full_path, name, e.error_code(), e.what());   \
        throw;                                                          \
      }                                                                 \

#define REACTOR_FILESYSTEM_HERROR(name)                                 \
      catch(Error const& e)                                             \
      {                                                                 \
        bool fail = false;                                              \
        int erc = e.error_code();                                       \
        std::string message = e.what();                                 \
        _owner.push_out(PushSerializer(this, name)("success", fail)     \
                        ("code", erc)("message", message)());           \
        throw;                                                          \
      }                                                                 \

      class PushSerializer
      {
      public:
        PushSerializer(std::string const& path, std::string const& op)
        : ios(b.ostreambuf())
        , _serializer(new elle::serialization::json::SerializerOut(ios))
        {
          _serializer->serialize("path", path);
          _serializer->serialize("operation", op);
        }
        PushSerializer(void* handle, std::string const& op)
        : ios(b.ostreambuf())
        , _serializer(new elle::serialization::json::SerializerOut(ios))
        {
          std::string shandle = std::to_string((uint64_t)handle);
          _serializer->serialize("handle", shandle);
          _serializer->serialize("operation", op);
        }
        PushSerializer(PushSerializer const& b) = delete;
        PushSerializer(PushSerializer&& b) = default;
        template<typename T>
        PushSerializer& operator()(std::string const& k, T& v)
        {
          _serializer->serialize(k, v);
          return *this;
        }
        elle::Buffer
        operator()()
        {
          _serializer.reset();
          ios.flush();
          return std::move(b);
        }
        elle::Buffer b;
        elle::IOStream ios;
        std::unique_ptr<elle::serialization::json::SerializerOut> _serializer;
      };

      class JournalOperations: public Operations
      {
      public:
        JournalOperations(std::unique_ptr<Operations> backend, std::string const& path)
        : _backend(std::move(backend))
        , _in_op(0)
        {
          _in.reset(new std::ofstream(path + "in"));
          _out.reset(new std::ofstream(path + "out"));
        }
        void
        filesystem(FileSystem* fs) override
        {
          _filesystem = fs;
          _backend->filesystem(fs);
        }
        std::shared_ptr<Path>
        path(std::string const& path) override;
        std::shared_ptr<Path>
        wrap(std::string const& path, std::shared_ptr<Path> in) override;
        void push_op(std::string const& path, std::string const& op)
        {
          push_in(PushSerializer(path, op)());
        }
        void push_ok(std::string const& path, std::string const& op)
        {
          push_out(PushSerializer(path, op)("success", True)());
        }
        void push_fail(std::string const& path, std::string const& op,
                       int erc, std::string const& msg)
        {
          bool False = false;
          push_out(PushSerializer(path, op)("success", False)("code", erc)
            ("message", msg)());
        }
        void push_out(elle::Buffer buf)
        {
          _out->write((const char*)buf.contents(), buf.size());
          _out->flush();
        }
        void push_in(elle::Buffer buf)
        {
          _in->write((const char*)buf.contents(), buf.size());
          _in->flush();
        }
      private:
        std::unique_ptr<Operations> _backend;
        std::shared_ptr<std::ostream> _in;
        std::shared_ptr<std::ostream> _out;
        ELLE_ATTRIBUTE_R(int, in_op);
        friend class InOp;
      };
      class InOp
      {
      public:
        InOp(JournalOperations& j)
        : _j(j)
        {
          ++j._in_op;
        }
        ~InOp()
        {
          --_j._in_op;
        }
        JournalOperations& _j;
      };
      std::unique_ptr<Operations> install_journal(std::unique_ptr<Operations> backend,
                                                  std::string const& path)
      {
        return std::make_unique<JournalOperations>(std::move(backend), path);
      }

      class JournalHandle: public Handle
      {
      public:
        JournalHandle(JournalOperations& owner, std::unique_ptr<Handle> backend)
        : _owner(owner)
        , _backend(std::move(backend))
        {
        }
      private:
        JournalOperations& _owner;
        std::unique_ptr<Handle> _backend;
      public:
        ~JournalHandle() override
        {
          _owner.push_in(
            PushSerializer(this, "dispose")()
          );
          _owner.push_out(PushSerializer(this, "dispose")("success", True)());
        }
        void close() override
        {
          _owner.push_in(
            PushSerializer(this, "close")()
          );
          try
          {
            _backend->close();
            _owner.push_out(PushSerializer(this, "close")("success", True)());
          }
          REACTOR_FILESYSTEM_HERROR("close")
        }
        void fsyncdir(int datasync) override
        {
          _owner.push_in(
            PushSerializer(this, "fsyncdir")("datasync", datasync)()
          );
          try
          {
            _backend->fsyncdir(datasync);
            _owner.push_out(PushSerializer(this, "fsyncdir")("success", True)());
          }
          REACTOR_FILESYSTEM_HERROR("fsyncdir")
        }
        void fsync(int datasync) override
        {
          _owner.push_in(
            PushSerializer(this, "fsync")("datasync", datasync)()
          );
          try
          {
            _backend->fsyncdir(datasync);
            _owner.push_out(PushSerializer(this, "fsync")("success", True)());
          }
          REACTOR_FILESYSTEM_HERROR("fsync")
        }
        void ftruncate(off_t offset) override
        {
          _owner.push_in(
            PushSerializer(this, "ftruncate")("size", offset)()
          );
          try
          {
            _backend->ftruncate(offset);
            _owner.push_out(PushSerializer(this, "ftruncate")("success", True)());
          }
          REACTOR_FILESYSTEM_HERROR("ftruncate")
        }
        int read(elle::WeakBuffer buffer, size_t size, off_t offset) override
        {
          auto size_ = uint64_t(size);
          _owner.push_in(
            PushSerializer(this, "read")("size", size_)("offset", offset)()
          );
          try
          {
            int res = _backend->read(buffer, size, offset);
            elle::Buffer buf(buffer.contents(), res);
            _owner.push_out(PushSerializer(this, "read")("content", buf)("success", True)());
            return res;
          }
          REACTOR_FILESYSTEM_HERROR("read")
        }

        int
        write(elle::ConstWeakBuffer buffer, size_t size, off_t offset) override
        {
          elle::Buffer buf(buffer.contents(), buffer.size());
          auto size_ = uint64_t(size);
          _owner.push_in(
            PushSerializer(this, "write")("size", size_)("offset", offset)("content", buf)()
          );
          try
          {
            int res = _backend->write(buffer, size, offset);
            _owner.push_out(PushSerializer(this, "write")("success", True)());
            return res;
          }
          REACTOR_FILESYSTEM_HERROR("write")
        }
      };

      class JournalPath: public Path
      {
      public:
        JournalPath(JournalOperations& owner, std::shared_ptr<Path> backend,
                    std::string  full_path)
        : _owner(owner)
        , _full_path(std::move(full_path))
        , _backend(std::move(backend))
        {}
      private:
        JournalOperations& _owner;
        std::string _full_path;
        std::shared_ptr<Path> _backend;
      public:
        std::shared_ptr<Path> child(std::string const& name) override
        {
          ELLE_DEBUG("journal_child %s", name);
          InOp inop(_owner);
          auto res = _backend->child(name);
          return std::make_shared<JournalPath>(_owner, res,
            _full_path + (_full_path == "/" ? "" : "/") + name);
        }
        std::unique_ptr<Handle> create(int flags, mode_t mode) override
        {
          InOp inop(_owner);
          void* h = nullptr;
          elle::SafeFinally pushin([&] {
              std::string shandle = std::to_string((uint64_t)h);
              _owner.push_in(
                PushSerializer(_full_path, "create")("mode", mode)("flags", flags)
                ("handle", shandle)
                ()
            );
          });
          try
          {
            auto bh = _backend->create(flags, mode);
            auto handle = std::make_unique<JournalHandle>(_owner, std::move(bh));
            h = handle.get();
            _owner.push_ok(_full_path, "create");
            return std::move(handle);
          }
          REACTOR_FILESYSTEM_ERROR("create")
        }
        std::unique_ptr<Handle> open(int flags, mode_t mode) override
        {
          InOp inop(_owner);
          void* h = nullptr;
          elle::SafeFinally pushin([&] {
              std::string shandle = std::to_string((uint64_t)h);
              _owner.push_in(
                PushSerializer(_full_path, "open")("mode", mode)("flags", flags)
                ("handle", shandle)
                ()
            );
          });
          try
          {
            auto bh = _backend->open(flags, mode);
            auto handle = std::make_unique<JournalHandle>(_owner, std::move(bh));
            h = handle.get();
            _owner.push_ok(_full_path, "open");
            return std::move(handle);
          }
          REACTOR_FILESYSTEM_ERROR("open")
        }
        void stat(struct stat* s) override
        {
          ELLE_DEBUG("journal_stat %s", _full_path);
          InOp inop(_owner);
          _owner.push_op(_full_path, "stat");
          try
          {
            _backend->stat(s);
            elle::Buffer buf;
            {
              elle::IOStream ios(buf.ostreambuf());
              elle::serialization::json::SerializerOut response(ios);
              response.serialize("success", true);
              response.serialize("operation", "stat");
              response.serialize("path"      , this->_full_path);
              response.serialize("st_dev"    , s->st_dev    );
              response.serialize("st_ino"    , s->st_ino    );
              response.serialize("st_mode"   , s->st_mode   );
              response.serialize("st_nlink"  , s->st_nlink  );
              response.serialize("st_uid"    , s->st_uid    );
              response.serialize("st_gid"    , s->st_gid    );
              response.serialize("st_rdev"   , s->st_rdev   );
              response.serialize("st_size"   , int64_t(s->st_size));
#ifndef INFINIT_WINDOWS
              response.serialize("st_blksize", int64_t(s->st_blksize));
              response.serialize("st_blocks" , s->st_blocks );
#endif
              response.serialize("st_atime"  , uint64_t(s->st_atime));
              response.serialize("st_mtime"  , uint64_t(s->st_mtime));
              response.serialize("st_ctime"  , uint64_t(s->st_ctime));
            }
            _owner.push_out(buf);
          }
          REACTOR_FILESYSTEM_ERROR("stat");
        }
        void mkdir(mode_t mode) override
        {
          InOp inop(_owner);
          _owner.push_in(
            PushSerializer(_full_path, "mkdir")("mode", mode)()
          );
          try
          {
            _backend->mkdir(mode);
            _owner.push_ok(_full_path, "mkdir");
          }
          REACTOR_FILESYSTEM_ERROR("mkdir");
        }
        void rmdir() override
        {
          _owner.push_op(_full_path, "rmdir");
          try
          {
            _backend->rmdir();
            _owner.push_ok(_full_path, "rmdir");
          }
          REACTOR_FILESYSTEM_ERROR("rmdir");
        }
        void list_directory(OnDirectoryEntry cb) override
        {
          InOp inop(_owner);
          _owner.push_op(_full_path, "list_directory");
          try
          {
            std::vector<std::string> content;
            _backend->list_directory([&](std::string const& name, struct stat* st) {
                cb(name, st);
                content.push_back(name);
            });
            _owner.push_out(PushSerializer(_full_path, "list_directory")
              ("success", True)
              ("entries", content)());
          }
          REACTOR_FILESYSTEM_ERROR("list_directory");
        }
        void unlink() override
        {
          InOp inop(_owner);
          _owner.push_op(_full_path, "unlink");
          try
          {
            _backend->unlink();
            _owner.push_ok(_full_path, "unlink");
          }
          REACTOR_FILESYSTEM_ERROR("unlink");
        }
        void rename(bfs::path const& where) override
        {
          InOp inop(_owner);
          _owner.push_in(
            PushSerializer(_full_path, "rename")("target", where.string())()
          );
          try
          {
            _backend->rename(where);
            _owner.push_ok(_full_path, "rename");
          }
          REACTOR_FILESYSTEM_ERROR("rename");
        }
        bfs::path readlink() override
        {
          InOp inop(_owner);
          _owner.push_op(_full_path, "readlink");
          try
          {
            auto res = _backend->readlink();
            _owner.push_out(PushSerializer(_full_path, "readlink")
              ("success", True)
              ("target", res.string())());
            return res;
          }
          REACTOR_FILESYSTEM_ERROR("readlink");
        }
        void symlink(bfs::path const& where) override
        {
          InOp inop(_owner);
          _owner.push_in(
            PushSerializer(_full_path, "symlink")("target", where.string())()
          );
          try
          {
            _backend->symlink(where);
            _owner.push_ok(_full_path, "symlink");
          }
          REACTOR_FILESYSTEM_ERROR("symlink");
        }
        void link(bfs::path const& where) override
        {
          ELLE_DEBUG("journal_link");
          InOp inop(_owner);
          _owner.push_in(
            PushSerializer(_full_path, "link")("target", where.string())()
          );
          try
          {
            _backend->link(where);
            _owner.push_ok(_full_path, "link");
          }
          REACTOR_FILESYSTEM_ERROR("link");
        }
        void chmod(mode_t mode) override
        {
          InOp inop(_owner);
          _owner.push_in(
            PushSerializer(_full_path, "chmod")("mode", mode)()
          );
          try
          {
            _backend->chmod(mode);
            _owner.push_ok(_full_path, "chmod");
          }
          REACTOR_FILESYSTEM_ERROR("chmod");
        }
        void chown(int uid, int gid) override
        {
          InOp inop(_owner);
          _owner.push_in(
            PushSerializer(_full_path, "chown")("uid", uid)("gid", gid)()
          );
          try
          {
            _backend->chown(uid, gid);
            _owner.push_ok(_full_path, "chown");
          }
          REACTOR_FILESYSTEM_ERROR("chown");
        }
        void statfs(struct statvfs* st) override
        {
          InOp inop(_owner);
          _owner.push_op(_full_path, "statfs");
          try
          {
            _backend->statfs(st);
            elle::Buffer buf;
            {
              elle::IOStream ios(buf.ostreambuf());
              elle::serialization::json::SerializerOut response(ios);
              response.serialize("success", true);
              response.serialize("f_bsize",   uint64_t(st->f_bsize));
              response.serialize("f_frsize",  uint64_t(st->f_frsize));
              response.serialize("f_blocks",  uint64_t(st->f_blocks));
              response.serialize("f_bfree",   uint64_t(st->f_bfree));
              response.serialize("f_bavail",  uint64_t(st->f_bavail));
              response.serialize("f_files",   uint64_t(st->f_files));
              response.serialize("f_ffree",   uint64_t(st->f_ffree));
              response.serialize("f_favail",  uint64_t(st->f_favail));
              response.serialize("f_fsid",    uint64_t(st->f_fsid));
              response.serialize("f_flag",    uint64_t(st->f_flag));
              response.serialize("f_namemax", uint64_t(st->f_namemax));
            }
            _owner.push_out(buf);
          }
          REACTOR_FILESYSTEM_ERROR("statfs");
        }
        void utimens(const struct timespec tv[2]) override
        {
          InOp inop(_owner);
          uint64_t ta = tv[0].tv_sec * 1000000000ULL + tv[0].tv_nsec;
          uint64_t tc = tv[1].tv_sec * 1000000000ULL + tv[1].tv_nsec;
          _owner.push_in(
            PushSerializer(_full_path, "utimens")("access", ta)("change", tc)()
          );
          try
          {
            _backend->utimens(tv);
            _owner.push_ok(_full_path, "utimens");
          }
          REACTOR_FILESYSTEM_ERROR("utimens");
        }
        void truncate(off_t new_size) override
        {
          InOp inop(_owner);
          _owner.push_in(
            PushSerializer(_full_path, "truncate")("size", new_size)()
          );
          try
          {
            _backend->truncate(new_size);
            _owner.push_ok(_full_path, "truncate");
          }
          REACTOR_FILESYSTEM_ERROR("truncate");
        }
        void setxattr(std::string const& name, std::string const& value,
                              int flags) override
        {
          InOp inop(_owner);
          _owner.push_in(
            PushSerializer(_full_path, "setxattr")("name", name)("value", value)()
          );
          try
          {
            _backend->setxattr(name, value, flags);
            _owner.push_ok(_full_path, "setxattr");
          }
          REACTOR_FILESYSTEM_ERROR("setxattr");
        }
        std::string getxattr(std::string const& name) override
        {
          InOp inop(_owner);
          _owner.push_in(
            PushSerializer(_full_path, "getxattr")("name", name)()
          );
          try
          {
            auto res = _backend->getxattr(name);
            _owner.push_out(PushSerializer(_full_path, "getxattr")
              ("success", True)
              ("value", res)());
            return res;
          }
          REACTOR_FILESYSTEM_ERROR("getxattr")
        }
        std::vector<std::string> listxattr() override
        {
          InOp inop(_owner);
          _owner.push_op(_full_path, "listxattr");
          try
          {
            auto res = _backend->listxattr();
            _owner.push_out(PushSerializer(_full_path, "listxattr")
              ("entries", res)("success", True)());
            return res;
          }
          REACTOR_FILESYSTEM_ERROR("listxattr");
        }
        void removexattr(std::string const& name) override
        {
          InOp inop(_owner);
           _owner.push_in(
             PushSerializer(_full_path, "removexattr")("name", name)()
           );
           try
           {
             _backend->removexattr(name);
             _owner.push_ok(_full_path, "removexattr");
           }
           REACTOR_FILESYSTEM_ERROR("removexattr");
        }
        std::shared_ptr<Path> unwrap() override
        {
          ELLE_DEBUG("unwrap: %s", _owner.in_op());
          if (_owner.in_op())
            return _backend;
          else
            return shared_from_this();
        }
      };

      std::shared_ptr<Path>
      JournalOperations::path(std::string const& p)
      {
        auto res = _backend->path(p);
        return std::make_shared<JournalPath>(*this, res, p);
      }
      std::shared_ptr<Path>
      JournalOperations::wrap(std::string const& path, std::shared_ptr<Path> in)
      {
        if (std::dynamic_pointer_cast<JournalPath>(in))
          return in;
        else
          return std::make_shared<JournalPath>(*this, in, path);
      }
    }
  }
}
