#include <elle/assert.hh>
#include <elle/system/Process.hh>
#include <elle/Exception.hh>

#include <iostream>
#include <fstream>

static
int
_test_main(int ac, char** av)
{
  if (av[0] == std::string{"stdout"})
    std::cout << av[1] << "\n";
  else if (av[0] == std::string{"stderr"})
    std::cerr << av[1] << "\n";
  return 0;
}

int
main(int argc, char** argv)
{
  if (argc > 1 && std::string{argv[1]} == "--")
    return _test_main(argc - 2, &argv[2]);

  using elle::system::Process;
  using elle::system::ProcessChannelStream;
  using elle::system::ProcessConfig;

  // Redirect stdout to a file
  {
    ProcessConfig cfg;
    cfg.pipe_file(ProcessChannelStream::out, "/tmp/bite");
    Process(std::move(cfg), argv[0], {"--", "stdout", "bite"}).wait();
    std::ifstream f{"/tmp/bite"};
    ELLE_ASSERT(f.good());
    std::string str;
    f >> str;
    ELLE_ASSERT(str == "bite");
  }

  // Redirect stderr to a file
  {
    ProcessConfig cfg;
    cfg.pipe_file(ProcessChannelStream::err, "/tmp/bite2");
    Process(std::move(cfg), argv[0], {"--", "stderr", "BIET"}).wait();
    std::ifstream f{"/tmp/bite2"};
    ELLE_ASSERT(f.good());
    std::string str;
    f >> str;
    ELLE_ASSERT(str == "BIET");
  }
  return 0;
}
