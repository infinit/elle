#include <string>

#include <elle/test.hh>

#include <elle/das/model.hh>
#include <elle/das/printer.hh>
#include <elle/das/Symbol.hh>

ELLE_DAS_SYMBOL(foo);
ELLE_DAS_SYMBOL(bar);

using elle::das::operator <<;

struct NoModel
{
  int foo, bar;
};

void
no_model()
{
  NoModel o;
  BOOST_CHECK_EQUAL(elle::sprintf("%s", o),
                    elle::sprintf("NoModel(%x)", (void*)&o));
}

struct BuiltinModel
{
  int foo, bar;
  using Model =
    elle::das::Model<BuiltinModel, elle::meta::List<Symbol_foo, Symbol_bar>>;
};

void
builtin_model()
{
  BuiltinModel o{1, 2};
  BOOST_CHECK_EQUAL(elle::sprintf("%s", o), "BuiltinModel(foo = 1, bar = 2)");
}

struct ExternalModel
{
  int foo, bar;
};

ELLE_DAS_MODEL_DEFAULT(
  ExternalModel,
  (elle::das::Model<ExternalModel, elle::meta::List<Symbol_foo, Symbol_bar>>));

void
external_model()
{
  ExternalModel o{3, 4};
  BOOST_CHECK_EQUAL(elle::sprintf("%s", o), "ExternalModel(foo = 3, bar = 4)");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(no_model), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(builtin_model), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(external_model), 0, valgrind(1));
}
