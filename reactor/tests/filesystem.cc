#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <boost/filesystem/fstream.hpp>

#include <reactor/filesystem.hh>
#include <reactor/scheduler.hh>
#include <reactor/signal.hh>
#include <elle/test.hh>

ELLE_LOG_COMPONENT("Test");


namespace sum
{
  class Handle: public reactor::filesystem::Handle
  {
  public:
    Handle(int num)
    : num(num) {}
    int read(elle::WeakBuffer buffer, size_t size, off_t offset) override
    {
      ELLE_TRACE("read %s/%s", offset, size);
      std::string val = std::to_string(num) + "\n";
      strncpy((char*)buffer.contents(), val.c_str(), std::min(val.length()+1, size));
      return size;
    }
    int write(elle::WeakBuffer buffer, size_t size, off_t offset) override
    {
      throw reactor::filesystem::Error(EPERM, "Write access denied");
    }
    void close() override {}
    int num;
  };
  class Path: public reactor::filesystem::Path
  {
  public:
    Path(int num, bool isSum = false)
    : num(num)
    , isDir(!isSum)
    {
    }
    void stat(struct stat* stbuf) override
    {
      ELLE_DEBUG("%s.stat(%s)", this, stbuf);
      memset(stbuf, 0, sizeof(struct stat));
      if (isDir)
        stbuf->st_mode = S_IFDIR | 0755;
      else
      {
        stbuf->st_mode = S_IFREG | 0444;
        // We have to report a dummy non-empty size
        stbuf->st_size = 4096;
      }
    }
    void list_directory(reactor::filesystem::OnDirectoryEntry cb) override
    {
      if (!isDir)
        throw reactor::filesystem::Error(ENOTDIR, "Not a directory");
      for (unsigned i=0; i<100; ++i)
      {
        cb(std::to_string(i), nullptr);
      }
      cb("sum", nullptr);
    }
    std::unique_ptr<reactor::filesystem::Path> child(std::string const& name) override
    {
      ELLE_LOG("child %s", name);
      if (!isDir)
        throw reactor::filesystem::Error(ENOTDIR, "Not a directory");
      if (name == "sum")
        return elle::make_unique<Path>(num, true);
      std::size_t pos;
      int n;
      try {
        n = std::stol(name, &pos, 0);
      }
      catch(...)
      {
        throw reactor::filesystem::Error(ENOENT, "No such file or directory");
      }
      if (pos < name.size())
        throw reactor::filesystem::Error(ENOENT, "No such file or directory");
      return elle::make_unique<Path>(n + num);
    }
    std::unique_ptr<reactor::filesystem::Handle> open(int flags)
    {
      if ((flags & 3) != O_RDONLY)
        throw reactor::filesystem::Error(EACCES, "Write access denied");
      return elle::make_unique<Handle>(num);
    }
    int num;
    bool isDir;
  };

  class Operations: public reactor::filesystem::Operations
  {
  public:
    std::unique_ptr<reactor::filesystem::Path> path(std::string const& path) override
    {
      ELLE_ASSERT(path == "/");
      return elle::make_unique<Path>(0);
    }
  };
}

static void run_filesystem(reactor::filesystem::FileSystem &fs,
             boost::filesystem::path tmp)
{
  reactor::Scheduler sched;
  reactor::Thread t(sched, "mount", [&] { fs.mount(tmp, {});});
  sched.run();
}

static void test_sum(void)
{
  reactor::filesystem::FileSystem fs(elle::make_unique<sum::Operations>(), true);
  auto tmp = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
  boost::filesystem::create_directories(tmp);
  std::thread t([&] { run_filesystem(fs, tmp);});
  ::usleep(500000);
  ELLE_LOG("will mount on %s", tmp);
  int s;
  boost::filesystem::ifstream(tmp/"1"/"sum") >> s;
  BOOST_CHECK_EQUAL(s, 1);
  boost::filesystem::ifstream(tmp/"10"/"12"/"sum") >> s;
  BOOST_CHECK_EQUAL(s, 22);
  s=0;
  boost::filesystem::ifstream(tmp/"10"/"12"/"sum") >> s;
  BOOST_CHECK_EQUAL(s, 22);
  {
    boost::filesystem::directory_iterator d(tmp);
    s=0;
    while (d!= boost::filesystem::directory_iterator())
    {
      ++s; ++d;
    }
    BOOST_CHECK_EQUAL(s, 101);
  }
  fs.unmount();
  t.join();
  {
    boost::filesystem::directory_iterator d(tmp);
    s=0;
    while (d!= boost::filesystem::directory_iterator())
    {
      ++s; ++d;
    }
    BOOST_CHECK_EQUAL(s, 0);
  }
}

namespace xorfs
{
  namespace rfs = reactor::filesystem;
  namespace bfs = boost::filesystem;
  class Encrypt: public rfs::Operations
  {
  public:
    std::unique_ptr<rfs::Path> path(std::string const&) override;
    Encrypt(bfs::path storage): storage(storage) {}
    bfs::path storage;
  };
  class Handle: public rfs::Handle
  {
  public:
    Handle(Encrypt& ctx, bfs::path path, int flags)
    {
      ELLE_TRACE("open %s with flags %s", path, flags);
      bfs::path p = ctx.storage / path;
      struct stat st;
      ::stat(p.string().c_str(), &st);
      if (! S_ISREG(st.st_mode))
        throw rfs::Error(EINVAL, "Not a regular file");
      fd = open(p.string().c_str(), flags, 0);
    }
    int read(elle::WeakBuffer buffer, size_t size, off_t offset) override
    {
      lseek(fd, offset, SEEK_SET);
      int len = ::read(fd, (void*)buffer.contents(), size);
      if (len <=0)
        return len;
      for (int i=0; i<len; ++i)
        buffer[i] = buffer[i] ^ 0xFF;
      return len;
    }
    int write(elle::WeakBuffer buffer, size_t size, off_t offset) override
    {
      lseek(fd, offset, SEEK_SET);
      for (int i=0; i<size; ++i)
        buffer[i] = buffer[i] ^ 0xFF;
      return ::write(fd, buffer.contents(), size);
    }
    void close() override
    {
      ::close(fd);
    }
    int fd;
  };
  class Path: public rfs::Path
  {
  public:
    Path(bfs::path where, Encrypt& ctx)
    : ctx(ctx)
    , where(where)
    {}
    void stat(struct stat* st) override
    {
      ELLE_TRACE("stat %s/%s", ctx.storage, where);
      ELLE_TRACE("stat %s", ctx.storage / where);
      bfs::path p = ctx.storage / where;

      ::stat(p.string().c_str(), st);
    }
    void list_directory(rfs::OnDirectoryEntry cb) override
    {
      ELLE_TRACE("listdir %s/%s", ctx.storage, where);
      ELLE_TRACE("listdir %s", ctx.storage / where);
      bfs::path p = ctx.storage / where;
      bfs::directory_iterator it(p);
      for(;it != bfs::directory_iterator(); ++it)
      {
        ELLE_TRACE("->%s", it->path());
        cb(it->path().filename().string(), nullptr);
      }
      ELLE_TRACE("end listdir");
    }
    std::unique_ptr<rfs::Path> child(std::string const& name) override
    {
      ELLE_ASSERT(false);
      return std::unique_ptr<rfs::Path>{};
    }
    std::unique_ptr<rfs::Handle> open(int flags) override
    {
      return elle::make_unique<Handle>(ctx, where, flags);
    }
    Encrypt& ctx;
    bfs::path where;
  };
  std::unique_ptr<rfs::Path> Encrypt::path(std::string const& p)
  {
    return elle::make_unique<Path>(p, *this);
  }
}


static void test_xor(void)
{
  auto tmpmount = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
  auto tmpsource = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();

  reactor::filesystem::FileSystem fs(
    elle::make_unique<xorfs::Encrypt>(tmpsource),
    false);
  boost::filesystem::create_directories(tmpmount);
  boost::filesystem::create_directories(tmpsource);
  ELLE_LOG("mount: %s   source: %s", tmpmount, tmpsource);
  std::thread t([&] { run_filesystem(fs, tmpmount);});
  while (true)
    ::usleep(500000);

}

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* filesystem = BOOST_TEST_SUITE("filesystem");
  boost::unit_test::framework::master_test_suite().add(filesystem);
  filesystem->add(BOOST_TEST_CASE(test_sum), 0, 5);
  filesystem->add(BOOST_TEST_CASE(test_xor), 0, 500);
}
