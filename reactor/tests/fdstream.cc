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
      static const std::string expected("something");
      reactor::FDStream stdin_stream(0);
      ELLE_ASSERT(!stdin_stream.eof());
      char content[1024] = {0};
      stdin_stream.read(content, sizeof(content));
      ELLE_ASSERT_EQ(static_cast<unsigned long>(stdin_stream.gcount()),
                     expected.size());
      ELLE_ASSERT_EQ(content, expected);
      ELLE_ASSERT(stdin_stream.eof());
    });
  sched.run();
  return 0;
}
