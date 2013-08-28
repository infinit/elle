#include <reactor/fs/File.hh>

#include <elle/Buffer.hh>

#define BOOST_TEST_MODULE file
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <iostream>

namespace
{
  struct TmpFile
  {
    char* name;
    TmpFile()
      : name{mktemp(strdup("test_file.XXXXXX"))}
    {
      BOOST_CHECK(this->name != nullptr);
    }
    ~TmpFile()
    {
      (void)::unlink(this->name);
      ::free(this->name);
    }
  };
}

BOOST_AUTO_TEST_CASE(simple)
{
  reactor::Scheduler sched;
  reactor::Thread task{sched, "task", [&] {
    TmpFile tmp;
    {
      reactor::fs::File f{sched, tmp.name};
      f.write_some(elle::ConstWeakBuffer{"pif", 3});
    }
    {
      std::string str;
      std::ifstream{tmp.name} >> str;
      BOOST_CHECK_EQUAL(str, "pif");
    }
    {
      reactor::fs::File f{sched, tmp.name};
      elle::Buffer buf{30};
      f.read_some(buf);
      BOOST_CHECK_EQUAL(buf.size(), 3);
      std::string str{(char*)buf.contents(), 3};
      BOOST_CHECK_EQUAL(str, "pif");
    }
  }};
  sched.run();
}

BOOST_AUTO_TEST_CASE(bigfile)
{
  reactor::Scheduler sched;
  bool done = false;
  size_t const write_count = 500;
  reactor::Thread grow_file{sched, "grow", [&] {
    TmpFile tmp;
    std::string data(5000000, '@');
    std::cerr << data.size() << std::endl;
    for (size_t i = 0; i < write_count; i++)
    {
      reactor::fs::File f{sched, tmp.name};
      f.write_some(elle::ConstWeakBuffer{data.c_str(), data.size()});
    }
    done = true;
  }};

  size_t idle_count = 0;
  reactor::Thread idle{sched, "idle", [&] {
    while (!done)
    {
      idle_count++;
      sched.current()->yield();
    }
  }};
  sched.run();
  BOOST_CHECK(idle_count > write_count);
}


