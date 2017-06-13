#include <elle/das/cli.hh>

#include <numeric>

#include <boost/range/numeric.hpp> // boost::accumulate

#include <elle/Exception.hh>
#include <elle/printf.hh>
#include <elle/test.hh>

ELLE_DAS_SYMBOL(foo);
ELLE_DAS_SYMBOL(bar);
ELLE_DAS_SYMBOL(baz);
ELLE_DAS_SYMBOL(quux);

#define CHECK_THROW(Expression, Exception, Message)     \
  do {                                                  \
    BOOST_CHECK_THROW(Expression, Exception);           \
    try                                                 \
    {                                                   \
      Expression;                                       \
    }                                                   \
    catch (Exception const& e)                          \
    {                                                   \
      BOOST_TEST(elle::sprintf("%s", e) == Message);    \
    }                                                   \
  } while (false)

static
void
basics()
{
  auto const f =
    [] (std::string const& a, std::string const& b) { return a + b; };
  auto const proto = elle::das::named::prototype(foo, baz);
  namespace cli = elle::das::cli;

  BOOST_TEST(cli::call(proto, f, {"--foo", "bar", "--baz", "quux"})
             == "barquux");
  BOOST_TEST(cli::call(proto, f, { "--baz", "quux", "--foo", "bar"})
             == "barquux");
  BOOST_TEST(cli::call(proto, f, {"--foo=bar", "--baz", "quux"})
             == "barquux");
  BOOST_TEST(cli::call(proto, f, {"--foo=bar", "--baz=--quux"})
             == "bar--quux");
  BOOST_TEST(cli::call(proto, f, {"--foo=", "--baz="})
             == "");
  CHECK_THROW(
    cli::call(proto, f, {"--foo", "foo", "--baz"}),
    cli::ValuelessOption,
    "option requires an argument: --baz");
  CHECK_THROW(
    cli::call(proto, f, {"--foo", "bar", "--baz", "x", "--bar", "quux"}),
    cli::UnknownOption,
    "unknown option: --bar");
  CHECK_THROW(
    cli::call(proto, f, {"--foo", "bar", "garbage", "--baz", "quux"}),
    cli::UnrecognizedValue,
    "extra unrecognized argument: garbage");
  CHECK_THROW(
    cli::call(proto, f, {"--foo", "bar"}),
    cli::MissingOption,
    "missing option: --baz");
  // Regression: at some point our algorithm could first look for
  // --baz and its argument, leaving --foo and foo together.  We don't
  // want to support that.
  CHECK_THROW(
    cli::call(proto, f, {"--foo", "--baz", "baz", "foo"}),
    cli::ValuelessOption,
    "option requires an argument: --foo");
  CHECK_THROW(
    cli::call(proto, f, {"--foo", "bar", "--baz", "quux", "--foo", "foo"}),
    cli::DuplicateOption,
    "duplicate option: --foo");
}

namespace conversions
{
  template <typename I>
  void
  check(std::string too_big, std::string too_little)
  {
    using elle::das::cli::call;
    auto constexpr max = std::numeric_limits<I>::max();
    auto constexpr min = std::numeric_limits<I>::min();
    auto const proto = elle::das::named::prototype(foo);
    auto const f = [] (I i)
      {
        return i;
      };
    BOOST_TEST(call(proto, f, {"--foo", std::to_string(max)}) == max);
    BOOST_TEST(call(proto, f, {"--foo", std::to_string(min)}) == min);
    CHECK_THROW(call(proto, f, {"--foo", too_big}),
                elle::das::cli::ValueError,
                "invalid value \"" + too_big +
                "\" for option --foo: integer out of range");
    CHECK_THROW(call(proto, f, {"--foo", too_little}),
                elle::das::cli::ValueError,
                "invalid value \"" + too_little +
                "\" for option --foo: integer out of range");
  }

  template <typename I>
  void
  check()
  {
    using Max = std::conditional_t<std::is_signed<I>::value,
                                   int64_t, uint64_t>;
    using Min = int64_t;
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
      auto const proto = elle::das::named::prototype(foo);
      {
        auto const f = [] (int i) {return i;};
        CHECK_THROW(
          elle::das::cli::call(proto, f, {"--foo", "lol"}),
          elle::das::cli::ValueError,
          "invalid value \"lol\" for option --foo: invalid integer");
      }
      // Verify we check for trailing garbage.
      {
        auto const f = [] (unsigned i) {return i;};
        CHECK_THROW(
          elle::das::cli::call(
            proto, f, {"--foo", "007 james bond"}), elle::das::cli::ValueError,
          "invalid value \"007 james bond\" for option --foo: invalid integer");
      }
      {
        auto const f = [] (signed i) {return i;};
        CHECK_THROW(
          elle::das::cli::call(
            proto, f, {"--foo", "-666numberofthebeast"}), elle::das::cli::ValueError,
          "invalid value \"-666numberofthebeast\" for option --foo: invalid integer");
      }
    }
  }

  static
  void
  multiple_strings()
  {
    using elle::das::cli::call;
    auto const f = [] (std::vector<std::string> const& strings)
      {
        return boost::accumulate(
          strings, std::string(""),
          [] (std::string const& a, std::string const& b)
          { return a + "-" + b; });
      };
    auto const proto = elle::das::named::prototype(foo);
    BOOST_TEST(call(proto, f, {}) == "");
    BOOST_TEST(call(proto, f, {"--foo", "foo"}) == "-foo");
    BOOST_TEST(call(proto, f, {"--foo", "foo", "--foo", "bar"}) == "-foo-bar");
  }

  static
  void
  boolean()
  {
    using elle::das::cli::call;
    {
      auto const f = [] (int expected, bool enabled)
        {
          BOOST_TEST(bool(expected) == enabled);
        };
      auto const named = elle::das::named::function(f, foo, bar = false);
      call(named, {"--foo", "0"});
      call(named, {"--foo", "1", "--bar"});
      call(named, {"--bar", "--foo", "1"});
      call(named, {"--foo", "1", "--bar", "true"});
      call(named, {"--foo", "0", "--bar", "false"});
    }
    {
      auto const f = [] (bool b)
        {
          BOOST_CHECK(!b);
        };
      call(elle::das::named::function(f, foo), {});
    }
    {
      auto const f = [] (bool foo, int bar)
        {
          return elle::sprintf("%s %s", foo, bar);
        };
      auto const named = elle::das::named::function(f, foo = false, bar = 20);
      BOOST_TEST(call(named, {})                               == "false 20");
      BOOST_TEST(call(named, {"--foo", "true", "--bar", "33"}) == "true 33");
      BOOST_TEST(call(named, {"--bar", "33"})                  == "false 33");
      BOOST_TEST(call(named, {"--foo", "--bar", "33"})         == "true 33");
      CHECK_THROW(call(named, {"--foo", "--bar", "33", "--BAD"}),
                  elle::das::cli::UnknownOption,
                  "unknown option: --BAD");
      // Because of the way we parse the options and their argument,
      // we may extract "--bar 33" first, leaving "--foo BAD1 BAD2",
      // which results in a complaint about BAD1 not being valid for
      // --foo, instead of being an extra argument.
      //
      // FIXME: Of course this pairing of --foo with BAD1 is a
      // problem.
      try
      {
        call(named, {"--foo", "--bar", "33", "BAD1", "BAD2"});
        BOOST_FAIL("did not raise an exception");
      }
      catch (elle::das::cli::UnrecognizedValue const& e)
      {
        BOOST_TEST(elle::sprintf("%s", e)
                   == "extra unrecognized argument: BAD1");
      }
      catch (elle::das::cli::OptionValueError const& e)
      {
        BOOST_TEST(elle::sprintf("%s", e)
                   == "invalid value \"BAD1\" for option --foo: invalid boolean");
      }
      catch (...)
      {
        BOOST_FAIL("unexpected exception: " + elle::exception_string());
      }
    }
  }

  static
  void
  multiple_integers()
  {
    using elle::das::cli::call;
    auto const f = [] (std::vector<int> ints) { return ints; };
    auto const proto = elle::das::named::prototype(foo);
    BOOST_TEST(call(proto, f, {}) == std::vector<int>{});
    BOOST_TEST(call(proto, f, {"--foo", "2"}) == std::vector<int>{2});
    BOOST_TEST(call(proto, f, {"--foo", "2", "--foo", "3"})
               == std::vector<int>({2, 3}));
  }
}

static
void
defaults()
{
  using elle::das::cli::call;
  auto const f =
    [] (std::string const& foo, int baz) { return foo + std::to_string(baz); };
  auto const proto = elle::das::named::prototype(foo = "16", baz = 42);
  BOOST_TEST(call(proto, f, {}) == "1642");
  BOOST_TEST(call(proto, f, {"--baz", "64"}) == "1664");
  BOOST_TEST(call(proto, f, {"--foo", "51"}) == "5142");
  BOOST_TEST(call(proto, f, {"--baz", "23", "--foo", "01"}) == "0123");
}

static
void
defaulted()
{
  using elle::das::cli::call;
  auto const f = elle::das::named::function(
    [] (elle::Defaulted<bool> b) { return std::make_pair(bool(b), b.get()); },
    foo = elle::defaulted(false));
  BOOST_TEST(call(f, {})        == std::make_pair(false, false));
  BOOST_TEST(call(f, {"--foo"}) == std::make_pair(true, true));
}

static
void
flag()
{
  auto const f =
    [] (std::string const& foo, int bar) { return foo + std::to_string(bar); };
  auto const proto = elle::das::named::prototype(foo, bar);
  // Beware that the order of parsing options and of conversion of
  // their argument values in undefined.  So be sure to have a single
  // type of error, to avoid error messages that differ depending on
  // the platform.
  namespace cli = elle::das::cli;
  BOOST_CHECK_THROW(cli::call(proto, f, {"--foo", "--bar", "12"}),
                    cli::ValuelessOption);
  BOOST_CHECK_THROW(cli::call(proto, f, {"--foo", "foo", "--bar"}),
                    cli::ValuelessOption);
  BOOST_CHECK_THROW(cli::call(proto, f, {"--foo", "foo", "--bar", "bar"}),
                    cli::OptionValueError);
}

ELLE_DAS_SYMBOL(composite_option);

static
void
dash()
{
  auto const f = [] (int foo) { return foo; };
  auto const proto = elle::das::named::prototype(composite_option = 0);
  {
    BOOST_CHECK_EQUAL(
      elle::das::cli::call(proto, f, {"--composite-option", "193"}), 193);
    CHECK_THROW(
      elle::das::cli::call(proto, f, {"--composite_option", "193"}),
      elle::das::cli::UnknownOption,
      "unknown option: --composite_option");
  }
}

namespace short_options_ct
{
  ELLE_DAS_CLI_SYMBOL(foo, 'f', "", false);
  ELLE_DAS_CLI_SYMBOL(bar, 'b', "", false);

  static
  void
  compile_time()
  {
    using elle::das::cli::call;
    auto const f =
      [] (int foo, int bar) { return foo + bar; };
    auto const proto = elle::das::named::prototype(foo, bar);
    BOOST_CHECK_EQUAL(call(proto, f, {"--foo", "1", "-b", "2"}), 3);
    BOOST_CHECK_EQUAL(call(proto, f, {"-f", "3", "--bar", "4"}), 7);
    BOOST_CHECK_EQUAL(call(proto, f, {"-f", "5", "-b", "6"}), 11);
  }
}

namespace short_options_rt
{
  static
  void
  run_time()
  {
    using elle::das::cli::call;
    auto const f =
      [] (int foo, int bar) { return foo + bar; };
    auto const proto = elle::das::named::prototype(foo, bar);
    {
      elle::das::cli::Options opts = {
        {"foo", {'f', ""}},
        {"bar", {'b', ""}},
      };
      BOOST_CHECK_EQUAL(call(proto, f, {"--foo", "1", "-b", "2"}, opts), 3);
      BOOST_CHECK_EQUAL(call(proto, f, {"-f", "3", "--bar", "4"}, opts), 7);
      BOOST_CHECK_EQUAL(call(proto, f, {"-f", "5", "-b", "6"}, opts), 11);
    }
  }
}

namespace positional_ct
{
  ELLE_DAS_CLI_SYMBOL(foo, 'f', "", false);
  ELLE_DAS_CLI_SYMBOL(bar, 'b', "", true);
  ELLE_DAS_CLI_SYMBOL(quux, 'q', "", true);

  static
  void
  compile_time()
  {
    using elle::das::cli::call;
    auto const f =
      [] (int foo, int bar) { return foo + bar; };
    {
      auto const proto = elle::das::named::prototype(foo, bar);
      BOOST_TEST(call(proto, f, {"-f", "1", "-b", "2"}) == 3);
      BOOST_TEST(call(proto, f, {"-f", "3", "4"}) == 7);
      BOOST_TEST(call(proto, f, {"6", "-f", "5"}) == 11);
    }
    {
      auto const proto = elle::das::named::prototype(foo = 1, bar = 2);
      BOOST_TEST(call(proto, f, {"247"}) == 248);
    }
    {
      auto const f =
        [] (std::vector<int> const& ints)
        {
          return boost::accumulate(ints, 0);
        };
      auto const proto = elle::das::named::prototype(bar);
      BOOST_TEST(call(proto, f, {"1", "2", "3"}) == 6);
      CHECK_THROW(call(proto, f, {"-b", "1", "2", "3"}),
                  elle::das::cli::UnrecognizedValue,
                  "extra unrecognized argument: 2");
    }
    {
      auto const f = elle::das::named::function(
        [] (int b, int q)
        {
          return b - q;
        }, bar, quux);
      BOOST_TEST(call(f, {"-b", "1", "-q", "2"}) == -1);
      // Not handled yet.
      // BOOST_CHECK_EQUAL(call(f, {"1", "2"}), -1);
    }
  }
}

namespace positional_rt
{
  static
  void
  run_time()
  {
    using elle::das::cli::call;
    auto const f =
      [] (int foo, int bar) { return foo + bar; };
    elle::das::cli::Options opts = {
      {"foo", {'f', "", false}},
      {"bar", {'b', "", true}},
    };
    {
      auto const proto = elle::das::named::prototype(foo, bar);
      BOOST_CHECK_EQUAL(call(proto, f, {"-f", "1", "-b", "2"}, opts), 3);
      BOOST_CHECK_EQUAL(call(proto, f, {"-f", "3", "4"}, opts), 7);
      BOOST_CHECK_EQUAL(call(proto, f, {"6", "-f", "5"}, opts), 11);
    }
    {
      auto const proto = elle::das::named::prototype(foo = 1, bar = 2);
      BOOST_CHECK_EQUAL(call(proto, f, {"247"}, opts), 248);
    }
    {
      auto const f =
        [] (std::vector<int> const& ints)
        {
          return boost::accumulate(ints, 0);
        };
      auto const proto = elle::das::named::prototype(bar);
      BOOST_CHECK_EQUAL(call(proto, f, {"1", "2", "3"}, opts), 6);
      CHECK_THROW(call(proto, f, {"-b", "1", "2", "3"}, opts),
                  elle::das::cli::UnrecognizedValue,
                  "extra unrecognized argument: 2");
    }
  }
}

static
void
serialization()
{
  using elle::das::cli::call;
  auto const f = [] (elle::Version v) { return v; };
  auto const proto = elle::das::named::prototype(foo);
    BOOST_CHECK_EQUAL(call(proto, f, {"--foo", "0.1.2"}),
      elle::Version(0, 1, 2));
}

static
void
help()
{
  auto const help = [](auto&&... args)
    {
      return elle::sprintf("%s",
        elle::das::cli::help(
          elle::das::named::prototype(
            std::forward<decltype(args)>(args)...)));
    };
  BOOST_TEST(help(foo)         == "      --foo arg           \n");
  BOOST_TEST(help(foo = 42)    == "      --foo arg            (default: 42)\n");
  BOOST_TEST(help(foo = false) == "      --foo               \n");
  BOOST_TEST(help(foo = true)  == "      --foo               \n");
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
  master.add(BOOST_TEST_CASE(defaulted));
  master.add(BOOST_TEST_CASE(flag));
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
