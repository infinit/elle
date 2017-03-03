# include <string>

# include <elle/das/named.hh>
# include <elle/serialization/binary.hh>
# include <elle/serialization/json.hh>
# include <elle/test.hh>

#define NAMED_FUNCTION(Name, F, ...)                                    \
  template <typename ... Args>                                          \
  auto                                                                  \
  Name(Args&& ... args) ->                                              \
    decltype(elle::das::named::prototype(__VA_ARGS__).call(                   \
               F, std::forward<Args>(args)...))                         \
  {                                                                     \
    return elle::das::named::prototype(__VA_ARGS__).call(                     \
      F, std::forward<Args>(args)...);                                  \
  }                                                                     \

/*------.
| Basic |
`------*/

ELLE_DAS_SYMBOL(versioned);
ELLE_DAS_SYMBOL(name);

static
void
_basic_f(std::string name, bool versioned)
{
  BOOST_CHECK_EQUAL(name, "dat name");
  BOOST_CHECK(!versioned);
}

template <typename ... Args>
static
void
basic_f(Args&& ... args)
{
  elle::das::named::prototype(name, versioned).
    call(_basic_f, std::forward<Args>(args)...);
}

static
void
basic()
{
  basic_f(versioned = false, name = "dat name");
  basic_f(name = "dat name", versioned = false);
}

/*-------------.
| Return value |
`-------------*/

static
bool
_nullary()
{
  return true;
}
NAMED_FUNCTION(nullary, _nullary);

static
bool
_opposite(bool versioned)
{
  return !versioned;
}
NAMED_FUNCTION(opposite, _opposite, versioned);

static
void
return_value()
{
  BOOST_CHECK(nullary());
  BOOST_CHECK(!opposite(versioned = true));
  BOOST_CHECK(opposite(versioned = false));
}

/*-----------.
| Forwarding |
`-----------*/

ELLE_DAS_SYMBOL(arg1);
ELLE_DAS_SYMBOL(arg2);
ELLE_DAS_SYMBOL(arg3);
ELLE_DAS_SYMBOL(arg4);

struct Neither
{
  Neither()
  {}
  Neither(Neither const&) = delete;
  Neither(Neither&&) = delete;
  void
  mutate()
  {}
  void
  observe() const
  {}
};

struct Movable
{
  Movable()
    : copied(false)
    , moved(false)
    , by_copy(false)
    , by_move(false)
    , dont_copy(false)
  {}

  Movable(Movable const& m)
    : copied(false)
    , moved(false)
    , by_copy(true)
    , by_move(false)
    , dont_copy(m.dont_copy)
  {
    BOOST_CHECK(!m.dont_copy);
    m.copied = true;
  }

  Movable(Movable&& m)
    : copied(false)
    , moved(false)
    , by_copy(false)
    , by_move(true)
    , dont_copy(m.dont_copy)
  {
    BOOST_CHECK(!m.moved);
    m.moved = true;
  }

  mutable bool copied, moved, by_copy, by_move;
  bool dont_copy;
};

static
void
forwarding()
{
  auto const forwarding_ref = elle::das::named::function(
    [] (Neither& arg1, Neither const& arg2)
    {
      arg1.mutate();
      arg2.observe();
    },
    arg1,
    arg2);
  auto const forwarding_value = elle::das::named::function(
    [] (Movable)
    {},
    arg1);
  {
    Neither neither;
    forwarding_ref(arg1 = neither, arg2 = neither);
    forwarding_ref(arg2 = neither, arg1 = neither);
  }
  {
    Movable movable;
    forwarding_value(arg1 = movable);
    BOOST_CHECK(movable.copied);
    BOOST_CHECK(!movable.moved);
  }
  {
    Movable movable;
    forwarding_value(arg1 = std::move(movable));
    BOOST_CHECK(!movable.copied);
    BOOST_CHECK(movable.moved);
  }
  {
    Neither neither;
    auto f =
      elle::das::named::function(
        [] (Neither& arg1, Neither const& arg2)
        {
          arg1.mutate();
          arg2.observe();
        },
        arg1 = neither,
        arg2 = neither);
    f();
    f(arg1 = neither);
    f(arg2 = neither);
    f(arg1 = neither, arg2 = neither);
  }
  {
    Movable movable_def;
    auto f = elle::das::named::function([] (Movable m) {},
                                        arg1 = std::move(movable_def));
    std::cerr << elle::type_info(f) << std::endl;
    BOOST_CHECK(movable_def.moved);
    f();
    f();
    Movable movable;
    movable.dont_copy = true;
    f(std::move(movable));
    BOOST_CHECK(!movable.copied);
    BOOST_CHECK(movable.moved);
  }
}

/*-----------.
| Positional |
`-----------*/

static
void
_positional_f(std::string& os, bool& ob, std::string is, bool ib)
{
  os = is + is;
  ob = !ib;
}
NAMED_FUNCTION(positional_f, _positional_f, arg1, arg2, arg3, arg4);

static
void
positional()
{
  std::string os;
  bool ob;
  positional_f(os, ob, arg3 = "named", arg4 = true);
  BOOST_CHECK(os == "namednamed");
  BOOST_CHECK(!ob);
}


/*--------------.
| Default value |
`--------------*/

ELLE_DAS_SYMBOL(out1);
ELLE_DAS_SYMBOL(out2);

static
void
_default_f(bool& o1, bool& o2, bool i1, bool i2, Neither const&)
{
  o1 = i1;
  o2 = i2;
}

static Neither const neither;
NAMED_FUNCTION(default_f, _default_f, out1, out2,
               arg1 = true, arg2 = false, versioned = neither);

static
void
default_value()
{
  bool a1 = true, a2 = false;
  default_f(out1 = a1, out2 = a2, arg1 = false, arg2 = true);
  BOOST_CHECK(!a1);
  BOOST_CHECK(a2);
  default_f(out1 = a1, out2 = a2);
  BOOST_CHECK(a1);
  BOOST_CHECK(!a2);
}

/*-------------------.
| Default positional |
`-------------------*/

static
void
_default_positional_f(bool arg1, bool arg2)
{
  BOOST_CHECK_EQUAL(arg1, arg2);
}
NAMED_FUNCTION(default_positional_f, _default_positional_f,
               arg1 = true, arg2 = false);

static
void
default_positional()
{
  default_positional_f(false);
}

/*-----.
| Call |
`-----*/

ELLE_DAS_SYMBOL(foo);
ELLE_DAS_SYMBOL(bar);

template <typename S>
static
void
call()
{
  auto f = elle::das::named::function(
    [] (int x, std::string const& s)
    {
      if (x < 0)
        elle::err("negative");
      return std::to_string(x) + " = " + s;
    },
    foo, bar);
  {
    auto call = f.call(42, "forty two");
    BOOST_CHECK_EQUAL(f(call)(), "42 = forty two");
    auto result = [&]
    {
      auto serialized = elle::serialization::serialize<S>(call, false);
      return f(elle::serialization::deserialize<S, typename decltype(f)::Call>(
                 serialized, false));
    }();
    BOOST_CHECK_EQUAL(result(), "42 = forty two");
    {
      auto serialized = elle::serialization::serialize<S>(result, false);
      BOOST_CHECK_EQUAL(
        (elle::serialization::deserialize<S, typename decltype(f)::Result>(
          serialized, false)()),
        "42 = forty two");
    }
  }
  {
    auto call = f.call(-1, "one");
    BOOST_CHECK_THROW(f(call)(), elle::Error);
    auto result = [&]
    {
      auto serialized = elle::serialization::serialize<S>(call, false);
      return f(elle::serialization::deserialize<S, typename decltype(f)::Call>(
                 serialized, false));
    }();
    BOOST_CHECK_THROW(result(), elle::Error);
    {
      auto serialized = elle::serialization::serialize<S>(result, false);
      BOOST_CHECK_THROW(
        (elle::serialization::deserialize<S, typename decltype(f)::Result>(
          serialized, false)()),
        elle::Error);
    }
  }
}

/*----.
| Map |
`----*/

static
void
map()
{
  auto p = elle::das::named::prototype(foo = boost::none);
  BOOST_CHECK(!p.map<boost::optional<int>>().foo);
  BOOST_CHECK_EQUAL(p.map<boost::optional<int>>(42).foo, 42);
}

/*-------.
| Driver |
`-------*/

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(basic));
  master.add(BOOST_TEST_CASE(return_value));
  master.add(BOOST_TEST_CASE(forwarding));
  master.add(BOOST_TEST_CASE(positional));
  master.add(BOOST_TEST_CASE(default_value));
  master.add(BOOST_TEST_CASE(default_positional));
  master.add(BOOST_TEST_CASE(call<elle::serialization::Json>));
  master.add(BOOST_TEST_CASE(call<elle::serialization::Binary>));
  master.add(BOOST_TEST_CASE(map));
}
