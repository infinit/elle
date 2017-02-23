#include <sstream>
#include <iostream>
#include <algorithm>
#include <typeinfo>

#include <elle/serialize/Base64Archive.hh>
#include <elle/serialize/ArraySerializer.hxx>
#include <elle/serialize/ListSerializer.hxx>
#include <elle/serialize/VectorSerializer.hxx>
#include <elle/serialize/SetSerializer.hxx>
#include <elle/serialize/MapSerializer.hxx>

namespace se = elle::serialize;

template<typename Container>
  void test_container(Container const& value)
  {
    std::stringstream ss;

      {
        se::OutputBase64Archive ar(ss);
        ar & value;
      }

      {
        se::InputBase64Archive ar(ss);
        Container out;
        ar & out;
        assert(std::equal(
            std::begin(value), std::end(value),
            std::begin(out)
        ));
      }
    std::cout << __PRETTY_FUNCTION__
              << ": " << ss.str() << std::endl;
  }

static
void
test_stl()
{
  double tab[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6};
  test_container(tab);
  test_container(std::array<double, 6>{{0.1, 0.2, 0.3, 0.4, 0.5, 0.6}});
  test_container(std::list<double>{{0.1, 0.2, 0.3, 0.4, 0.5, 0.6}});
  test_container(std::vector<double>{{0.1, 0.2, 0.3, 0.4, 0.5, 0.6}});
  test_container(std::set<double>{{0.1, 0.2, 0.3, 0.4, 0.5, 0.6}});
  test_container(
      std::map<std::string, double>{
          {"salut", 42.0},
          {"tata", 42.0},
          {"toto", 42.0},
          {"pif", 42.0},
          {"paf", 42.0},
      }
  );
  test_container(
      std::map<std::string, std::vector<double>>{
          {"salut", {{1,2,3,4,5}}},
          {"tata",  {{1,2,3,4,5}}},
          {"toto",  {{1,2,3,4,5}}},
          {"pif",   {{1,2,3,4,5}}},
          {"paf",   {{1,2,3,4,5}}},
      }
  );
}

int main()
{
  test_stl();
  std::cout << "tests done.\n";
  return 0;
}
