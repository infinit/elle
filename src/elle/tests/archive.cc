#include <unordered_set>

#include <boost/filesystem.hpp>

#include <elle/archive/zip.hh>
#include <elle/attribute.hh>
#include <elle/filesystem.hh>
#include <elle/filesystem/TemporaryDirectory.hh>
#include <elle/filesystem/TemporaryFile.hh>
#include <elle/finally.hh>
#include <elle/Error.hh>
#include <elle/os/environ.hh>
#include <elle/printf.hh>
#include <elle/system/Process.hh>
#include <elle/test.hh>

#ifndef INFINIT_WINDOWS
# define EXTENSION ""
#else
# define EXTENSION ".exe"
#endif

using elle::filesystem::TemporaryDirectory;
using elle::filesystem::TemporaryFile;

static const std::string ROOT("root");
static const std::string SUB("sub");

class DummyHierarchy
{
public:
  DummyHierarchy()
  {
    auto pattern =
      boost::filesystem::temp_directory_path() / "%%%%-%%%%-%%%%-%%%%";
    this->_root = boost::filesystem::unique_path(pattern) / ROOT;
    auto root = this->_root;
    auto sub = root / SUB;
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

class LargerTemporaryFile
{
public:
  LargerTemporaryFile(std::string const& name, std::string const& content)
    : _directory(name)
  {
    this->_path = this->_directory.path() / name;
    boost::filesystem::ofstream f(this->_path);
    f << content;
    f.close();
  }
private:
  ELLE_ATTRIBUTE_R(TemporaryDirectory, directory);
  ELLE_ATTRIBUTE_R(boost::filesystem::path, path);
};

class ChangeDirectory
{
public:
  ChangeDirectory(boost::filesystem::path const& path)
    : _path(path)
    , _previous(boost::filesystem::current_path())
  {
    boost::filesystem::current_path(path);
  }

  ~ChangeDirectory()
  {
    boost::filesystem::current_path(this->_previous);
  }

private:
  ELLE_ATTRIBUTE_R(boost::filesystem::path, path);
  ELLE_ATTRIBUTE_R(boost::filesystem::path, previous);
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
check_file_content(boost::filesystem::path const& path,
                   std::string const& content)
{
  boost::filesystem::ifstream f(path);
  char file_content[content.size()];
  f.read(file_content, content.size());
  BOOST_CHECK_EQUAL(f.gcount(), content.size());
  BOOST_CHECK_EQUAL(std::string(file_content, content.size()), content);
}

static
boost::filesystem::path
renamer_forbid(boost::filesystem::path const& input)
{
  BOOST_FAIL("no file renaming needed");
  return input;
}

static
void
extract(elle::archive::Format fmt,
        boost::filesystem::path const& path,
        boost::optional<boost::filesystem::path> const& where = boost::none,
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
  auto root =
    boost::filesystem::unique_path(
      boost::filesystem::temp_directory_path() / "%%%%-%%%%-%%%%-%%%%");
  boost::filesystem::create_directories(root);
  elle::SafeFinally clean_root([&] { boost::filesystem::remove_all(root); });
  auto pattern = root / "%%%%-%%%%-%%%%-%%%%.zip";
  auto path = boost::filesystem::unique_path(pattern);
  elle::SafeFinally clean([&] { boost::filesystem::remove(path); });
  {
    DummyHierarchy dummy;
    elle::archive::archive(fmt, {dummy.root()}, path, renamer_forbid);
  }
  {
    auto output_pattern =
      boost::filesystem::temp_directory_path() / "%%%%-%%%%-%%%%-%%%%";
    auto output = boost::filesystem::unique_path(output_pattern);
    boost::filesystem::create_directory(output);
    elle::SafeFinally clean([&] { boost::filesystem::remove_all(output); });
    bool change_directory_beacon = false;
    bool extract_and_delete_beacon = false;
    auto extraction = [&] (bool use_libarchive_binaries,
                           boost::optional<boost::filesystem::path> _output)
      {
        int count = 0;
        boost::filesystem::path output;
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
          boost::filesystem::remove(path);
        }
        ChangeDirectory cd(output);
        bool beacon1 = false;
        bool beacon2 = false;
        bool beacon3 = false;
        bool beacon4 = false;
        for (auto it = boost::filesystem::recursive_directory_iterator(".");
             it != boost::filesystem::recursive_directory_iterator();
             ++it)
        {
          ++count;
          auto path = *it;
          if (path == elle::sprintf("./%s", ROOT) ||
              path == elle::sprintf("./%s/%s", ROOT, SUB))
            continue;
          else if (path == elle::sprintf("./%s/1", ROOT))
          {
            beacon1 = true;
            check_file_content(path, '1');
          }
          else if (path == elle::sprintf("./%s/2", ROOT))
          {
            beacon2 = true;
            check_file_content(path, '2');
          }
          else if (path == elle::sprintf("./%s/%s/3", ROOT, SUB))
          {
            beacon3 = true;
            check_file_content(path, '3');
          }
          else if (path == elle::sprintf("./%s/%s/4", ROOT, SUB))
          {
            beacon4 = true;
            check_file_content(path, '4');
          }
          else
          {
            BOOST_FAIL(path);
          }
        }
        BOOST_CHECK_EQUAL(count, 6);
        BOOST_CHECK(beacon1 && beacon2 && beacon3 && beacon4);
      };
    extraction(true, output);
    extraction(false, output);
    extraction(false, boost::none);
    ELLE_ASSERT(change_directory_beacon && extract_and_delete_beacon);
  }
}

static
void
larger_archive(elle::archive::Format fmt)
{
  std::string content;
  for (int i = 0; i < 10000; i++)
  {
    content += "stuffs n ";
  }
  LargerTemporaryFile f1("file1", content);
  LargerTemporaryFile f2("file2", content);
  TemporaryDirectory output("output");
  auto archive_path = output.path() / "output.zip";
  elle::archive::archive(fmt, {f1.path(), f2.path()}, archive_path);
  {
    TemporaryDirectory decompress("decompress");
    extract(fmt, archive_path, decompress.path());
    ChangeDirectory cd(decompress.path());
    int count = 0;
    for (auto it = boost::filesystem::recursive_directory_iterator(".");
         it != boost::filesystem::recursive_directory_iterator();
         ++it)
    {
      count++;
      auto path = *it;
      check_file_content(path, content);
    }
    BOOST_CHECK_EQUAL(count, 2);
  }
}

static
boost::filesystem::path
renamer(boost::filesystem::path const& input)
{
  return input.string() + " bis";
}

static
void
archive_duplicate(elle::archive::Format fmt)
{
  TemporaryDirectory d1("same");
  boost::filesystem::ofstream(d1.path() / "beacon");
  TemporaryDirectory d2("same");
  boost::filesystem::ofstream(d2.path() / "beacon");
  TemporaryFile f("same");
  TemporaryDirectory output("output");
  auto path = output.path() / "output.zip";
  elle::archive::archive(fmt,
                         {d1.path(), d2.path(), f.path()},
                         path,
                         renamer);
  {
    TemporaryDirectory decompress("decompress");
    extract(fmt, path, decompress.path());
    int count = 0;
    std::unordered_set<boost::filesystem::path> accepted({
        "./same",
        "./same/beacon",
        "./same bis",
        "./same bis/beacon",
        "./same bis bis",
          });
    ChangeDirectory cd(decompress.path());
    for (auto it = boost::filesystem::recursive_directory_iterator(".");
         it != boost::filesystem::recursive_directory_iterator();
         ++it)
    {
      ++count;
      if (accepted.find(*it) == accepted.end())
        BOOST_FAIL(*it);
    }
    BOOST_CHECK_EQUAL(count, accepted.size());
  }
}

static
void
archive_symlink(elle::archive::Format fmt)
{
  #ifdef INFINIT_WINDOWS
  return;
  #else
  TemporaryDirectory d1("input");
  boost::filesystem::create_directories(d1.path() / "files");
  boost::filesystem::ofstream(d1.path() / "files" / "data") << "data";
  boost::filesystem::ofstream(d1.path() / "outside-data") << "outside-data";
  boost::filesystem::create_symlink("data", (d1.path() / "files" / "data-link"));
  boost::filesystem::create_symlink("nosuchfile", (d1.path() / "files" / "invalid-link"));
  boost::filesystem::create_symlink("../outside-data", (d1.path() / "files" / "external-link"));
  TemporaryDirectory d2("output");
  auto path = d2.path() / "output.zip";
  elle::archive::archive(fmt,
                         {d1.path() / "files"},
                         path
                         );
  TemporaryDirectory decompress("decompress");
  extract(fmt, path, decompress.path());
  BOOST_CHECK_EQUAL(boost::filesystem::read_symlink(decompress.path() / "files" / "data-link"),
                    boost::filesystem::path("data"));
  BOOST_CHECK_EQUAL(boost::filesystem::read_symlink(decompress.path() / "files" / "external-link"),
                    boost::filesystem::path("..") / "outside-data");
  BOOST_CHECK_EQUAL(boost::filesystem::read_symlink(decompress.path() / "files" / "invalid-link"),
                    boost::filesystem::path("nosuchfile"));
  boost::filesystem::ifstream input(decompress.path() / "files" / "data-link",
                                    std::ios_base::in | std::ios_base::binary);
  BOOST_CHECK(input.good());
  char buffer[BUFSIZ];
  input.read(buffer, BUFSIZ);
  BOOST_CHECK_EQUAL(input.gcount(), 4);
  buffer[4] = 0;
  BOOST_CHECK_EQUAL(buffer, "data");
  #endif // INFINIT_WINDOWS
}

static
void
archiving_error(elle::archive::Format fmt)
{
  TemporaryDirectory d1("input");
  TemporaryDirectory d2("output");
  boost::filesystem::path doesnt_exist = d1.path() / "this path doesn't exists";
  boost::filesystem::path exists(d1.path() / "data");
  boost::filesystem::ofstream(exists) << "data";
  BOOST_CHECK(boost::filesystem::exists(exists));
  {
    auto path = d2.path() / "output.zip";
    BOOST_CHECK_THROW(elle::archive::archive(fmt, {doesnt_exist}, path),
                      elle::Error);
    // Should have been deleted.
    BOOST_CHECK(boost::filesystem::exists(path));
    TemporaryDirectory decompress("decompress");
    extract(fmt, path, decompress.path());
    BOOST_CHECK(boost::filesystem::is_empty(decompress.path()));
  }
  {
    auto path = d2.path() / "output2.zip";
    elle::archive::archive(
      fmt, {doesnt_exist, exists}, path, elle::archive::Renamer{}, elle::archive::Excluder{}, true);
    TemporaryDirectory decompress("decompress2");
    extract(fmt, path, decompress.path());
    BOOST_CHECK(!boost::filesystem::is_empty(decompress.path()));
    BOOST_CHECK(boost::filesystem::exists(decompress.path() / "data"));
  }
}

#define FORMAT(Fmt)                                     \
  namespace Fmt                                         \
  {                                                     \
    static                                              \
    void                                                \
    simple()                                            \
    {                                                   \
      archive(elle::archive::Format::Fmt);              \
    }                                                   \
                                                        \
    static                                              \
    void                                                \
    less_simple()                                       \
    {                                                   \
      larger_archive(elle::archive::Format::Fmt);       \
    }                                                   \
                                                        \
    static                                              \
    void                                                \
    duplicate()                                         \
    {                                                   \
      archive_duplicate(elle::archive::Format::Fmt);    \
    }                                                   \
    static                                              \
    void                                                \
    symboliclink()                                      \
    {                                                   \
      archive_symlink(elle::archive::Format::Fmt);      \
    }                                                   \
                                                        \
    static                                              \
    void                                                \
    error()                                             \
    {                                                   \
      archiving_error(elle::archive::Format::Fmt);      \
    }                                                   \
  }                                                     \

FORMAT(zip)
FORMAT(tar)
FORMAT(tar_gzip)
#undef FORMAT

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  // Under libc.musl, lchmod() call fails with a ENOTSUPP which causes
  // a libarchive error when decompressing a symlink.
  bool musl = boost::filesystem::exists("/lib/libc.musl-x86_64.so.1");
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
