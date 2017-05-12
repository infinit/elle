#include <elle/das/tuple.hh>

#include <elle/test.hh>

#include <elle/serialization/json.hh>

#include <elle/das/Symbol.hh>
#include <elle/das/serializer.hh>


ELLE_DAS_SYMBOL(foo);
ELLE_DAS_SYMBOL(bar);

static
void
basics()
{
  auto t = elle::das::make_tuple(foo = 3, bar = "lol");
  BOOST_CHECK_EQUAL(t.foo, 3);
  BOOST_CHECK_EQUAL(t.bar, "lol");
}

static
void
move()
{
  elle::das::make_tuple(foo = elle::make_unique<int>(42),
                        bar = std::vector<int>());
  auto f = elle::make_unique<int>(42);
  auto b = std::vector<int>();
  elle::das::make_tuple(foo = std::move(f), bar = b);
}

static
void
print()
{
  auto t = elle::das::make_tuple(foo = 3, bar = "lol");
  BOOST_CHECK_EQUAL(elle::sprintf("%s", t), "(foo = 3, bar = lol)");
}

static
void
serialize()
{
  auto t = elle::das::make_tuple(foo = 3, bar = std::string("lol"));
  auto s = elle::serialization::json::serialize(t);
  auto j = elle::json::read(s.string());
  auto & o = boost::any_cast<elle::json::Object&>(j);
  BOOST_CHECK(boost::any_cast<int64_t>(o["foo"]) == 3);
  BOOST_CHECK(boost::any_cast<std::string>(o["bar"]) == "lol");
  auto d = elle::serialization::json::deserialize<decltype(t)>(s);
  BOOST_CHECK(d.foo == t.foo);
  BOOST_CHECK(d.bar == t.bar);
}

static
void
hash()
{
  using elle::das::make_tuple;
  BOOST_TEST(
    hash_value(elle::das::make_tuple()) ==
    hash_value(elle::das::make_tuple()));
  BOOST_TEST(
    hash_value(elle::das::make_tuple(foo = 3, bar = std::string("lol"))) ==
    hash_value(elle::das::make_tuple(foo = 3, bar = std::string("lol"))));
  BOOST_TEST(
    hash_value(elle::das::make_tuple(foo = 3, bar = std::string("lol"))) !=
    hash_value(elle::das::make_tuple(foo = 4, bar = std::string("lol"))));
  BOOST_TEST(
    hash_value(elle::das::make_tuple(foo = 3, bar = std::string("lol"))) !=
    hash_value(elle::das::make_tuple(foo = 3, bar = std::string("bar"))));
  BOOST_TEST(
    hash_value(elle::das::make_tuple(foo = 3, bar = std::string("lol"))) !=
    hash_value(elle::das::make_tuple(bar = std::string("lol"), foo = 3)));
}

static
void
compare()
{
  BOOST_TEST(
    elle::das::make_tuple() ==
    elle::das::make_tuple());
  BOOST_TEST(
    elle::das::make_tuple(foo = 3, bar = std::string("lol")) ==
    elle::das::make_tuple(foo = 3, bar = std::string("lol")));
  BOOST_TEST(
    elle::das::make_tuple(foo = 3, bar = std::string("lol")) !=
    elle::das::make_tuple(foo = 4, bar = std::string("lol")));
  BOOST_TEST(
    elle::das::make_tuple(foo = 3, bar = std::string("lol")) !=
    elle::das::make_tuple(foo = 3, bar = std::string("bar")));
}

/*-------.
| Driver |
`-------*/

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(basics));
  master.add(BOOST_TEST_CASE(move));
  master.add(BOOST_TEST_CASE(print));
  master.add(BOOST_TEST_CASE(serialize));
  master.add(BOOST_TEST_CASE(hash));
  master.add(BOOST_TEST_CASE(compare));
}
