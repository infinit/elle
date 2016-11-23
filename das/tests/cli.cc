#include <das/cli.hh>

#include <numeric>

#include <elle/test.hh>

ELLE_LOG_COMPONENT("das.cli.tests");

DAS_SYMBOL(foo);
DAS_SYMBOL(bar);
DAS_SYMBOL(baz);
DAS_SYMBOL(quux);

static
void
basics()
{
  auto const f =
    [] (std::string const& a, std::string const& b) { return a + b; };
  auto const proto = das::named::prototype(foo, baz);
  {
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"--foo", "bar", "--baz", "quux"}),
      "barquux");
  }
  BOOST_CHECK_THROW(
    das::cli::call(proto, f, {"--foo", "bar", "--bar", "quux"}),
    das::cli::UnknownOption);
  BOOST_CHECK_THROW(
    das::cli::call(proto, f, {"--foo", "bar", "garbage", "--baz", "quux"}),
    das::cli::UnrecognizedValue);
  BOOST_CHECK_THROW(
    das::cli::call(proto, f, {"--foo", "bar"}),
    das::cli::MissingOption);
  BOOST_CHECK_THROW(
    das::cli::call(proto, f, {"--foo", "bar", "--baz", "quux", "--foo", "foo"}),
    das::cli::DuplicateOption);
}

namespace conversions
{
  template <typename I>
  void
  check(std::string too_big, std::string too_little)
  {
    auto const max = std::numeric_limits<I>::max();
    auto const min = std::numeric_limits<I>::min();
    auto const proto = das::named::prototype(foo);
    auto const f = [] (I i)
      {
        return i;
      };
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"--foo", std::to_string(max)}), max);
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"--foo", std::to_string(min)}), min);
    BOOST_CHECK_THROW(
      das::cli::call(proto, f, {"--foo", too_big}),
      das::cli::ValueError);
    BOOST_CHECK_THROW(
      das::cli::call(proto, f, {"--foo", too_little}),
      das::cli::ValueError);
  }

  template <typename I>
  void
  check()
  {
    using Max = typename std::conditional<
      std::is_signed<I>::value, int64_t, uint64_t>::type;
    using Min = int64_t;
    using elle::sprintf;
    static_assert(std::is_integral<I>::value, "");
    Max const max = std::numeric_limits<I>::max();
    Min const min = std::numeric_limits<I>::min();
    check<I>(std::to_string(max + 1), std::to_string(min - 1));
  }

  static
  void
  integers()
  {
    check<int8_t>();
    check<int16_t>();
    check<int32_t>();
    check<int64_t>("9223372036854775808", "-9223372036854775809");
    check<uint8_t>();
    check<uint16_t>();
    check<uint32_t>();
    check<uint64_t>("18446744073709551616", "-1");
    {
      auto const proto = das::named::prototype(foo);
      {
        auto const f = [] (int i) {return i;};
        BOOST_CHECK_THROW(
          das::cli::call(proto, f, {"--foo", "lol"}), das::cli::ValueError);
      }
      // Verify we check for trailing garbage.
      {
        auto const f = [] (unsigned i) {return i;};
        BOOST_CHECK_THROW(
          das::cli::call(
            proto, f, {"--foo", "007 james bond"}), das::cli::ValueError);
      }
      {
        auto const f = [] (signed i) {return i;};
        BOOST_CHECK_THROW(
          das::cli::call(
            proto, f, {"--foo", "-666numberofthebeast"}), das::cli::ValueError);
      }
    }
  }

  static
  void
  multiple_strings()
  {
    auto const f = [] (std::vector<std::string> strings)
      {
        return std::accumulate(
          strings.begin(), strings.end(), std::string(""),
          [] (std::string const& a, std::string const& b)
          { return a + "-" + b; });
      };
    auto const proto = das::named::prototype(foo);
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {}),
      "");
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"--foo", "foo"}),
      "-foo");
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"--foo", "foo", "--foo", "bar"}),
      "-foo-bar");
  }

  static
  void
  boolean()
  {
    auto const f = [] (int expected, bool enabled)
      {
        BOOST_CHECK_EQUAL(bool(expected), enabled);
      };
    auto const proto = das::named::prototype(foo, bar);
    das::cli::call(proto, f, {"--foo", "0"});
    das::cli::call(proto, f, {"--foo", "1", "--bar"});
    das::cli::call(proto, f, {"--bar", "--foo", "1"});
    das::cli::call(proto, f, {"--foo", "1", "--bar", "true"});
    das::cli::call(proto, f, {"--foo", "0", "--bar", "false"});
  }

  static
  void
  multiple_integers()
  {
    auto const f = [] (std::vector<int> ints) { return ints; };
    auto const proto = das::named::prototype(foo);
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {}), std::vector<int>{});
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"--foo", "2"}), std::vector<int>{2});
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"--foo", "2", "--foo", "3"}),
      std::vector<int>({2, 3}));
  }
}

static
void
defaults()
{
  auto const f =
    [] (std::string const& foo, int baz) { return foo + std::to_string(baz); };
  auto const proto = das::named::prototype(foo = "16", baz = 42);
  BOOST_CHECK_EQUAL(das::cli::call(proto, f, {}), "1642");
  BOOST_CHECK_EQUAL(das::cli::call(proto, f, {"--baz", "64"}), "1664");
  BOOST_CHECK_EQUAL(das::cli::call(proto, f, {"--foo", "51"}), "5142");
  BOOST_CHECK_EQUAL(
    das::cli::call(proto, f, {"--baz", "23", "--foo", "01"}), "0123");
}

DAS_SYMBOL(composite_option);

static
void
dash()
{
  auto const f = [] (int foo) { return foo; };
  auto const proto = das::named::prototype(composite_option);
  {
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"--composite-option", "193"}), 193);
    BOOST_CHECK_THROW(
      das::cli::call(proto, f, {"--composite_option", "193"}),
      das::cli::UnknownOption);
  }
}

static
void
short_options()
{
  auto const f =
    [] (int foo, int bar) { return foo + bar; };
  auto const proto = das::named::prototype(foo, bar);
  {
    das::cli::Options opts = {
      {"foo", {'f', ""}},
      {"bar", {'b', ""}},
    };
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"--foo", "1", "-b", "2"}, opts), 3);
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"-f", "3", "--bar", "4"}, opts), 7);
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"-f", "5", "-b", "6"}, opts), 11);
  }
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(basics));
  auto conversions = BOOST_TEST_SUITE("conversions");
  master.add(conversions);
  {
    using namespace conversions;
    conversions->add(BOOST_TEST_CASE(integers));
    conversions->add(BOOST_TEST_CASE(boolean));
    conversions->add(BOOST_TEST_CASE(multiple_strings));
    conversions->add(BOOST_TEST_CASE(multiple_integers));
  }
  master.add(BOOST_TEST_CASE(defaults));
  master.add(BOOST_TEST_CASE(short_options));
  master.add(BOOST_TEST_CASE(dash));
}
