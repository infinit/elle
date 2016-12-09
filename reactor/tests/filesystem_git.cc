#include <elle/system/Process.hh>

#include <reactor/filesystem.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

namespace rfs = reactor::filesystem;
namespace bfs = boost::filesystem;


class GitHandle: public rfs::BindHandle
{
public:
  GitHandle(int fd, boost::filesystem::path const& where)
  : rfs::BindHandle(fd, where)
  {
  }
  void close() override
  {
    rfs::BindHandle::close();
    elle::system::Process({"git", "add", "."}).wait();
    elle::system::Process({"git", "commit", "-m", _where.string()}).wait();
  }
};
class GitPath: public rfs::BindPath
{
public:
  GitPath(boost::filesystem::path const& where,
               rfs::BindOperations& ops)
  : rfs::BindPath(where, ops)
  {
  }
  void unlink()
  {
    elle::system::Process({"git", "rm", where().string()}).wait();
    elle::system::Process({"git", "commit", "-m", "remove " + where().string()}).wait();
  }
  void rename(boost::filesystem::path const& target)
  {
    rfs::BindPath::rename(target);
    elle::system::Process({"git", "rm", where().string()}).wait();
    elle::system::Process({"git", "add", "."}).wait();
    elle::system::Process({"git", "commit", "-m", where().string() + " -> " + target.string()}).wait();
  }
  std::unique_ptr<rfs::BindHandle> make_handle(boost::filesystem::path& where,
                                          int fd) override
  {
    return std::make_unique<GitHandle>(fd, where);
  }
};
class GitFilesystemOperations: public rfs::BindOperations
{
public:
  GitFilesystemOperations(boost::filesystem::path const& source)
  : rfs::BindOperations(source)
  {
    bfs::current_path(source);
    if (!bfs::exists(".git"))
      elle::system::Process({"git", "init"}).wait();
  }
  std::shared_ptr<rfs::Path> path(std::string const& path) override
  {
    return std::make_shared<GitPath>(path, *this);
  }
};
rfs::FileSystem* fs;

static void run(int argc, char** argv)
{
  std::unique_ptr<rfs::Operations> ops =
    std::make_unique<GitFilesystemOperations>(bfs::path(argv[1]));
  fs = new rfs::FileSystem(std::move(ops), false);
  std::vector<std::string> args(argv + 2, argv + argc);
  fs->mount(argv[2], args);
}

static void sig_int()
{
  fs->unmount();
}
int main(int argc, char** argv)
{
  reactor::Scheduler sched;
  sched.signal_handle(SIGINT, sig_int);
  reactor::Thread t(sched, "filesystem", [&]{run(argc, argv);});
  sched.run();
}
