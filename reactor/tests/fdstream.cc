/// Crash on OS X when using: echo "something" | /path/to/fdstream

#include <reactor/FDStream.hh>
#include <reactor/scheduler.hh>

int
main(int argc, char** argv)
{
  reactor::Scheduler sched;
  reactor::Thread main_thread(
    sched,
    "main",
    [&]
    {
      reactor::FDStream stdin_stream(0);
    });
  sched.run();
  return 0;
}
