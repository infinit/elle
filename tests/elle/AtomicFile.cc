#define BOOST_TEST_MODULE AtomicFile

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

#include <elle/AtomicFile.hh>
#include <elle/With.hh>

static boost::filesystem::path const path("atomic-file");
static boost::filesystem::path const path_old(".atomic-file.old");
static boost::filesystem::path const path_new(".atomic-file.new");

static
void
cleanup()
{
  remove(path);
  remove(path_old);
  remove(path_new);
}

BOOST_AUTO_TEST_CASE(nominal)
{
  typedef elle::AtomicFile::Read Read;
  typedef elle::AtomicFile::Write Write;
  static std::string const data1("File me !\n");
  static std::string const data2("File me harder !\n");
  cleanup();
  elle::AtomicFile f(path);
  auto write = [&] (std::string const& data)
    {
      BOOST_CHECK(!f.writing());
      BOOST_CHECK(!f.reading());
      f.write() << [&] (Write& write)
      {
        BOOST_CHECK(f.writing());
        BOOST_CHECK(!f.reading());
        write.stream() << data;
      };
      BOOST_CHECK(!f.writing());
      BOOST_CHECK(!f.reading());
    };
  auto read = [&] (std::string const& data)
    {
      BOOST_CHECK(!f.writing());
      BOOST_CHECK(!f.reading());
      f.read() << [&] (Read& read)
      {
        BOOST_CHECK(!f.writing());
        BOOST_CHECK(f.reading());
        char buf[128];
        read.stream().read(buf, sizeof(buf));
        BOOST_CHECK_EQUAL(read.stream().gcount(), data.size());
        BOOST_CHECK_EQUAL(std::string(buf, data.size()), data);
      };
      BOOST_CHECK(!f.writing());
      BOOST_CHECK(!f.reading());
    };
  BOOST_CHECK(!f.exists());
  write(data1);
  BOOST_CHECK(f.exists());
  read(data1);
  BOOST_CHECK(f.exists());
  write(data2);
  BOOST_CHECK(f.exists());
  read(data2);
  BOOST_CHECK(f.exists());
}

BOOST_AUTO_TEST_CASE(errors)
{
  cleanup();
  elle::AtomicFile f(path);
  BOOST_CHECK_THROW(f.read(), std::exception);
  {
    auto write = f.write();
    BOOST_CHECK_THROW(f.write(), std::exception);
    BOOST_CHECK_THROW(f.read(), std::exception);
  }
  {
    auto read = f.read();
    BOOST_CHECK_THROW(f.write(), std::exception);
    BOOST_CHECK_THROW(f.read(), std::exception);
  }
}

BOOST_AUTO_TEST_CASE(transaction_creation)
{
  cleanup();
  elle::AtomicFile f(path);
  BOOST_CHECK_THROW(
    f.write() << [&]
    {
      // Check the not yet commited initial content gets discarded.
      elle::AtomicFile check(path);
      BOOST_CHECK(!check.exists());
    },
    // Check the initial write fails.
    std::exception);
}

BOOST_AUTO_TEST_CASE(transaction_write)
{
  cleanup();
  elle::AtomicFile f(path);
  {
    f.write() << [] (elle::AtomicFile::Write& write)
    {
      write.stream() << "old\n";
    };
  }
  {
    BOOST_CHECK_THROW(
      f.write() << [] (elle::AtomicFile::Write& write)
      {
        write.stream() << "new\n";
        // Check the not yet commited content gets discarded.
        {
          elle::AtomicFile check(path);
          BOOST_CHECK(check.exists());
          check.read() << [] (elle::AtomicFile::Read& read)
          {
            char buf[128];
            read.stream().read(buf, sizeof(buf));
            BOOST_CHECK_EQUAL(read.stream().gcount(), 4);
            BOOST_CHECK_EQUAL(std::string(buf, 4), "old\n");
          };
        }
      },
      // Check the initial write fails.
      std::exception);
  }
}
