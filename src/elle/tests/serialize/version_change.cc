
#include <elle/assert.hh>
#include <sstream>
#include <string>

#include <elle/serialize/BinaryArchive.hh>
#include <elle/serialize/BinaryArchive.hxx>


// This is the initial version of the class;
struct Old
{
  std::string s;
  int32_t     i;
};

ELLE_SERIALIZE_STATIC_FORMAT(Old, 0); // not needed

ELLE_SERIALIZE_SIMPLE(Old, ar, value, version)
{
  ELLE_ASSERT(version == 0);
  ar & value.s;
  ar & value.i;
}

// We simulate a new version by subclassing.
struct New : Old
{
  double      d;
};

ELLE_SERIALIZE_STATIC_FORMAT(New, 1);

ELLE_SERIALIZE_SIMPLE(New, ar, value, version)
{
  ar & value.s;
  ar & value.i;
  if (version == 0)
    {
      value.d = 42.0;
    }
  else
    {
      ELLE_ASSERT(version == 1);
      ar & value.d;
    }
}


int main()
{
  Old old{"pif", 12};

  std::stringstream ss;

  elle::serialize::OutputBinaryArchive{ss, old};

  New new_;

  elle::serialize::InputBinaryArchive{ss, new_};

  ELLE_ASSERT(new_.d == 42.0);

  std::cout << "tests done.\n";
  return 0;
}


