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

  return true;
}

int
main(int argc, char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
