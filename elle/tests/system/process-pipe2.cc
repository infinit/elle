#ifndef DEBUG
# define DEBUG
#endif
#ifdef NDEBUG
# undef NDEBUG
#endif

#include <elle/system/Process.hh>
#include <elle/printf.hh>
#include <unistd.h>
#include <iostream>

int
main()
{
  using namespace elle::system;

  ProcessConfig echo{};
  ProcessConfig cat{};
  ProcessConfig tr{};
  echo.connect_stdout(cat);
  cat.connect_stdout(tr);
  tr.create_pipe(ProcessChannelStream::out);
  Process p{std::move(echo), "echo", {"hello", "guys"}};
  Process p2{std::move(cat), "cat", {"-e"}};
  Process p3{std::move(tr), "tr", {"a-z", "A-Z"}};
  std::string str = p3.read();
  std::cout << str;
  ELLE_ASSERT_EQ(str, "HELLO GUYS$\n");
  p3.wait();
  p2.wait();
  p.wait();
}
