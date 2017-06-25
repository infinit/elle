#include <chrono>
#include <ratio>

#include <elle/test.hh>

#define CHECK_PRETTY_PRINT(O, Expected)                 \
  BOOST_TEST(elle::sprintf("%s", O) == Expected)

static
void
pretty_print()
{
  CHECK_PRETTY_PRINT((std::chrono::duration<int, std::nano>(42)), "42ns");
  CHECK_PRETTY_PRINT((std::chrono::duration<int, std::nano>(42000)), "42us");
  CHECK_PRETTY_PRINT((std::chrono::duration<int, std::micro>(42)), "42us");
  CHECK_PRETTY_PRINT((std::chrono::duration<int, std::nano>(42000000)), "42ms");
  CHECK_PRETTY_PRINT((std::chrono::duration<int, std::micro>(42000)), "42ms");
  CHECK_PRETTY_PRINT((std::chrono::duration<int, std::milli>(42)), "42ms");
  CHECK_PRETTY_PRINT((std::chrono::duration<int, std::ratio<1, 1>>(42)), "42s");
  CHECK_PRETTY_PRINT((std::chrono::duration<int, std::milli>(1234)), "1.23s");
  CHECK_PRETTY_PRINT((std::chrono::duration<int, std::milli>(12340)), "12.3s");
  CHECK_PRETTY_PRINT((std::chrono::duration<int, std::milli>(123400)), "123s");
  CHECK_PRETTY_PRINT((std::chrono::duration<int, std::milli>(-12340)), "-12.3s");
  CHECK_PRETTY_PRINT((std::chrono::duration<int, std::milli>(-123400)), "-123s");
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(pretty_print), 0, 1);
}
