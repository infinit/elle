#include <iostream>
#include <sstream>

#include <elle/test.hh>

#include <elle/Buffer.hh>

static
void
test_ctor_empty()
{
  auto b = elle::Buffer{};
  BOOST_TEST(b.size() == 0);
  BOOST_CHECK(b.contents());
}

static
void
test_ctor_size(size_t size)
{
  auto b = elle::Buffer(size);
  BOOST_TEST(b.size() == size);
  for (unsigned i = 0; i < size; ++i)
    b.mutable_contents()[i] = 0xaa;
  for (unsigned i = 0; i < size; ++i)
    BOOST_TEST(b.contents()[i] == 0xaa);
}

static
void
test_ctor_raw(size_t size)
{
  auto* raw = new elle::Buffer::Byte[size];
  for (unsigned i = 0; i < size; ++i)
    raw[i] = 0xaa;
  auto b = elle::Buffer(raw, size);
  BOOST_TEST(b.size() == size);
  if (size > 0) // malloc(0) can legitimately return the same address twice.
    BOOST_TEST(static_cast<void*>(b.contents()) != static_cast<void*>(raw));
  for (unsigned i = 0; i < size; ++i)
    raw[i] = 0;
  delete [] raw;
  for (unsigned i = 0; i < size; ++i)
    BOOST_TEST(b.contents()[i] == 0xaa);
}

static
void
test_ctor_move(size_t size)
{
  auto source = elle::Buffer(size);
  BOOST_TEST(source.size() == size);
  for (unsigned i = 0; i < size; ++i)
    source.mutable_contents()[i] = 0xaa;
  auto dest = elle::Buffer(std::move(source));
  for (unsigned i = 0; i < size; ++i)
    BOOST_TEST(dest.contents()[i] == 0xaa);
  BOOST_TEST(source.size() == 0);
  BOOST_TEST(source.contents() == nullptr);
}

static
void
test_ctor_weak_raw()
{
  elle::Buffer::Byte ptr[42];
  elle::WeakBuffer b(ptr, 42);
  BOOST_TEST(b.contents() == ptr);
  BOOST_TEST(b.mutable_contents() == ptr);
  BOOST_TEST(b.size() == 42);
}

static
void
test_ctor_weak_buffer()
{
  auto b = elle::Buffer(7);
  elle::WeakBuffer wb(b);
  BOOST_TEST(wb.contents() == b.contents());
  BOOST_TEST(wb.mutable_contents() == b.contents());
  BOOST_TEST(wb.size() == 7);
}

static
void
test_ctor_weak_copy_move()
{
  elle::Buffer::Byte* raw = new elle::Buffer::Byte[7];
  elle::WeakBuffer wb1(raw, 7);;
  elle::WeakBuffer wb2(wb1);
  BOOST_TEST(wb2.contents() == wb1.contents());
  BOOST_TEST(wb2.size() == wb1.size());

  auto wb3 = elle::WeakBuffer(std::move(wb2));
  BOOST_TEST(wb3.contents() == raw);
  BOOST_TEST(wb3.size() == 7);
  BOOST_TEST(!wb2.contents());
  BOOST_TEST(wb2.size() == 0);

  delete[] raw;
}

template <typename Buffer>
static
Buffer
mkbuf(size_t size);

template <>
elle::Buffer
mkbuf<elle::Buffer>(size_t size)
{
  auto res = elle::Buffer(size);

  for (size_t i = 0; i < size; ++i)
    res.mutable_contents()[i] = ((elle::Buffer::Byte) i);

  return res;
}

template <>
elle::WeakBuffer
mkbuf<elle::WeakBuffer>(size_t size)
{
  auto* raw = new elle::Buffer::Byte[size];
  auto res = elle::WeakBuffer(raw, size);

  for (size_t i = 0; i < size; ++i)
    res.mutable_contents()[i] = ((elle::Buffer::Byte) i);

  return res;
}

template <typename Buffer, void (*Delete) (elle::Buffer::Byte*)>
static
void
test_cmp()
{
  {
    auto b1 = Buffer(mkbuf<Buffer>(16));
    auto b2 = Buffer(mkbuf<Buffer>(16));

    BOOST_CHECK_EQUAL(b1, b2);
    BOOST_CHECK_GE(b1, b2);
    BOOST_CHECK_LE(b1, b2);

    b1.mutable_contents()[7] = 8;
    BOOST_CHECK_NE(b1, b2);
    BOOST_CHECK_GT(b1, b2);
    BOOST_CHECK_GE(b1, b2);

    b1.mutable_contents()[7] = 6;
    BOOST_CHECK_NE(b1, b2);
    BOOST_CHECK_LT(b1, b2);
    BOOST_CHECK_LE(b1, b2);

    Delete(b1.mutable_contents());
    Delete(b2.mutable_contents());
  }

  {
    auto b1 = Buffer(mkbuf<Buffer>(4));
    auto b2 = Buffer(mkbuf<Buffer>(8));

    BOOST_CHECK_NE(b1, b2);
    BOOST_CHECK_LT(b1, b2);
    BOOST_CHECK_LE(b1, b2);

    Delete(b1.mutable_contents());
    Delete(b2.mutable_contents());
  }

  {
    auto b1 = Buffer(mkbuf<Buffer>(8));
    auto b2 = Buffer(mkbuf<Buffer>(4));

    BOOST_CHECK_NE(b1, b2);
    BOOST_CHECK_GT(b1, b2);
    BOOST_CHECK_GE(b1, b2);

    Delete(b1.mutable_contents());
    Delete(b2.mutable_contents());
  }
}

static
void
test_capacity()
{
  auto b = elle::Buffer(64);
  BOOST_CHECK_GE(b.capacity(), 64);
  b.size(128);
  BOOST_CHECK_GE(b.capacity(), 128);

  char more[128];
  b.append(more, 128);
  BOOST_CHECK_GE(b.capacity(), 256);

  auto prev = b.capacity();
  b.size(8);
  BOOST_TEST(b.capacity() == prev);
  b.shrink_to_fit();
  BOOST_TEST(b.capacity() == 8);
}

static
void
test_release()
{
  auto b = elle::Buffer(256);
  BOOST_CHECK_GE(b.capacity(), 256);
  b.release();
  BOOST_CHECK_LT(b.capacity(), 256);
  BOOST_TEST(b.size() == 0);
}

static
void
test_assign()
{
  auto b1 = elle::Buffer(8);
  auto b2 = elle::Buffer(16);
  BOOST_TEST(b1.size() == 8);
  BOOST_TEST(b2.size() == 16);
  b1 = std::move(b2);
  BOOST_TEST(b1.size() == 16);
  BOOST_TEST(b2.size() == 0);
}

static
void
delete_noop(elle::Buffer::Byte*)
{}

static
void
delete_array(elle::Buffer::Byte* p)
{
  delete[] p;
}

namespace print
{
  static const std::string data("somedata somedata somedata");

  static
  void
  string()
  {
    elle::ConstWeakBuffer buffer(data);
    BOOST_TEST(elle::sprintf("%s", buffer) == data);
  }

  static
  void
  string_fixed()
  {
    elle::ConstWeakBuffer buffer(data);
    BOOST_TEST(elle::sprintf("%f", buffer) == "somedata s...a somedata");
  }

  static
  void
  hexadecimal()
  {
    elle::ConstWeakBuffer buffer(data);
    BOOST_TEST(elle::sprintf("%x", buffer) ==
                      "0x736f6d656461746120736f6d656461746120736f6d6564617461");
  }

  static
  void
  printable()
  {
    static const std::string print("a\nb\nc");
    static const std::string noprint("\x01\x02\x03""abc");
    BOOST_TEST(elle::print("{}", elle::ConstWeakBuffer(print)) == print);
    BOOST_TEST(
      elle::print("{}", elle::ConstWeakBuffer(noprint)) == "0x010203616263");
  }
}

static
void
hash()
{
  std::hash<elle::ConstWeakBuffer> hash;
  BOOST_TEST(hash(elle::ConstWeakBuffer(""))
             == hash(elle::ConstWeakBuffer("")));
  BOOST_TEST(hash(elle::ConstWeakBuffer("aeouidhtns"))
             == hash(elle::ConstWeakBuffer("aeouidhtns")));
  BOOST_TEST(hash(elle::ConstWeakBuffer("aeouidhtns"))
             != hash(elle::ConstWeakBuffer("aeouidhtns-")));
  BOOST_TEST(hash(elle::ConstWeakBuffer("aeouidhtns"))
             != hash(elle::ConstWeakBuffer("snthdiueoa")));
}

template <typename B>
static
void
_range()
{
  auto data = strdup("0123456789");
  auto buffer = B(data, strlen(data));
  BOOST_TEST(buffer.range(3) == "3456789");
  BOOST_TEST(buffer.range(1, 6) == "12345");
  BOOST_TEST(buffer.range(5, -1) == "5678");
  BOOST_TEST(buffer.range(-5, -3) == "56");
  free(data);
}

static
void
range()
{
  _range<elle::ConstWeakBuffer>();
  _range<elle::WeakBuffer>();
  _range<elle::Buffer>();
}

static
void
output()
{
  auto buffer = elle::Buffer{};
  {
    elle::IOStream stream(buffer.ostreambuf());
    stream << 42;
    BOOST_TEST(buffer.size() == 0);
  }
  BOOST_TEST(buffer == "42");
}

static
void
input()
{
  {
    auto b = elle::Buffer("10 11", 5);
    elle::IOStream stream(b.istreambuf());
    int x, y;
    stream >> x >> y;
    BOOST_TEST(x == 10);
    BOOST_TEST(y == 11);
  }
  auto b = elle::Buffer("10 11 ", 6);
  auto b2 = elle::Buffer("12 13", 5);
  elle::IOStream stream(b.istreambuf_combine(b2));
  int x, y, z, a;
  stream >> x >> y >> z >> a;
  BOOST_TEST(x == 10);
  BOOST_TEST(y == 11);
  BOOST_TEST(z == 12);
  BOOST_TEST(a == 13);
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();

  // Buffer
  boost::unit_test::test_suite* buffer = BOOST_TEST_SUITE("Buffer");
  master.add(buffer);

  boost::unit_test::test_suite* ctor = BOOST_TEST_SUITE("Construction");
  buffer->add(ctor);

  ctor->add(BOOST_TEST_CASE(test_ctor_empty));

#define SIZE(F)                                    \
  ctor->add(BOOST_TEST_CASE(std::bind(F, 0)));     \
  ctor->add(BOOST_TEST_CASE(std::bind(F, 1)));     \
  ctor->add(BOOST_TEST_CASE(std::bind(F, 8)));     \
  ctor->add(BOOST_TEST_CASE(std::bind(F, 64)));    \
  ctor->add(BOOST_TEST_CASE(std::bind(F, 1024)));  \

  SIZE(test_ctor_size);
  SIZE(test_ctor_raw);
  SIZE(test_ctor_move);
#undef SIZE

  boost::unit_test::test_suite* cmp = BOOST_TEST_SUITE("Comparisons");
  buffer->add(cmp);
  cmp->add(BOOST_TEST_CASE((&test_cmp<elle::Buffer, delete_noop>)));

  boost::unit_test::test_suite* memory = BOOST_TEST_SUITE("Memory");
  buffer->add(memory);
  memory->add(BOOST_TEST_CASE(test_capacity));
  memory->add(BOOST_TEST_CASE(test_release));
  memory->add(BOOST_TEST_CASE(test_assign));

  boost::unit_test::test_suite* streams = BOOST_TEST_SUITE("streams");
  buffer->add(streams);
  streams->add(BOOST_TEST_CASE(output));
  streams->add(BOOST_TEST_CASE(input));

  // WeakBuffer
  boost::unit_test::test_suite* weakbuffer = BOOST_TEST_SUITE("WeakBuffer");
  master.add(weakbuffer);

  boost::unit_test::test_suite* ctor_weak = BOOST_TEST_SUITE("Construction");
  weakbuffer->add(ctor_weak);
  ctor->add(BOOST_TEST_CASE(test_ctor_weak_raw));
  ctor->add(BOOST_TEST_CASE(test_ctor_weak_buffer));
  ctor->add(BOOST_TEST_CASE(test_ctor_weak_copy_move));

  boost::unit_test::test_suite* cmp_weak = BOOST_TEST_SUITE("Comparisons");
  weakbuffer->add(cmp_weak);
  cmp_weak->add(BOOST_TEST_CASE((&test_cmp<elle::WeakBuffer, delete_array>)));

  {
    using namespace print;
    boost::unit_test::test_suite* print = BOOST_TEST_SUITE("print");
    master.add(print);
    print->add(BOOST_TEST_CASE(string));
    print->add(BOOST_TEST_CASE(string_fixed));
    print->add(BOOST_TEST_CASE(hexadecimal));
    print->add(BOOST_TEST_CASE(printable));
  }

  master.add(BOOST_TEST_CASE(hash), 0, 1);
  master.add(BOOST_TEST_CASE(range), 0, 1);
}
