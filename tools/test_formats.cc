#include <lune/Authority.hh>
#include <lune/Descriptor.hh>
#include <lune/Identity.hh>

#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Subject.hh>
#include <nucleus/neutron/Record.hh>

#include <elle/serialize/BinaryArchive.hh>
#include <elle/serialize/Base64Archive.hh>
#include <elle/serialize/VectorSerializer.hxx>
#include <elle/serialize/PairSerializer.hxx>

#include <iostream>
#include <fstream>

lune::Authority authorities[elle::serialize::StaticFormat<lune::Authority>::version];

static
void
check_instance(lune::Authority const& value)
{
  //authorities[value.version()];

  assert(value.cipher != nullptr);
}

static
void
check_instance(lune::Identity const& value)
{

}

static
void
check_instance(nucleus::neutron::Group const& value)
{

}

static
void
check_instance(nucleus::neutron::Subject const& value)
{

}

static
void
check_instance(nucleus::neutron::Access const& value)
{

}

static
void
check_instance(nucleus::neutron::Object const& value)
{

}



template <typename T>
void
check_class(std::string const& name)
{
  std::string filename = name + ".txt";
  static unsigned int const format = elle::serialize::StaticFormat<T>::version;

  std::vector<std::pair<uint32_t, std::string>> lines;

  {
    std::ifstream in(filename);
    if (in.good())
      {
        elle::serialize::InputBinaryArchive(in) >> lines;
        std::cout << "Loaded " << lines.size()
                  << " format of class " << name << std::endl;
      }
    else
      throw std::runtime_error{
          "cannot load format references for class '" + name + "'"
      };
  }

  for (auto const& pair : lines)
    {
      assert(pair.first <= format &&
             "reference format is greater than current one !");

      std::cout << "Check " << name << " with format = "
                << pair.first << std::endl;

      std::istringstream ss(pair.second);
      T val;
      elle::serialize::InputBase64Archive(ss) >> val;

      check_instance(val);

    }
}

#define CHECK_CLASS(__type) \
  check_class<__type>(#__type) \
/**/

int main()
{
  CHECK_CLASS(lune::Authority);
  CHECK_CLASS(lune::Identity);
  CHECK_CLASS(nucleus::neutron::Group);
  CHECK_CLASS(nucleus::neutron::Subject);
  CHECK_CLASS(nucleus::neutron::Access);
  CHECK_CLASS(nucleus::neutron::Object);

  return 0;
}
