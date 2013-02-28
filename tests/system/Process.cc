
#include <elle/system/Process.hh>

#include <unistd.h>
#include <iostream>

using namespace elle::system;


int main()
{
  ProcessConfig config;
  config.pipe_stdout();
  Process p{std::move(config), "echo", {"toto"}};

  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  std::cout << p.read() << std::endl;
  return 0;
}

