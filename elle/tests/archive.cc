#include <boost/filesystem/fstream.hpp>

#include <elle/system/Process.hh>
#include <elle/archive/zip.hh>
#include <elle/finally.hh>
#include <elle/test.hh>

class DummyHierarchy
{
public:
  DummyHierarchy()
  {
    auto pattern =
      boost::filesystem::temp_directory_path() / "%%%%-%%%%-%%%%-%%%%";
    this->_root = boost::filesystem::unique_path(pattern) / "root";
    auto root = this->_root;
    auto sub = root / "sub";
    boost::filesystem::create_directories(sub);
    boost::filesystem::ofstream(root / "1") << "1";
    boost::filesystem::ofstream(root / "2") << "2";
    boost::filesystem::ofstream(sub  / "3") << "3";
    boost::filesystem::ofstream(sub  / "4") << "4";
  }

  ~DummyHierarchy()
  {
    boost::filesystem::remove_all(this->_root.parent_path());
  }

  boost::filesystem::path
  root() const
  {
    return this->_root;
  }

private:
  boost::filesystem::path _root;
};

static
void
check_file_content(boost::filesystem::path const& path,
                   char content)
{
  char result[BUFSIZ];
  boost::filesystem::ifstream f(path);
  f.read(result, BUFSIZ);
  BOOST_CHECK_EQUAL(f.gcount(), 1);
  BOOST_CHECK_EQUAL(result[0], content);
}

static
void
zip()
{
  DummyHierarchy dummy;
  auto pattern =
    boost::filesystem::temp_directory_path() / "%%%%-%%%%-%%%%-%%%%.zip";
  auto path = boost::filesystem::unique_path(pattern);
  elle::SafeFinally clean([&] { boost::filesystem::remove(path); });
  elle::archive::zip({dummy.root()}, path);
  {
    auto output_pattern =
      boost::filesystem::temp_directory_path() / "%%%%-%%%%-%%%%-%%%%";
    auto output = boost::filesystem::unique_path(output_pattern);
    boost::filesystem::create_directory(output);
    elle::SafeFinally clean([&] { boost::filesystem::remove_all(output); });
    auto current = boost::filesystem::current_path();
    boost::filesystem::current_path(output);
    elle::SafeFinally go_back(
      [&] { boost::filesystem::current_path(current); });
    elle::system::Process p(
      (current / "libarchive/bin/bsdcpio").native(),
      {"--extract", "--make-directories", "-I", path.native()});
    BOOST_CHECK_EQUAL(p.wait_status(), 0);
    int count = 0;
    for (auto it = boost::filesystem::recursive_directory_iterator(".");
         it != boost::filesystem::recursive_directory_iterator();
         ++it)
    {
      ++count;
      auto path = *it;
      if (path == "./root" || path == "./root/sub")
        continue;
      else if (path == "./root/1")
        check_file_content(path, '1');
      else if (path == "./root/2")
        check_file_content(path, '2');
      else if (path == "./root/sub/3")
        check_file_content(path, '3');
      else if (path == "./root/sub/4")
        check_file_content(path, '4');
      else
        BOOST_FAIL(path);
    }
    BOOST_CHECK_EQUAL(count, 6);
  }
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();

  master.add(BOOST_TEST_CASE(zip));
}
