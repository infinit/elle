#include <unordered_set>

#include <boost/filesystem.hpp>

#include <elle/archive/zip.hh>
#include <elle/attribute.hh>
#include <elle/filesystem.hh>
#include <elle/filesystem/TemporaryDirectory.hh>
#include <elle/filesystem/TemporaryFile.hh>
#include <elle/Error.hh>
#include <elle/os/environ.hh>
#include <elle/printf.hh>
#include <elle/system/Process.hh>
#include <elle/test.hh>

#ifdef ELLE_WINDOWS
# define EXTENSION ".exe"
#else
# define EXTENSION ""
#endif

using namespace std::literals;
namespace bfs = boost::filesystem;

using elle::filesystem::TemporaryDirectory;
using elle::filesystem::TemporaryFile;

namespace
{
  auto const ROOT = "root"s;
  auto const SUB = "sub"s;
}

class DummyHierarchy
{
public:
  DummyHierarchy()
  {
    auto const pattern = bfs::temp_directory_path() / "%%%%-%%%%-%%%%-%%%%";
    this->_root = bfs::unique_path(pattern) / ROOT;
    auto const root = this->_root;
    auto const sub = root / SUB;
    bfs::create_directories(sub);
    bfs::ofstream(root / "1") << "1";
    bfs::ofstream(root / "2") << "2";
    bfs::ofstream(sub  / "3") << "3";
    bfs::ofstream(sub  / "4") << "4";
  }

  ~DummyHierarchy()
  {
    bfs::remove_all(this->_root.parent_path());
  }

  bfs::path
  root() const
  {
    return this->_root;
  }

private:
  bfs::path _root;
};

class LargerTemporaryFile
{
public:
  LargerTemporaryFile(std::string const& name, std::string const& content)
    : _directory(name)
    , _path{this->_directory.path() / name}
  {
    bfs::ofstream f(this->_path);
    f << content;
    f.close();
  }
private:
  ELLE_ATTRIBUTE_R(TemporaryDirectory, directory);
  ELLE_ATTRIBUTE_R(bfs::path, path);
};

/// RAII for changing directory momentarily.
class ChangeDirectory
{
public:
  ChangeDirectory(bfs::path const& path)
    : _path(path)
    , _previous(bfs::current_path())
  {
    bfs::current_path(path);
  }

  ~ChangeDirectory()
  {
    bfs::current_path(this->_previous);
  }

private:
  ELLE_ATTRIBUTE_R(bfs::path, path);
  ELLE_ATTRIBUTE_R(bfs::path, previous);
};

static
void
check_file_content(bfs::path const& path,
                   std::string const& content)
{
  auto&& f = bfs::ifstream(path);
  char file_content[content.size()];
  f.read(file_content, content.size());
  BOOST_CHECK_EQUAL(f.gcount(), content.size());
  BOOST_CHECK_EQUAL(std::string(file_content, content.size()), content);
}

static
bfs::path
renamer_forbid(bfs::path const& input)
{
  BOOST_FAIL("no file renaming needed");
  return input;
}

static
void
extract(elle::archive::Format fmt,
        bfs::path const& path,
        boost::optional<bfs::path> const& where = boost::none,
        bool use_libarchive_binaries = false)
{
  if (where)
  {
    ChangeDirectory cd(where.get());
    if (use_libarchive_binaries)
    {
      switch (fmt)
      {
        case elle::archive::Format::tar:
        case elle::archive::Format::tar_bzip2:
        case elle::archive::Format::tar_gzip:
        {
          elle::system::Process p({
              (cd.previous() /
               elle::os::getenv("BUILD_DIR")
               / "../../libarchive/bin/bsdtar" EXTENSION).string(),
              "-x", "-f", path.string()});
          BOOST_CHECK_EQUAL(p.wait(), 0);
          break;
        }
        case elle::archive::Format::zip:
        case elle::archive::Format::zip_uncompressed:
        {
          elle::system::Process p({
              (cd.previous() /
               elle::os::getenv("BUILD_DIR") /
               "../../libarchive/bin/bsdcpio" EXTENSION).string(),
                "--extract", "--make-directories", "-I", path.string()});
          BOOST_CHECK_EQUAL(p.wait(), 0);
          break;
        }
      }
    }
    else
      elle::archive::extract(path, cd.path());
  }
  else
  {
    ELLE_ASSERT(use_libarchive_binaries == false);
    elle::archive::extract(path);
  }
}

static
void
archive(elle::archive::Format fmt)
{
  auto const root = TemporaryDirectory{};
  auto const pattern = root.path() / "%%%%-%%%%-%%%%-%%%%.zip";
  auto const path = bfs::unique_path(pattern);
  {
    DummyHierarchy dummy;
    elle::archive::archive(fmt, {dummy.root()}, path, renamer_forbid);
  }
  {
    auto const output = TemporaryDirectory{};
    bool change_directory_beacon = false;
    bool extract_and_delete_beacon = false;
    auto extraction = [&] (bool use_libarchive_binaries,
                           boost::optional<bfs::path> _output)
      {
        int count = 0;
        bfs::path output;
        if (_output)
        {
          change_directory_beacon = true;
          output = _output.get();
          extract(fmt, path, output, use_libarchive_binaries);
        }
        else
        {
          extract_and_delete_beacon = true;
          output = path.parent_path();
          extract(fmt, path);
          bfs::remove(path);
        }
        ChangeDirectory cd(output);
        bool beacon1 = false;
        bool beacon2 = false;
        bool beacon3 = false;
        bool beacon4 = false;
        for (auto path: bfs::recursive_directory_iterator("."))
        {
          ++count;
          if (path == elle::sprintf("./%s", ROOT) ||
              path == elle::sprintf("./%s/%s", ROOT, SUB))
            continue;
          else if (path == elle::sprintf("./%s/1", ROOT))
          {
            beacon1 = true;
            check_file_content(path, "1");
          }
          else if (path == elle::sprintf("./%s/2", ROOT))
          {
            beacon2 = true;
            check_file_content(path, "2");
          }
          else if (path == elle::sprintf("./%s/%s/3", ROOT, SUB))
          {
            beacon3 = true;
            check_file_content(path, "3");
          }
          else if (path == elle::sprintf("./%s/%s/4", ROOT, SUB))
          {
            beacon4 = true;
            check_file_content(path, "4");
          }
          else
          {
            BOOST_FAIL(path);
          }
        }
        BOOST_CHECK_EQUAL(count, 6);
        BOOST_CHECK(beacon1 && beacon2 && beacon3 && beacon4);
      };
    extraction(true, output.path());
    extraction(false, output.path());
    extraction(false, boost::none);
    BOOST_REQUIRE(change_directory_beacon && extract_and_delete_beacon);
  }
}

static
void
larger_archive(elle::archive::Format fmt)
{
  auto content = std::string{};
  for (int i = 0; i < 10000; i++)
    content += "stuffs n ";
  auto const f1 = LargerTemporaryFile("file1", content);
  auto const f2 = LargerTemporaryFile("file2", content);
  auto const output = TemporaryDirectory("output");
  auto const archive_path = output.path() / "output.zip";
  elle::archive::archive(fmt, {f1.path(), f2.path()}, archive_path);
  {
    auto const decompress = TemporaryDirectory("decompress");
    extract(fmt, archive_path, decompress.path());
    auto const cd = ChangeDirectory(decompress.path());
    int count = 0;
    for (auto p: bfs::recursive_directory_iterator("."))
    {
      ++count;
      check_file_content(p, content);
    }
    BOOST_CHECK_EQUAL(count, 2);
  }
}

static
bfs::path
renamer(bfs::path const& input)
{
  return input.string() + " bis";
}

static
void
archive_duplicate(elle::archive::Format fmt)
{
  auto const d1 = TemporaryDirectory("same");
  bfs::ofstream(d1.path() / "beacon");
  auto const d2 = TemporaryDirectory("same");
  bfs::ofstream(d2.path() / "beacon");
  auto const f = TemporaryFile("same");
  auto const output = TemporaryDirectory("output");
  auto const path = output.path() / "output.zip";
  elle::archive::archive(fmt,
                         {d1.path(), d2.path(), f.path()},
                         path,
                         renamer);
  {
    auto const decompress = TemporaryDirectory("decompress");
    extract(fmt, path, decompress.path());
    int count = 0;
    auto const accepted = std::unordered_set<bfs::path>{
      "./same",
      "./same/beacon",
      "./same bis",
      "./same bis/beacon",
      "./same bis bis",
    };
    auto const cd = ChangeDirectory(decompress.path());
    for (auto p: bfs::recursive_directory_iterator("."))
    {
      ++count;
      BOOST_TEST(contains(accepted, p));
    }
    BOOST_CHECK_EQUAL(count, accepted.size());
  }
}

static
void
archive_symlink(elle::archive::Format fmt)
{
#if ! defined ELLE_WINDOWS
  auto const d1 = TemporaryDirectory("input");
  bfs::create_directories(d1.path() / "files");
  bfs::ofstream(d1.path() / "files" / "data") << "data";
  bfs::ofstream(d1.path() / "outside-data") << "outside-data";
  bfs::create_symlink("data", (d1.path() / "files" / "data-link"));
  bfs::create_symlink("nosuchfile", (d1.path() / "files" / "invalid-link"));
  bfs::create_symlink("../outside-data", (d1.path() / "files" / "external-link"));
  auto const d2 = TemporaryDirectory("output");
  auto const path = d2.path() / "output.zip";
  elle::archive::archive(fmt, {d1.path() / "files"}, path);
  auto const decompress = TemporaryDirectory("decompress");
  extract(fmt, path, decompress.path());
  BOOST_CHECK_EQUAL(bfs::read_symlink(decompress.path() / "files" / "data-link"),
                    bfs::path("data"));
  BOOST_CHECK_EQUAL(bfs::read_symlink(decompress.path() / "files" / "external-link"),
                    bfs::path("..") / "outside-data");
  BOOST_CHECK_EQUAL(bfs::read_symlink(decompress.path() / "files" / "invalid-link"),
                    bfs::path("nosuchfile"));
  bfs::ifstream input(decompress.path() / "files" / "data-link",
                      std::ios_base::in | std::ios_base::binary);
  BOOST_CHECK(input.good());
  char buffer[BUFSIZ];
  input.read(buffer, BUFSIZ);
  BOOST_CHECK_EQUAL(input.gcount(), 4);
  buffer[4] = 0;
  BOOST_CHECK_EQUAL(buffer, "data");
#endif // ELLE_WINDOWS
}

static
void
archiving_error(elle::archive::Format fmt)
{
  auto const d1 = TemporaryDirectory("input");
  auto const d2 = TemporaryDirectory("output");
  auto const doesnt_exist = d1.path() / "this path doesn't exists";
  auto const exists = d1.path() / "data";
  bfs::ofstream(exists) << "data";
  BOOST_CHECK(bfs::exists(exists));
  {
    auto const path = d2.path() / "output.zip";
    BOOST_CHECK_THROW(elle::archive::archive(fmt, {doesnt_exist}, path),
                      elle::Error);
    // Should have been deleted.
    BOOST_CHECK(bfs::exists(path));
    auto const decompress = TemporaryDirectory("decompress");
    extract(fmt, path, decompress.path());
    BOOST_CHECK(bfs::is_empty(decompress.path()));
  }
  {
    auto const path = d2.path() / "output2.zip";
    elle::archive::archive(
      fmt, {doesnt_exist, exists}, path,
      {}, {},
      true);
    auto const decompress = TemporaryDirectory("decompress2");
    extract(fmt, path, decompress.path());
    BOOST_CHECK(!bfs::is_empty(decompress.path()));
    BOOST_CHECK(bfs::exists(decompress.path() / "data"));
  }
}

#define FORMAT(Fmt)                                     \
  namespace Fmt                                         \
  {                                                     \
    namespace                                           \
    {                                                   \
      auto const fmt = elle::archive::Format::Fmt;      \
      void simple()       { archive(fmt); }             \
      void less_simple()  { larger_archive(fmt); }      \
      void duplicate()    { archive_duplicate(fmt); }   \
      void symboliclink() { archive_symlink(fmt); }     \
      void error()        { archiving_error(fmt); }     \
    }                                                   \
  }

FORMAT(zip)
FORMAT(tar)
FORMAT(tar_gzip)
#undef FORMAT

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  // Under libc.musl, lchmod() call fails with a ENOTSUPP which causes
  // a libarchive error when decompressing a symlink.
  bool musl = bfs::exists("/lib/libc.musl-x86_64.so.1");
#define FORMAT(Fmt)                             \
  {                                             \
    using namespace Fmt;                        \
    auto suite = BOOST_TEST_SUITE(#Fmt);        \
    master.add(suite);                          \
    suite->add(BOOST_TEST_CASE(simple));        \
    suite->add(BOOST_TEST_CASE(less_simple));   \
    suite->add(BOOST_TEST_CASE(duplicate));     \
    if (!musl)                                  \
      suite->add(BOOST_TEST_CASE(symboliclink));\
    suite->add(BOOST_TEST_CASE(error));         \
  }                                             \

  FORMAT(zip);
  FORMAT(tar);
  FORMAT(tar_gzip);

#undef FORMAT
}
