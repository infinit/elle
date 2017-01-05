# include <string>

# include <elle/named.hh>
# include <elle/test.hh>

/*------.
| Basic |
`------*/

NAMED_ARGUMENT(versioned);
NAMED_ARGUMENT(name);

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
  prototype(name, versioned).call(_basic_f, std::forward<Args>(args)...);
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

// #define TEST(R, Name, ...)
//   static
//   R
//   _##Name(__VA_ARGS__);
//
//   template <typename ... Args>
//   R
//   Name(Args&& ... args)
//   {
//     return elle::named::prototype(versioned).call(
//       _##Name, std::forward<Args>(args)...);
//   }
//
//   static
//   R
//   _##Name(__VA_ARGS__)
//
// TEST(bool, opposite, bool versioned)
// {
//   return !versioned;
// }

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

NAMED_ARGUMENT(arg1);
NAMED_ARGUMENT(arg2);

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

  Movable(Movable& m)
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

  bool copied, moved;
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
NAMED_FUNCTION(positional_f, _positional_f, arg1, arg2);

static
void
positional()
{
  std::string os;
  bool ob;
  positional_f(os, ob, arg1 = "named", arg2 = true);
  BOOST_CHECK(os == "namednamed");
  BOOST_CHECK(!ob);
}


/*--------------.
| Default value |
`--------------*/

NAMED_ARGUMENT(out1);
NAMED_ARGUMENT(out2);

static
void
_default_f(bool& o1, bool& o2, bool i1, bool i2, Neither const&)
{
  o1 = i1;
  o2 = i2;
}
NAMED_FUNCTION(default_f, _default_f, out1, out2,
               arg1 = true, arg2 = false, versioned = Neither());

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
