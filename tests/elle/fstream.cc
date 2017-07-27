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

BOOST_AUTO_TEST_CASE(rotate)
{
  auto d = elle::filesystem::TemporaryDirectory{};
  auto const family = elle::print("{}/fstream.log", d.path().string());

  // Fill family.
  {
    auto&& o = std::ofstream(elle::print("%s.0", family));
    BOOST_TEST(o.good());
    for (auto _: boost::irange(0, 1000))
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
  BOOST_TEST_MESSAGE("versions: " << elle::rotate_versions(family));
  BOOST_TEST(elle::rotate_versions(family) == (Ints{2, 3}));
}
