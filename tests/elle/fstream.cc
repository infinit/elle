#define BOOST_TEST_MODULE fstream

#include <elle/bytes.hh>
#include <elle/filesystem/TemporaryDirectory.hh>
#include <elle/fstream.hh>
#include <elle/test.hh>

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
      BOOST_CHECK_NO_THROW(elle::rotate(o, family, 1_KiB));
    }
    BOOST_TEST(o.good());
  }
  // Check it created four files.
  for (int i = 0; i < 10; ++i)
  {
    auto f = bfs::path(elle::print("{}.{}", family, i));
    BOOST_TEST_MESSAGE("file: " << f);
    if (i < 4)
    {
      BOOST_TEST(exists(f));
      // The last file is shorter.
      BOOST_TEST(file_size(f) == (i < 3 ? 1028 : 916));
    }
    else
      BOOST_TEST(!exists(f));
  }
}
