#include <random>
#include <string>
#include <thread>
#include <vector>

#include <elle/threading/Monitor.hh>
#include <elle/test.hh>

static
int
max_threads()
{
  return RUNNING_ON_VALGRIND ? 8 : 32;
}

static
void
monitor_ctor()
{
  typedef elle::threading::Monitor<std::vector<int>> Vector;
  Vector v1{};
  Vector v2{std::vector<int>(12)};
  BOOST_CHECK_EQUAL(v2->size(), 12);

  Vector v3{{1, 2, 3, 4, 5}};
  BOOST_CHECK_EQUAL(v3->size(), 5);

  std::vector<int> v4(std::vector<int>::size_type{12});
  BOOST_CHECK_EQUAL(v4.size(), 12);

  typedef elle::threading::Monitor<std::vector<std::string>> SVector;
  SVector v5{{"BITE"}};
  BOOST_CHECK_EQUAL(v5->size(), 1);
}

template <typename Callable>
static
void
do_concurrently(Callable callable)
{
  std::vector<std::thread> actors;
  for (int i = 0; i < max_threads(); i++)
    actors.push_back(std::thread{callable});

  for (int i = 0; i < max_threads(); i++)
    actors[i].join();
}

static
void
monitor_operator_arrow()
{
  elle::threading::Monitor<std::vector<int>> vector;

  vector->push_back(212);
  vector->push_back(212);
  vector->push_back(212);
  vector->push_back(212);

  BOOST_CHECK_EQUAL(vector->size(), 4);

  vector->clear();

  auto lambda = [&] {
    for (int i = 0; i < max_threads(); ++i)
    {
      vector->push_back(i);
      vector->pop_back();
    }
  };

  do_concurrently(lambda);

  BOOST_CHECK_EQUAL(vector->size(), 0);
}


static
void
monitor_operator_call()
{
  elle::threading::Monitor<std::string> string;

  string([] (std::string& s) { s = "bite"; });

  string([] (std::string& s) {
      BOOST_CHECK_EQUAL(s, "bite");
  });

  int i = 0;
  do_concurrently([&string, &i] {
      string([&i] (std::string& s) {
        int j = i++;
        s = "bite" + std::to_string(j);
        BOOST_CHECK_EQUAL(s, "bite" + std::to_string(j));
      });
  });
}

static
void
monitor_operator_call_retval()
{
  elle::threading::Monitor<std::vector<float>> v;
  do_concurrently([&v] {
    v->push_back(42.0f);
  });

  for (int i = 0; i < max_threads(); ++i)
    BOOST_CHECK_EQUAL(
        v([i] (std::vector<float>& v) { return v[i]; }),
        42.0f
    );
}

static
void
monitor_const_call()
{
  elle::threading::Monitor<std::string> string;

  string([] (std::string& s) { s = "bite"; });

  auto const& ref = string;

  ref([](std::string const& s) {
      BOOST_CHECK_EQUAL(s, "bite");
  });
}

static
void
monitor_param_scope()
{
  elle::threading::Monitor<std::vector<float>> v;

  std::cerr << RUNNING_ON_VALGRIND << std::endl;
  std::cerr << max_threads() << std::endl;

  std::thread thread1{[&v] () {
      do_concurrently([&v] {
        std::sort(v->begin(), v->end());
      });
  }};

  std::thread thread2{[&v] () {
    do_concurrently([&v] {
      for (int i = 0; i < 1000; ++i)
      {
        v([] (std::vector<float>& vec) {
          if (vec.begin() != vec.end())
            vec.erase(vec.begin());
        });
      }
    });
  }};

  do_concurrently([&v] {
    for (int i = 0; i < 10000; ++i)
      v->push_back(float(std::rand() % 100000) / 1000.0f);
  });

  thread1.join();
  thread2.join();
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(monitor_ctor));
  suite.add(BOOST_TEST_CASE(monitor_operator_arrow));
  suite.add(BOOST_TEST_CASE(monitor_operator_call));
  suite.add(BOOST_TEST_CASE(monitor_operator_call_retval));
  suite.add(BOOST_TEST_CASE(monitor_const_call));
  suite.add(BOOST_TEST_CASE(monitor_param_scope));
}
