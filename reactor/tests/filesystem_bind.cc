#include <reactor/filesystem.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

namespace rfs = reactor::filesystem;
rfs::FileSystem* fs;

static void run(int argc, char** argv)
{
  std::unique_ptr<rfs::Operations> ops =
    std::make_unique<rfs::BindOperations>(argv[1]);
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