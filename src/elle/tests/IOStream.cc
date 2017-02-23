#include <elle/test.hh>
#include <elle/Buffer.hh>
#include <elle/IOStream.hh>

static
std::string content ("Three Rings for the Elven-kings under the sky, "
                     "Seven for the Dwarf-lords in their halls of stone, "
                     "Nine for Mortal Men doomed to die, "
                     "One for the Dark Lord on his dark throne "
                     "In the Land of Mordor where the Shadows lie. "
                     "One Ring to rule them all, One Ring to find them, "
                     "One Ring to bring them all and in the darkness bind them "
                     "In the Land of Mordor where the Shadows lie.");

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
    {
      return elle::WeakBuffer(nullptr, 0);
    }

    virtual
    void
    flush(Size)
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
    unsigned int _read_pos;
    int _read_size;
};

static
void
test_stream_buffer_read(int read_size)
{
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

class WriteStreamBuffer: public elle::StreamBuffer
{
  public:
    WriteStreamBuffer(std::string& data, int write_size)
      : _write_buffer(new char[write_size])
      , _write_data(data)
      , _write_size(write_size)
    {}

    ~WriteStreamBuffer()
    {
      delete [] _write_buffer;
    }

    virtual
    elle::WeakBuffer
    write_buffer()
    {
      return elle::WeakBuffer(_write_buffer, _write_size);
    }
    char* _write_buffer;
    std::string& _write_data;
    int _write_size;

    virtual
    void
    flush(StreamBuffer::Size size)
    {
      _write_data += std::string(_write_buffer, size);
    }

    virtual
    elle::WeakBuffer
    read_buffer()
    {
      return elle::WeakBuffer(nullptr, 0);
    }
};

static
void
test_stream_buffer_write(int write_size)
{
  std::string res;
  auto buffer = new WriteStreamBuffer(res, write_size);
  elle::IOStream stream(buffer);

  {
    std::stringstream ref(content);

    bool first = true;
    while (!ref.eof())
    {
      if (!first)
        stream << " ";
      else
        first = false;
      std::string word;
      ref >> word;
      stream << word;
    }
    stream.flush();
  }
  BOOST_CHECK_EQUAL(res, content);
}

class BeaconException
{};

class ExceptionBuffer:
  public elle::StreamBuffer
{
  virtual
  elle::WeakBuffer
  write_buffer()
  {
    throw BeaconException();
  }

  virtual
  elle::WeakBuffer
  read_buffer()
  {
    throw BeaconException();
  }
};

static
void
exceptions()
{
  {
    elle::IOStream stream(new ExceptionBuffer);
    BOOST_CHECK_THROW(stream.write("lol", 3), BeaconException);
  }
  {
    elle::IOStream stream(new ExceptionBuffer);
    char buf[3];
    BOOST_CHECK_THROW(stream.read(buf, sizeof(buf)), BeaconException);
  }
}

static
void
readsome_eof()
{
  std::stringstream s;
  char c;
  BOOST_CHECK_EQUAL(readsome(s, &c, 1), 0);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_read, 1)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_read, 2)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_read, 3)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_read, 4)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_read, 5)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_read, 6)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_read, 7)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_read, 8)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_write, 1)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_write, 2)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_write, 3)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_write, 4)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_write, 5)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_write, 6)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_write, 7)));
  suite.add(BOOST_TEST_CASE(std::bind(test_stream_buffer_write, 8)));
  suite.add(BOOST_TEST_CASE(exceptions));
  suite.add(BOOST_TEST_CASE(readsome_eof));
}
