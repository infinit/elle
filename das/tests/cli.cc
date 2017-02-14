#include <das/cli.hh>

#include <numeric>

#include <elle/test.hh>

DAS_SYMBOL(foo);
DAS_SYMBOL(bar);
DAS_SYMBOL(baz);
DAS_SYMBOL(quux);

#define CHECK_THROW(Expression, Exception, Message)     \
  do {                                                  \
    BOOST_CHECK_THROW(Expression, Exception);           \
    try                                                 \
    {                                                   \
      Expression;                                       \
    }                                                   \
    catch (Exception const& e)                          \
    {                                                   \
      BOOST_CHECK_EQUAL(elle::sprintf("%s", e),         \
                        Message);                       \
    }                                                   \
  } while (false)

static
void
basics()
{
  auto const f =
    [] (std::string const& a, std::string const& b) { return a + b; };
  auto const proto = das::named::prototype(foo, baz);

  BOOST_CHECK_EQUAL(
    das::cli::call(proto, f, {"--foo", "bar", "--baz", "quux"}),
    "barquux");
  BOOST_CHECK_EQUAL(
    das::cli::call(proto, f, {"--foo=bar", "--baz", "quux"}),
    "barquux");
  BOOST_CHECK_EQUAL(
    das::cli::call(proto, f, {"--foo=bar", "--baz=--quux"}),
    "bar--quux");
  BOOST_CHECK_EQUAL(
    das::cli::call(proto, f, {"--foo=", "--baz="}),
    "");
  CHECK_THROW(
    das::cli::call(proto, f, {"--foo", "bar", "--baz", "x", "--bar", "quux"}),
    das::cli::UnknownOption,
    "unknown option: --bar");
  CHECK_THROW(
    das::cli::call(proto, f, {"--foo", "bar", "garbage", "--baz", "quux"}),
    das::cli::UnrecognizedValue,
    "extra unrecognized argument: garbage");
  CHECK_THROW(
    das::cli::call(proto, f, {"--foo", "bar"}),
    das::cli::MissingOption,
    "missing option: --baz");
  CHECK_THROW(
    das::cli::call(proto, f, {"--foo", "bar", "--baz", "quux", "--foo", "foo"}),
    das::cli::DuplicateOption,
    "duplicate option: --foo");
}

namespace conversions
{
  template <typename I>
  void
  check(std::string too_big, std::string too_little)
  {
    auto constexpr max = std::numeric_limits<I>::max();
    auto constexpr min = std::numeric_limits<I>::min();
    auto const proto = das::named::prototype(foo);
    auto const f = [] (I i)
      {
        return i;
      };
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"--foo", std::to_string(max)}), max);
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"--foo", std::to_string(min)}), min);
    CHECK_THROW(
      das::cli::call(proto, f, {"--foo", too_big}),
      das::cli::ValueError,
      "invalid value \"" + too_big + "\" for option --foo: integer out of range");
    CHECK_THROW(
      das::cli::call(proto, f, {"--foo", too_little}),
      das::cli::ValueError,
      "invalid value \"" + too_little + "\" for option --foo: integer out of range");
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
    auto constexpr max = Max{std::numeric_limits<I>::max()};
    auto constexpr min = Min{std::numeric_limits<I>::min()};
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
        CHECK_THROW(
          das::cli::call(proto, f, {"--foo", "lol"}), das::cli::ValueError,
          "invalid value \"lol\" for option --foo: invalid integer");
      }
      // Verify we check for trailing garbage.
      {
        auto const f = [] (unsigned i) {return i;};
        CHECK_THROW(
          das::cli::call(
            proto, f, {"--foo", "007 james bond"}), das::cli::ValueError,
          "invalid value \"007 james bond\" for option --foo: invalid integer");
      }
      {
        auto const f = [] (signed i) {return i;};
        CHECK_THROW(
          das::cli::call(
            proto, f, {"--foo", "-666numberofthebeast"}), das::cli::ValueError,
          "invalid value \"-666numberofthebeast\" for option --foo: invalid integer");
      }
    }
  }

  static
  void
  multiple_strings()
  {
    auto const f = [] (std::vector<std::string> const& strings)
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
    {
      auto const f = [] (int expected, bool enabled)
        {
          BOOST_CHECK_EQUAL(bool(expected), enabled);
        };
      auto const named = das::named::function(f, foo, bar = false);
      das::cli::call(named, {"--foo", "0"});
      das::cli::call(named, {"--foo", "1", "--bar"});
      das::cli::call(named, {"--bar", "--foo", "1"});
      das::cli::call(named, {"--foo", "1", "--bar", "true"});
      das::cli::call(named, {"--foo", "0", "--bar", "false"});
    }
    {
      auto const f = [] (bool b)
        {
          BOOST_CHECK(!b);
        };
      das::cli::call(das::named::function(f, foo), {});
    }
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

static
void
flag()
{
  auto const f =
    [] (std::string const& foo, int baz) { return foo + std::to_string(baz); };
  auto const proto = das::named::prototype(foo, baz);
  BOOST_CHECK_THROW(das::cli::call(proto, f, {"--foo", "--bar", "bar"}),
                    das::cli::OptionValueError);
  BOOST_CHECK_THROW(das::cli::call(proto, f, {"--foo", "foo", "--bar"}),
                    das::cli::OptionValueError);
}

DAS_SYMBOL(composite_option);

static
void
dash()
{
  auto const f = [] (int foo) { return foo; };
  auto const proto = das::named::prototype(composite_option = 0);
  {
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"--composite-option", "193"}), 193);
    CHECK_THROW(
      das::cli::call(proto, f, {"--composite_option", "193"}),
      das::cli::UnknownOption,
      "unknown option: --composite_option");
  }
}

namespace short_options_ct
{
  DAS_CLI_SYMBOL(foo, 'f', "", false);
  DAS_CLI_SYMBOL(bar, 'b', "", false);

  static
  void
  compile_time()
  {
    auto const f =
      [] (int foo, int bar) { return foo + bar; };
    auto const proto = das::named::prototype(foo, bar);
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"--foo", "1", "-b", "2"}), 3);
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"-f", "3", "--bar", "4"}), 7);
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"-f", "5", "-b", "6"}), 11);
  }
}

namespace short_options_rt
{
  static
  void
  run_time()
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
}

namespace positional_ct
{
  DAS_CLI_SYMBOL(foo, 'f', "", false);
  DAS_CLI_SYMBOL(bar, 'b', "", true);
  DAS_CLI_SYMBOL(quux, 'q', "", true);

  static
  void
  compile_time()
  {
    auto const f =
      [] (int foo, int bar) { return foo + bar; };
    {
      auto const proto = das::named::prototype(foo, bar);
      BOOST_CHECK_EQUAL(
        das::cli::call(proto, f, {"-f", "1", "-b", "2"}), 3);
      BOOST_CHECK_EQUAL(
        das::cli::call(proto, f, {"-f", "3", "4"}), 7);
      BOOST_CHECK_EQUAL(
        das::cli::call(proto, f, {"6", "-f", "5"}), 11);
    }
    {
      auto const proto = das::named::prototype(foo = 1, bar = 2);
      BOOST_CHECK_EQUAL(das::cli::call(proto, f, {"247"}), 248);
    }
    {
      auto const f =
        [] (std::vector<int> const& ints)
        {
          return std::accumulate(ints.begin(), ints.end(), 0);
        };
      auto const proto = das::named::prototype(bar);
      BOOST_CHECK_EQUAL(
        das::cli::call(proto, f, {"1", "2", "3"}), 6);
      CHECK_THROW(
        das::cli::call(proto, f, {"-b", "1", "2", "3"}),
        das::cli::UnrecognizedValue,
        "extra unrecognized argument: 2");
    }
    {
      auto const f = das::named::function(
        [] (int b, int q)
        {
          return b - q;
        }, bar, quux);
      BOOST_CHECK_EQUAL(
        das::cli::call(f, {"-b", "1", "-q", "2"}), -1);
      // Not handled yet.
      // BOOST_CHECK_EQUAL(
      //   das::cli::call(f, {"1", "2"}), -1);
    }
  }
}

namespace positional_rt
{
  static
  void
  run_time()
  {
    auto const f =
      [] (int foo, int bar) { return foo + bar; };
    das::cli::Options opts = {
      {"foo", {'f', "", false}},
      {"bar", {'b', "", true}},
    };
    {
      auto const proto = das::named::prototype(foo, bar);
      BOOST_CHECK_EQUAL(
        das::cli::call(proto, f, {"-f", "1", "-b", "2"}, opts), 3);
      BOOST_CHECK_EQUAL(
        das::cli::call(proto, f, {"-f", "3", "4"}, opts), 7);
      BOOST_CHECK_EQUAL(
        das::cli::call(proto, f, {"6", "-f", "5"}, opts), 11);
    }
    {
      auto const proto = das::named::prototype(foo = 1, bar = 2);
      BOOST_CHECK_EQUAL(das::cli::call(proto, f, {"247"}, opts), 248);
    }
    {
      auto const f =
        [] (std::vector<int> const& ints)
        {
          return std::accumulate(ints.begin(), ints.end(), 0);
        };
      auto const proto = das::named::prototype(bar);
      BOOST_CHECK_EQUAL(
        das::cli::call(proto, f, {"1", "2", "3"}, opts), 6);
      CHECK_THROW(
        das::cli::call(proto, f, {"-b", "1", "2", "3"}, opts),
        das::cli::UnrecognizedValue,
        "extra unrecognized argument: 2");
    }
  }
}

static
void
serialization()
{
  auto const f = [] (elle::Version v) { return v; };
  auto const proto = das::named::prototype(foo);
    BOOST_CHECK_EQUAL(
      das::cli::call(proto, f, {"--foo", "0.1.2"}), elle::Version(0, 1, 2));
}

static
void
help()
{
  BOOST_CHECK_EQUAL(
    elle::sprintf("%s", das::cli::help(das::named::prototype(foo))),
    "      --foo arg           \n");
  BOOST_CHECK_EQUAL(
    elle::sprintf("%s", das::cli::help(das::named::prototype(foo = 42))),
    "      --foo arg            (default: 42)\n");
  BOOST_CHECK_EQUAL(
    elle::sprintf("%s", das::cli::help(das::named::prototype(foo = false))),
    "      --foo               \n");
  BOOST_CHECK_EQUAL(
    elle::sprintf("%s", das::cli::help(das::named::prototype(foo = true))),
    "      --foo               \n");
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(basics));
  {
    auto conversions = BOOST_TEST_SUITE("conversions");
    master.add(conversions);
    using namespace conversions;
    conversions->add(BOOST_TEST_CASE(integers));
    conversions->add(BOOST_TEST_CASE(boolean));
    conversions->add(BOOST_TEST_CASE(multiple_strings));
    conversions->add(BOOST_TEST_CASE(multiple_integers));
  }
  master.add(BOOST_TEST_CASE(defaults));
  {
    auto suite = BOOST_TEST_SUITE("short_options");
    master.add(suite);
    using namespace short_options_ct;
    using namespace short_options_rt;
    suite->add(BOOST_TEST_CASE(compile_time));
    suite->add(BOOST_TEST_CASE(run_time));
  }
  master.add(BOOST_TEST_CASE(dash));
  {
    auto suite = BOOST_TEST_SUITE("positional");
    master.add(suite);
    using namespace positional_ct;
    using namespace positional_rt;
    suite->add(BOOST_TEST_CASE(compile_time));
    suite->add(BOOST_TEST_CASE(run_time));
  }
  master.add(BOOST_TEST_CASE(serialization));
  master.add(BOOST_TEST_CASE(help));
}
