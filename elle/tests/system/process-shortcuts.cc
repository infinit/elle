#include <elle/assert.hh>
#include <elle/system/Process.hh>

#include <unistd.h>
#include <iostream>

int
main()
{
  using namespace elle::system;

  ELLE_ENFORCE(
    check_output("echo", "bite") == "bite\n"
  );
  ELLE_ENFORCE(
    check_output(command("echo", "bite2")) == "bite2\n"
  );

  ELLE_ENFORCE(
    check_output(
        command("echo", "bite")
      | command("tr", "beit", "BTIE", pipe_stdout_config)
    ) == "BIET\n"
  );


  ELLE_ENFORCE(
    check_output(
        command("echo", "TROLLpifpifpifpifipfipf")
      | command("tr", "pif", "lol")
      | command("tr", "a-z", "A-Z", check_output_config)
    ) == "TROLLLOLLOLLOLLOLOLLOLL\n"
  );

  ELLE_ENFORCE(
    check_output(
        command("echo", "bite2")
      | command("tr", "beit", "BTIE")
      | command("tr", "TIEB", "abcd")
    ) == "dbca2\n"
  );

  ELLE_ENFORCE(
    execute(
      command("echo", "1"),
      command("echo", "2"),
      command("echo", "3")
    ) == 0
  );
}
