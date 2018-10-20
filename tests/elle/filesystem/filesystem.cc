#include <elle/filesystem.hh>

#include <string>

#include <elle/test.hh>

#include <elle/filesystem/TemporaryFile.hh>

using namespace std::string_literals;

static
void
get_native_handle()
{
  auto const data = "file_descriptor"s;
  elle::filesystem::TemporaryFile file("fd");
  {
    elle::fs::fstream s(file.path(), std::ios::out);
    auto fd = elle::fs::get_native_handle(s);
    BOOST_CHECK(write(fd, data.c_str(), data.size()) == signed(data.size()));
  }
  {
    elle::fs::fstream s(file.path(), std::ios::in);
    char buf[128];
    s.read(buf, sizeof(buf));
    BOOST_CHECK(s.gcount() == signed(data.size()));
    buf[data.size()] = 0;
    BOOST_CHECK(data == buf);
  }
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(get_native_handle));
}
