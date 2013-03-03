
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
    config.create_pipe(ProcessChannelStream::out);
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

  {
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
    ELLE_ASSERT(str == "HELLO GUYS$\n");
    p3.wait();
  }

  {
    ELLE_ASSERT(
      check_output("echo", "bite") == "bite\n"
    );
    ELLE_ASSERT(
      check_output(command("echo", "bite2")) == "bite2\n"
    );

    ELLE_ASSERT(
      check_output(
          command("echo", "bite")
        | command("tr", "beit", "BTIE", pipe_stdout_config)
      ) == "BIET\n"
    );


    ELLE_ASSERT(
      check_output(
          command("echo", "TROLLpifpifpifpifipfipf")
        | command("tr", "pif", "lol")
        | command("tr", "a-z", "A-Z", check_output_config)
      ) == "TROLLLOLLOLLOLLOLOLLOLL\n"
    );

    ELLE_ASSERT(
      check_output(
          command("echo", "bite2")
        | command("tr", "beit", "BTIE")
        | command("tr", "TIEB", "abcd")
      ) == "dbca2\n"
    );
  }

  {
    ELLE_ASSERT(
      execute(
        command("echo", "1"),
        command("echo", "2"),
        command("echo", "3")
      ) == 0
    );
  }

  return 0;
}

