# include <elle/meta.hh>
# include <elle/test.hh>

using namespace elle::meta;

namespace list
{
  namespace apply
  {
    template <typename ... Args>
    struct res
    {};

    typedef List<int, float, double> l;
    static_assert(std::is_same<typename l::apply<res>::type,
                               res<int, float, double>>::value,
                  "list::apply yielded the wrong type");
  }

  namespace filter
  {
    typedef List<int, std::string, void, float*> l;
    typedef typename l::filter<std::is_scalar>::type res;
    static_assert(std::is_same<res, List<int, float*>>::value,
                  "list::filter yielded the wrong type");
  }

  namespace index_of
  {
    typedef List<int, float, double> l;
    static_assert(l::index_of<int>::value == 0,
                  "list::index_of yielded the wrong index");
    static_assert(l::index_of<float>::value == 1,
                  "list::index_of yielded the wrong index");
    static_assert(l::index_of<double>::value == 2,
                  "list::index_of yielded the wrong index");
  }

  namespace map
  {
    typedef List<int, float, std::string> l;
    template <typename T>
    struct voidify
    {
      using type = void;
    };
    static_assert(
      std::is_same<l::map<voidify>::type, List<void, void, void>>::value,
      "list::map yielded the wrong type");
    template <typename T>
    struct pointerize
    {
      using type = T*;
    };
    static_assert(
      std::is_same<l::map<pointerize>::type,
                   List<int*, float*, std::string*>>::value,
      "list::map yielded the wrong type");
  }

  namespace prepend
  {
    typedef List<int, void> l;
    static_assert(std::is_same<l::prepend<float>::type,
                               List<float, int, void>>::value,
                  "list::prepend yielded the wrong type");
  }

  namespace tail
  {
    typedef List<int, float, void> l;
    static_assert(
      std::is_same<l::tail<>::type, List<float, void>>::value,
      "list::tail yielded the wrong type");
  }
}

template <typename T>
struct print_type
{
  using type = std::string;
  static
  type
  value(std::string const& prefix, std::string const& suffix)
  {
    return prefix + elle::type_info<T>().name() + suffix;
  }
};

static
void
map()
{
  using l = List<int, float, char>;
  using map = l::map<print_type>;
  static_assert(
    std::is_same<map::type, List<std::string, std::string, std::string>>::value,
    "list::map yielded the wrong type");
  static_assert(elle::meta::map_runtime<print_type, int>(0), "blerg");
  BOOST_CHECK_EQUAL(l::map<print_type>::value("<", ">"),
                    std::make_tuple("<int>", "<float>", "<char>"));
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(map));
}
