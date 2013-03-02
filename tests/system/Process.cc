
#include <elle/system/Process.hh>
#include <elle/assert.hh>

#include <unistd.h>
#include <iostream>

using namespace elle::system;


int main()
{
  {
    Process p{check_output_config, "echo", {"toto"}};
    ELLE_ASSERT(p.read() == "toto\n");
    p.wait();
  }

  {
    ProcessConfig config;
    config.create_pipe(ProcessChannel::out);
    Process p{std::move(config), "echo", {"toto"}};
    ELLE_ASSERT(p.read() == "toto\n");
    p.wait();
  }

  {
    Process p{check_output_config, "sh", {"-c", "echo toto"}};
    std::cout << p.read();
    p.wait();
  }

  std::cout << check_output("cat", "/etc/passwd") ;
  std::cout << check_output("cat", "-e", "/etc/passwd") ;
  std::cout << check_output("sh", "-c", "env") ;
  std::cout << check_output("env") ;

  return 0;
}

