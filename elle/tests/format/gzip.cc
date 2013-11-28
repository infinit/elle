#include <sstream>

#include <boost/filesystem.hpp>

#include <elle/format/gzip.hh>
#include <elle/test.hh>

static
void
data_size(std::string content, bool flush, int buffer_size)
{
  std::stringstream buffer;

  {
    elle::format::gzip::Stream filter(buffer, flush, buffer_size);
    filter << content;
  }

  BOOST_CHECK_LE(buffer.str().size(), content.size() * 80 / 100 + 20);

  /*
   elle::Buffer output(content.size());
  {
    boost::iostreams::filtering_istream filter;
    filter.push(boost::iostreams::gzip_decompressor());
    filter.push(buffer);
    filter.read(reinterpret_cast<char*>(output.mutable_contents()),
                output.size());
    BOOST_CHECK_EQUAL(filter.gcount(), content.size());
    char c;
    filter.read(&c, 1);
    BOOST_CHECK_EQUAL(filter.gcount(), 0);
    BOOST_CHECK(filter.eof());
  }
  // Do not use BOOST_CHECK_EQUAL because the output is HUGE.
  BOOST_CHECK(content == output.string());
  */
}

static
std::string
content()
{
  std::stringstream res;
  for (int i = 0; i < 1024; ++i)
    res <<
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam velit"
      "tortor, facilisis eget nisl ac, convallis mattis dui. Donec laoreet purus"
      "at ligula congue, ut gravida felis viverra. Suspendisse potenti. Quisque"
      "elementum lacus ante, ut pellentesque purus tincidunt et. Aliquam"
      "fringilla, augue a consectetur viverra, ante urna pharetra lectus, et"
      "hendrerit purus orci ut lacus. Aliquam erat volutpat. Cras ligula elit,"
      "lobortis euismod pellentesque non, consequat nec sapien. Sed pharetra,"
      "nulla nec hendrerit tempor, odio nisl suscipit sapien, at lacinia dolor"
      "dolor ut lorem. Mauris lacinia ultrices feugiat. Nullam sit amet dolor ut"
      "posuere.";
  return res.str();
}

static
void
data_big_buffer()
{
  return data_size(content(), true, 1 << 18);
}

static
void
data_big_buffer_noflush()
{
  return data_size(content(), false, 1 << 18);
}

static
void
data_small_buffer()
{
  return data_size(content(), true, 1 << 12);
}

static
void
data_small_buffer_noflush()
{
  return data_size(content(), false, 1 << 12);
}

static
void
data_pico_buffer_noflush()
{
  // This checks that with honor_flush = false, data is not encoded by chunks of
  // 8 bytes, this yielding a bigger size.
  return data_size(content(), false, 8);
}

static
void
empty_content()
{
  return data_size("", true, 1024);
}

static
void
empty_content_noflush()
{
  return data_size("", false, 1024);
}

static
void
flush()
{
  std::stringstream buffer;
  {
    elle::format::gzip::Stream filter(buffer, true);
    BOOST_CHECK_EQUAL(buffer.str().size(), 0);
    filter << "a first line";
    filter.flush();
    auto size = buffer.str().size();
    BOOST_CHECK_GT(size, 0);
    filter << "a second line";
    filter.flush();
    BOOST_CHECK_GT(buffer.str().size(), size);
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(content));
  suite.add(BOOST_TEST_CASE(data_big_buffer));
  suite.add(BOOST_TEST_CASE(data_big_buffer_noflush));
  suite.add(BOOST_TEST_CASE(data_small_buffer));
  suite.add(BOOST_TEST_CASE(data_small_buffer_noflush));
  suite.add(BOOST_TEST_CASE(data_pico_buffer_noflush));
  suite.add(BOOST_TEST_CASE(empty_content));
  suite.add(BOOST_TEST_CASE(empty_content_noflush));
  suite.add(BOOST_TEST_CASE(flush));
}
