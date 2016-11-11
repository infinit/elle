#include <string>

#include <elle/test.hh>

#include <das/model.hh>
#include <das/printer.hh>
#include <das/Symbol.hh>

DAS_SYMBOL(foo);
DAS_SYMBOL(bar);

using das::operator <<;

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
    das::Model<BuiltinModel, elle::meta::List<Symbol_foo, Symbol_bar>>;
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

DAS_MODEL_DEFAULT(
  ExternalModel,
  (das::Model<ExternalModel, elle::meta::List<Symbol_foo, Symbol_bar>>));

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
