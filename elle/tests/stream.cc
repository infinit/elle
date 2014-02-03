#include <elle/OStream.hh>
#include <elle/stream/Buffer.hh>
#include <elle/test.hh>

class TestBackend:
  public elle::OStream::Backend
{
public:
  TestBackend():
    _position(0)
  {}

  virtual
  void
  write(elle::ConstWeakBuffer const& data)
  {
    BOOST_CHECK_GE(data.size(), 3u);
    BOOST_CHECK_LE(data.size(), 5u);
    for (unsigned i = 0; i < data.size(); ++i)
      BOOST_CHECK_EQUAL(data[i], this->_position++);
  }

  virtual
  Size
  size()
  {
    return 3;
  }

  virtual
  Size
  size(Size size)
  {
    BOOST_CHECK_GT(size, 3u);
    return std::min(size, 5u);
  }

  ELLE_ATTRIBUTE_R(int, position);
};

static
void
ostream()
{
  auto backend = new TestBackend;
  auto copy = backend;
  elle::OStream stream{std::unique_ptr<elle::OStream::Backend>(copy)};
  stream.write(0);
  BOOST_CHECK_EQUAL(backend->position(), 0);
  stream.write(1);
  BOOST_CHECK_EQUAL(backend->position(), 0);
  stream.write(2);
  BOOST_CHECK_EQUAL(backend->position(), 3);
  stream.write("\x3\x4");
  BOOST_CHECK_EQUAL(backend->position(), 3);
  stream.write("\x5\x6");
  BOOST_CHECK_EQUAL(backend->position(), 6);
  stream.write("\x7\x8");
  BOOST_CHECK_EQUAL(backend->position(), 9);
  stream.write("\x9\xA\xB\xC");
  BOOST_CHECK_EQUAL(backend->position(), 13);
  stream.write("\xD\xE\xF\x10\x11");
  BOOST_CHECK_EQUAL(backend->position(), 18);
  stream.write("\x12\x13\x14\x15\x16\x17");
  BOOST_CHECK_EQUAL(backend->position(), 23);
  stream.write("\x18");
  BOOST_CHECK_EQUAL(backend->position(), 23);
  stream.write("\x19");
  BOOST_CHECK_EQUAL(backend->position(), 26);
}

static
void
output_buffer()
{
  elle::Buffer b;

  {
    auto stream = elle::ostream(b);
    stream.write("lol");
    BOOST_CHECK_EQUAL(b.size(), 0);
  }
  BOOST_CHECK_EQUAL(b.string(), "lol");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(ostream));
  suite.add(BOOST_TEST_CASE(output_buffer));
}
