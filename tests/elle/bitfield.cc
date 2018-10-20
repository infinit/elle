#include <elle/bitfield.hh>

#include <elle/test.hh>

enum class Test : uint16_t
{
  A = 1 << 0,
  B = 1 << 1,
  C = 1 << 2,
};

ELLE_BITFIELD(Test);

static_assert(std::is_same<decltype(Test::A | Test::B), Test>::value);

static
void
basics()
{
  BOOST_CHECK(static_cast<uint16_t>(Test::A | Test::B) == 0b011);
  BOOST_CHECK(static_cast<uint16_t>(Test::A | Test::C) == 0b101);
  BOOST_CHECK(static_cast<uint16_t>(Test::B | Test::C) == 0b110);
  BOOST_CHECK(static_cast<uint16_t>(Test::A | Test::B | Test::C) == 0b111);
  BOOST_CHECK(  (Test::A | Test::C) & Test::A);
  BOOST_CHECK(!((Test::A | Test::C) & Test::B));
  BOOST_CHECK(  (Test::A | Test::C) & Test::C);
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();

  master.add(BOOST_TEST_CASE(basics));
}
