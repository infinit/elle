#include <iostream>
#include <sstream>

#include <elle/test.hh>

#include <elle/Buffer.hh>

static
void
test_ctor_empty()
{
  elle::Buffer b;
  BOOST_CHECK_EQUAL(b.size(), 0);
  BOOST_CHECK(b.contents());
}

static
void
test_ctor_size(size_t size)
{
  elle::Buffer b(size);
  BOOST_CHECK_EQUAL(b.size(), size);
  for (unsigned i = 0; i < size; ++i)
    b.mutable_contents()[i] = 0xaa;
  for (unsigned i = 0; i < size; ++i)
    BOOST_CHECK_EQUAL(b.contents()[i], 0xaa);
}

static
void
test_ctor_raw(size_t size)
{
  elle::Byte* raw = new elle::Byte[size];
  for (unsigned i = 0; i < size; ++i)
    raw[i] = 0xaa;
  elle::Buffer b(raw, size);
  BOOST_CHECK_EQUAL(b.size(), size);
  if (size > 0) // malloc(0) can legitimately return the same address twice.
    BOOST_CHECK_NE(static_cast<void*>(b.contents()), static_cast<void*>(raw));
  for (unsigned i = 0; i < size; ++i)
    raw[i] = 0;
  delete [] raw;
  for (unsigned i = 0; i < size; ++i)
    BOOST_CHECK_EQUAL(b.contents()[i], 0xaa);
}

static
void
test_ctor_move(size_t size)
{
  elle::Buffer source(size);
  BOOST_CHECK_EQUAL(source.size(), size);
  for (unsigned i = 0; i < size; ++i)
    source.mutable_contents()[i] = 0xaa;
  elle::Buffer dest(std::move(source));
  for (unsigned i = 0; i < size; ++i)
    BOOST_CHECK_EQUAL(dest.contents()[i], 0xaa);
  BOOST_CHECK_EQUAL(source.size(), 0);
  BOOST_CHECK(source.contents());
}

static
void
test_ctor_weak_raw()
{
  elle::Byte ptr[42];
  elle::WeakBuffer b(ptr, 42);
  BOOST_CHECK_EQUAL(b.contents(), ptr);
  BOOST_CHECK_EQUAL(b.mutable_contents(), ptr);
  BOOST_CHECK_EQUAL(b.size(), 42);
}

static
void
test_ctor_weak_buffer()
{
  elle::Buffer b(7);
  elle::WeakBuffer wb(b);
  BOOST_CHECK_EQUAL(wb.contents(), b.contents());
  BOOST_CHECK_EQUAL(wb.mutable_contents(), b.contents());
  BOOST_CHECK_EQUAL(wb.size(), 7);
}

static
void
test_ctor_weak_copy_move()
{
  elle::Byte* raw = new elle::Byte[7];
  elle::WeakBuffer wb1(raw, 7);;
  elle::WeakBuffer wb2(wb1);
  BOOST_CHECK_EQUAL(wb2.contents(), wb1.contents());
  BOOST_CHECK_EQUAL(wb2.size(), wb1.size());

  elle::WeakBuffer wb3(std::move(wb2));
  BOOST_CHECK_EQUAL(wb3.contents(), raw);
  BOOST_CHECK_EQUAL(wb3.size(), 7);
  BOOST_CHECK_EQUAL(wb2.contents(), (elle::Byte*)(nullptr));
  BOOST_CHECK_EQUAL(wb2.size(), 0);

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
  elle::Buffer buffer(size);

  for (size_t i = 0; i < size; ++i)
    buffer.mutable_contents()[i] = ((elle::Byte) i);

  return buffer;
}

template <>
elle::WeakBuffer
mkbuf<elle::WeakBuffer>(size_t size)
{
  elle::Byte* raw = new elle::Byte[size];
  elle::WeakBuffer buffer(raw, size);

  for (size_t i = 0; i < size; ++i)
    buffer.mutable_contents()[i] = ((elle::Byte) i);

  return buffer;
}

template <typename Buffer, void (*Delete) (elle::Byte*)>
static
void
test_cmp()
{
  {
    Buffer b1(mkbuf<Buffer>(16));
    Buffer b2(mkbuf<Buffer>(16));

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
    Buffer b1(mkbuf<Buffer>(4));
    Buffer b2(mkbuf<Buffer>(8));

    BOOST_CHECK_NE(b1, b2);
    BOOST_CHECK_LT(b1, b2);
    BOOST_CHECK_LE(b1, b2);

    Delete(b1.mutable_contents());
    Delete(b2.mutable_contents());
  }

  {
    Buffer b1(mkbuf<Buffer>(8));
    Buffer b2(mkbuf<Buffer>(4));

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
  elle::Buffer b(64);
  BOOST_CHECK_GE(b.capacity(), 64);
  b.size(128);
  BOOST_CHECK_GE(b.capacity(), 128);

  char more[128];
  b.append(more, 128);
  BOOST_CHECK_GE(b.capacity(), 256);

  auto prev = b.capacity();
  b.size(8);
  BOOST_CHECK_EQUAL(b.capacity(), prev);
  b.shrink_to_fit();
  BOOST_CHECK_EQUAL(b.capacity(), 8);
}

static
void
test_release()
{
  elle::Buffer b(256);
  BOOST_CHECK_GE(b.capacity(), 256);
  b.release();
  BOOST_CHECK_LT(b.capacity(), 256);
  BOOST_CHECK_EQUAL(b.size(), 0);
}

static
void
test_assign()
{
  elle::Buffer b1(8);
  elle::Buffer b2(16);
  BOOST_CHECK_EQUAL(b1.size(), 8);
  BOOST_CHECK_EQUAL(b2.size(), 16);
  b1 = std::move(b2);
  BOOST_CHECK_EQUAL(b1.size(), 16);
  BOOST_CHECK_EQUAL(b2.size(), 0);
}

static
void
delete_noop(elle::Byte*)
{}

static
void
delete_array(elle::Byte* p)
{
  delete [] p;
}

namespace print
{
  static const std::string data("somedata somedata somedata");

  static
  void
  string()
  {
    elle::ConstWeakBuffer buffer(data);
    BOOST_CHECK_EQUAL(elle::sprintf("%s", buffer), data);
  }

  static
  void
  string_fixed()
  {
    elle::ConstWeakBuffer buffer(data);
    BOOST_CHECK_EQUAL(elle::sprintf("%f", buffer), "somedata s...a somedata");
  }

  static
  void
  hexadecimal()
  {
    elle::ConstWeakBuffer buffer(data);
    BOOST_CHECK_EQUAL(elle::sprintf("%x", buffer),
                      "0x736f6d656461746120736f6d656461746120736f6d6564617461");
    boost::format fmt("%s");
  }
}

static
void
hash()
{
  std::hash<elle::ConstWeakBuffer> hash;
  BOOST_CHECK_EQUAL(hash(elle::ConstWeakBuffer("")),
                    hash(elle::ConstWeakBuffer("")));
  BOOST_CHECK_EQUAL(hash(elle::ConstWeakBuffer("aeouidhtns")),
                    hash(elle::ConstWeakBuffer("aeouidhtns")));
  BOOST_CHECK_NE(hash(elle::ConstWeakBuffer("aeouidhtns")),
                 hash(elle::ConstWeakBuffer("aeouidhtns-")));
  BOOST_CHECK_NE(hash(elle::ConstWeakBuffer("aeouidhtns")),
                 hash(elle::ConstWeakBuffer("snthdiueoa")));
}

static
void
range()
{
  elle::ConstWeakBuffer buffer("0123456789");
  BOOST_CHECK_EQUAL(buffer.range(3), "3456789");
  BOOST_CHECK_EQUAL(buffer.range(1, 6), "12345");
  BOOST_CHECK_EQUAL(buffer.range(5, -1), "5678");
  BOOST_CHECK_EQUAL(buffer.range(-5, -3), "56");
}

static
void
output()
{
  elle::Buffer buffer;
  {
    elle::IOStream stream(buffer.ostreambuf());
    stream << 42;
    BOOST_CHECK_EQUAL(buffer.size(), 0);
  }
  BOOST_CHECK_EQUAL(buffer, "42");
}

static
void
input()
{
  {
    elle::Buffer b("10 11", 5);
    elle::IOStream stream(b.istreambuf());
    int x, y;
    stream >> x >> y;
    BOOST_CHECK_EQUAL(x, 10);
    BOOST_CHECK_EQUAL(y, 11);
  }
  elle::Buffer b("10 11 ", 6);
  elle::Buffer b2("12 13", 5);
  elle::IOStream stream(b.istreambuf_combine(b2));
  int x, y, z, a;
  stream >> x >> y >> z >> a;
  BOOST_CHECK_EQUAL(x, 10);
  BOOST_CHECK_EQUAL(y, 11);
  BOOST_CHECK_EQUAL(z, 12);
  BOOST_CHECK_EQUAL(a, 13);
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
  }

  master.add(BOOST_TEST_CASE(hash), 0, 1);
  master.add(BOOST_TEST_CASE(range), 0, 1);
}
