
#include <cstdio>
#include <iostream>

#include "Application.hh"

#define BUF_SIZE 4096

int     main(int ac, char* av[])
{

  //auto buf = new char[BUF_SIZE];
  //size_t sz = 0;
  //do {
  //    ssize_t readBytes = ::read(STDIN_FILENO, buf + sz, BUF_SIZE - sz);
  //    if (readBytes <= 0)
  //      break;
  //    sz += static_cast<size_t>(readBytes);
  //    if (sz >= BUF_SIZE)
  //      {
  //        std::cerr << "Buffer cannot handle that much\n";
  //        return EXIT_FAILURE;
  //      }
  //} while (true);

  ////::close(STDIN_FILENO);

  //size_t idx = 0;
  //while (idx < sz && buf[idx] != ' ')
  //  ++idx;
  //if (idx >= sz - 1)
  //  {
  //    std::cerr << "Not enough data to continue\n";
  //    return EXIT_FAILURE;
  //  }
  //std::string token(buf, idx);
  //idx += 1;
  //std::string identity(buf + idx, sz - idx);
  //delete [] buf;
  //std::cout << "Started with token=" << token
  //          << " identity=" << identity << std::endl;

  plasma::watchdog::Application app(ac, av);
  try
    {
      return app.Exec();
    }
  catch (std::exception const& err)
    {
      std::cout << "An exception occured: " << err.what() << std::endl;
    }
  catch (...)
    {
      std::cout << "Uncatched exception\n";
    }
  return EXIT_FAILURE;
}
