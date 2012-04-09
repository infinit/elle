
#include <sstream>
#include <iostream>

#include <elle/serialize/Base64Archive.hh>
#include <elle/serialize/ArraySerializer.hxx>
#include <elle/serialize/ListSerializer.hxx>
#include <elle/serialize/VectorSerializer.hxx>
#include <elle/serialize/SetSerializer.hxx>

namespace se = elle::serialize;

int main()
{
  std::stringstream ss;

    {
      se::OutputBase64Archive ar(ss);

      int tab[3] = {1,2,3};
      ar & tab;

      std::array<float, 6> ftab{{0.1, 0.2, 0.3, 0.4, 0.5, 0.6}};
      ar & ftab;

      std::list<float> lst{{0.1, 0.2, 0.3, 0.4, 0.5, 0.6}};
      ar & lst;

      std::vector<double> vec{{0.1, 0.2, 0.3, 0.4, 0.5, 0.6}};
      ar & vec;

      std::set<double> set{{0.1, 0.2, 0.3, 0.4, 0.5, 0.6}};
      ar & set;
    }
  std::cout << ss.str() << std::endl;

    {
      se::InputBase64Archive ar(ss);

      int tab[3];
      ar & tab;
      assert(tab[0] == 1);
      assert(tab[1] == 2);
      assert(tab[2] == 3);

      std::array<float, 6> ftab;
      ar & ftab;
      assert((ftab == std::array<float, 6>{{0.1, 0.2, 0.3, 0.4, 0.5, 0.6}}));

      std::list<float> lst;
      ar & lst;
      assert((lst == std::list<float>{{0.1, 0.2, 0.3, 0.4, 0.5, 0.6}}));

      std::vector<double> vec;
      ar & vec;
      assert((vec == std::vector<double>{{0.1, 0.2, 0.3, 0.4, 0.5, 0.6}}));

      std::set<double> set;
      ar & set;
      assert((set == std::set<double>{{0.1, 0.2, 0.3, 0.4, 0.5, 0.6}}));

    }

  std::cout << "tests done.\n";
  return 0;
}



