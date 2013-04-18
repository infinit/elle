#include <iostream>
#include <sstream>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

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
  BOOST_CHECK_NE(b.contents(), raw);
  for (unsigned i = 0; i < size; ++i)
    raw[i] = 0;
  delete [] raw;
  for (unsigned i = 0; i < size; ++i)
    BOOST_CHECK_EQUAL(b.contents()[i], 0xaa);
}

static
void
test_ctor_content_pair(size_t size)
{
  elle::Byte* raw = new elle::Byte[size];
  for (unsigned i = 0; i < size; ++i)
    raw[i] = 0x55;
  elle::Buffer b(std::make_pair(elle::Buffer::ContentPtr(raw), size));
  BOOST_CHECK_EQUAL(b.size(), size);
  BOOST_CHECK_EQUAL(b.contents(), raw);
  for (unsigned i = 0; i < size; ++i)
    raw[i] = 0xaa;
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
elle::Buffer
mkbuf(size_t size)
{
  elle::Buffer buffer(size);

  for (int i = 0; i < 16; ++i)
    buffer.mutable_contents()[i] = i;

  return buffer;
}

static
void
test_cmp()
{
  {
    elle::Buffer b1(mkbuf(16));
    elle::Buffer b2(mkbuf(16));

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
  }

  {
    elle::Buffer b1(mkbuf(4));
    elle::Buffer b2(mkbuf(8));

    BOOST_CHECK_NE(b1, b2);
    BOOST_CHECK_LT(b1, b2);
    BOOST_CHECK_LE(b1, b2);
  }

  {
    elle::Buffer b1(mkbuf(8));
    elle::Buffer b2(mkbuf(4));

    BOOST_CHECK_NE(b1, b2);
    BOOST_CHECK_GT(b1, b2);
    BOOST_CHECK_GE(b1, b2);
  }
}

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
bool
test_suite()
{
  boost::unit_test::test_suite* ctor = BOOST_TEST_SUITE("Construction");
  boost::unit_test::framework::master_test_suite().add(ctor);

  ctor->add(BOOST_TEST_CASE(test_ctor_empty));

#define SIZE(F)                                    \
  ctor->add(BOOST_TEST_CASE(std::bind(F, 0)));     \
  ctor->add(BOOST_TEST_CASE(std::bind(F, 1)));     \
  ctor->add(BOOST_TEST_CASE(std::bind(F, 8)));     \
  ctor->add(BOOST_TEST_CASE(std::bind(F, 64)));    \
  ctor->add(BOOST_TEST_CASE(std::bind(F, 1024)));  \

  SIZE(test_ctor_size);
  SIZE(test_ctor_raw);
  SIZE(test_ctor_content_pair);
  SIZE(test_ctor_move);
#undef SIZE

  boost::unit_test::test_suite* cmp = BOOST_TEST_SUITE("Comparisons");
  boost::unit_test::framework::master_test_suite().add(cmp);
  cmp->add(BOOST_TEST_CASE(test_cmp));

  boost::unit_test::test_suite* memory = BOOST_TEST_SUITE("Memory");
  boost::unit_test::framework::master_test_suite().add(memory);
  memory->add(BOOST_TEST_CASE(test_capacity));
  memory->add(BOOST_TEST_CASE(test_release));

  return true;
}

int
main(int argc, char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
