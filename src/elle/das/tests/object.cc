#include <sstream>
#include <string>

#include <elle/UUID.hh>
#include <elle/meta.hh>
#include <elle/test.hh>

#include <elle/das/Symbol.hh>
#include <elle/das/flatten.hh>
#include <elle/das/model.hh>
#include <elle/das/printer.hh>

DAS_SYMBOL(name);
DAS_SYMBOL(model);
DAS_SYMBOL(id);

namespace das
{
  // template <typename T, typename Fields_>
  // class Model
  // {
  // public:
  //   using Fields = Fields_;
  // };

  // template <typename T>
  // struct DefaultModel
  // {};

  // namespace
  // {
  //   template <typename O>
  //   struct stringify_object
  //   {
  //     template <typename S>
  //     struct stringify
  //     {
  //       using type = std::string;
  //       static
  //       type
  //       value(O const& o)
  //       {
  //         return elle::sprintf("%s = %s", S::name(), S::attr_get(o));
  //        }
  //     };
  //   };
  // }

  // template <typename T>
  // std::enable_if_exists_t<
  //   typename DefaultModel<T>, std::ostream&>::type
  // operator <<(std::ostream& s, T const& o)
  // {
  //   using Fields = typename DefaultModel<T>::type::Fields;
  //   s << elle::type_info(o) <<
  //     Fields::template map<stringify_object<T>::template stringify>::value(o);
  //   return s;
  // }

  // namespace
  // {
  //   template <typename O>
  //   struct flatten_object
  //   {
  //     template <typename S>
  //     struct flatten
  //     {
  //       using type = typename S::template attr_type<O>::type;
  //       static
  //       type
  //       value(O const& o)
  //       {
  //         return S::attr_get(o);
  //       }
  //     };
  //   };
  // }

  // // Flatten
  // template <typename Model, typename T>
  // typename Model::Fields::template map<flatten_object<T>::template flatten>
  //   ::type::template apply<std::tuple>::type
  // flatten(T const& o)
  // {
  //   using Fields = typename Model::Fields;
  //   return Fields::template map<flatten_object<T>::template flatten>::value(o);
  // }

  // template <typename T>
  // decltype(flatten<typename DefaultModel<T>::type, T>(std::declval<T>()))
  // flatten(T const& o)
  // {
  //   return flatten<typename DefaultModel<T>::type, T>(o);
  // }
}

class Device
{
public:
  std::string name;
  boost::optional<std::string> model;
  elle::UUID id;
  Device(std::string name_,
         boost::optional<std::string> model_ = boost::none,
         elle::UUID id_ = elle::UUID::random())
    : name(std::move(name_))
    , model(std::move(model_))
    , id(std::move(id_))
  {}

  // Device()
  //   : name()
  //   , model()
  //   , id()
  // {}

  bool
  operator ==(Device const& other) const
  {
    return this->id == other.id && this->name == other.name;
  }
};

typedef das::Model<
  Device, elle::meta::List<Symbol_name, Symbol_model, Symbol_id>> DasDevice;

namespace das
{
  template <>
  struct DefaultModel<Device>
  {
  public:
    using type = DasDevice;
  };
}

static
void
printer()
{
  using das::operator <<;
  Device d("name", boost::none, elle::UUID());
  std::stringstream s;
  s << d;
  BOOST_CHECK_EQUAL(
    s.str(),
    "Device(name = name, model = null, id = 00000000-0000-0000-0000-000000000000)");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(printer), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(flatten), 0, valgrind(1));
}
