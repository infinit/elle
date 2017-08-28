#define BOOST_TEST_MODULE fstream

#include <elle/bytes.hh>
#include <elle/filesystem/TemporaryDirectory.hh>
#include <elle/filesystem/TemporaryFile.hh>
#include <elle/fstream.hh>
#include <elle/test.hh>

using namespace std::literals;

namespace bfs = boost::filesystem;

BOOST_AUTO_TEST_CASE(content)
{
  auto foo = elle::filesystem::TemporaryFile{"foo"};
  auto const msg = "Top of the world!\n"s;
  std::ofstream{foo.path().string()} << msg;
  BOOST_TEST(elle::content(foo.path()) == msg);
}

BOOST_AUTO_TEST_CASE(ofstream)
{
  // Make sure there is a working default ctor.
  auto&& f = elle::ofstream{};
  BOOST_CHECK(!(f << "foo"));
}

BOOST_AUTO_TEST_CASE(ofstream_rename)
{
  auto foo = elle::filesystem::TemporaryFile{"foo"};
  auto bar = elle::filesystem::TemporaryFile{"bar"};
  // These files already exist, as a side effect of TemporaryFile.
  BOOST_TEST(is_empty(foo.path()));
  BOOST_TEST(is_empty(bar.path()));

  auto&& f = elle::ofstream{foo};
  BOOST_CHECK_EQUAL(f.path(), foo.path());
  BOOST_TEST(is_empty(foo.path()));
  BOOST_TEST(is_empty(bar.path()));
  f << "Hello, ";
  f.flush();
  BOOST_TEST(elle::content(foo) == "Hello, ");
  f.path(bar);
  BOOST_CHECK_EQUAL(f.path(), bar.path());
  f << "world!";
  f.close();
  BOOST_TEST(!exists(foo.path()));
  BOOST_TEST(!is_empty(bar.path()));
  BOOST_TEST(elle::content(bar) == "Hello, world!");
}

BOOST_AUTO_TEST_CASE(rotate)
{
  auto d = elle::filesystem::TemporaryDirectory{};
  auto const family = d.path() / "fstream.log";

  // Fill family.
  {
    auto&& o = elle::ofstream(elle::print("%s.0", family.string()));
    BOOST_TEST(o.good());
    for (int i = 0; i < 1000; ++i)
    {
      // End with a period, rather than a \n, to avoid getting
      // a different size on Windows.
      o << "foo.";
      elle::rotate(o, family, 1_KiB, 2);
    }
    BOOST_TEST(o.good());
  }
  // Check it created four files and removed the two oldest.
  using Ints = std::vector<int>;
  BOOST_CHECK_EQUAL(elle::rotate_versions(family), (Ints{2, 3}));
}
