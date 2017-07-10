#define BOOST_TEST_MODULE fstream

#include <elle/bytes.hh>
#include <elle/filesystem/TemporaryDirectory.hh>
#include <elle/fstream.hh>
#include <elle/test.hh>

namespace bfs = boost::filesystem;

BOOST_AUTO_TEST_CASE(rotate)
{
  auto d = elle::filesystem::TemporaryDirectory{};
  auto const family = elle::print("{}/fstream.log", d.path().string());

  // Fill family.
  {
    auto&& o = std::ofstream(elle::print("%s.0", family));
    BOOST_TEST(o.good());
    for (int i = 0; i < 1000; ++i)
    {
      // End with a period, rather than a \n, to avoid issues in Windows.
      o << "foo.";
      BOOST_CHECK_NO_THROW(elle::rotate(o, family, 1_KiB, 2));
    }
    BOOST_TEST(o.good());
  }
  // Check it created four files and removed the two oldest.
  using Ints = std::vector<int>;
  BOOST_TEST_MESSAGE("versions: " << elle::rotate_versions(family));
  BOOST_TEST(elle::rotate_versions(family) == (Ints{2, 3}));
}
