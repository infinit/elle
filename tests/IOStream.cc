#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <elle/IOStream.hh>

class ReadStreamBuffer: public elle::StreamBuffer
{
  public:
    ReadStreamBuffer(std::string const& data, int read_size)
      : _read_data(data)
      , _read_pos(0)
      , _read_size(read_size)
    {}

    virtual
    elle::WeakBuffer
    write_buffer()
    {}

    virtual
    void
    flush(elle::Size)
    {}

    virtual
    elle::WeakBuffer
    read_buffer()
    {
      if (_read_pos == _read_data.size())
        return elle::WeakBuffer(nullptr, 0);
      int pos = _read_pos;
      auto size = std::min(_read_size, int(_read_data.size() - pos));
      _read_pos += size;
      return elle::WeakBuffer(const_cast<char*>(_read_data.c_str()) + pos,
                              size);
    }
    std::string _read_data;
    int _read_pos;
    int _read_size;
};

static
void
test_stream_buffer(int read_size)
{
  std::string content
    ("Three Rings for the Elven-kings under the sky,"
     "Seven for the Dwarf-lords in their halls of stone,"
     "Nine for Mortal Men doomed to die,"
     "One for the Dark Lord on his dark throne"
     "In the Land of Mordor where the Shadows lie."
     "One Ring to rule them all, One Ring to find them,"
     "One Ring to bring them all and in the darkness bind them"
     "In the Land of Mordor where the Shadows lie.");

  auto buffer = new ReadStreamBuffer(content, read_size);
  elle::IOStream stream(buffer);

  std::stringstream ref(content);

  while (!stream.eof())
  {
    BOOST_CHECK(!ref.eof());
    std::string word;
    std::string ref_word;
    stream >> word;
    ref >> ref_word;
    BOOST_CHECK_EQUAL(word, ref_word);
  }
  BOOST_CHECK(ref.eof());
}

static
bool
test_suite()
{
  boost::unit_test::test_suite* stream_buffer = BOOST_TEST_SUITE("StreamBuffer");
  boost::unit_test::framework::master_test_suite().add(stream_buffer);
  stream_buffer->add(BOOST_TEST_CASE(std::bind(test_stream_buffer, 1)));
  stream_buffer->add(BOOST_TEST_CASE(std::bind(test_stream_buffer, 2)));
  stream_buffer->add(BOOST_TEST_CASE(std::bind(test_stream_buffer, 3)));
  stream_buffer->add(BOOST_TEST_CASE(std::bind(test_stream_buffer, 4)));
  stream_buffer->add(BOOST_TEST_CASE(std::bind(test_stream_buffer, 5)));
  stream_buffer->add(BOOST_TEST_CASE(std::bind(test_stream_buffer, 6)));
  stream_buffer->add(BOOST_TEST_CASE(std::bind(test_stream_buffer, 7)));
  stream_buffer->add(BOOST_TEST_CASE(std::bind(test_stream_buffer, 8)));

  return true;
}

int
main(int argc, char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
