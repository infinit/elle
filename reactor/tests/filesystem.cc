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
    void write(elle::WeakBuffer buffer, size_t size, off_t offset) override
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

static void run_sum(reactor::filesystem::FileSystem &fs,
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
  std::thread t([&] { run_sum(fs, tmp);});
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


ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* filesystem = BOOST_TEST_SUITE("filesystem");
  boost::unit_test::framework::master_test_suite().add(filesystem);
  filesystem->add(BOOST_TEST_CASE(test_sum), 0, 5);
}
