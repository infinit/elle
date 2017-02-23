# include <string>

# include <elle/das/named.hh>
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
  {}

  Movable(Movable const& m)
    : copied(true)
    , moved(false)
  {
    m.copied = true;
  }

  Movable(Movable&& m)
    : copied(false)
    , moved(true)
  {
    m.moved = true;
  }

  mutable bool copied, moved;
};

static
void
_forwarding_ref(Neither& arg1, Neither const& arg2)
{
  arg1.mutate();
  arg2.observe();
}
NAMED_FUNCTION(forwarding_ref, _forwarding_ref, arg1, arg2);

static
void
_forwarding_value(Movable)
{}
NAMED_FUNCTION(forwarding_value, _forwarding_value, arg1);

static
void
forwarding()
{
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
      elle::das::named::function(_forwarding_ref,
                                 arg1 = neither, arg2 = neither);
    f();
    f(arg1 = neither);
    f(arg2 = neither);
    f(arg1 = neither, arg2 = neither);
  }
  {
    Movable movable_def;
    auto f = elle::das::named::function([] (Movable m) { return m.moved; },
                                        arg1 = std::move(movable_def));
    BOOST_CHECK(movable_def.moved);
    BOOST_CHECK(!f());
    Movable movable;
    BOOST_CHECK(f(std::move(movable)));
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
}
