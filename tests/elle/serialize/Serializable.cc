
#include <elle/serialize/BinaryArchive.hh>
#include <elle/serialize/Serializable.hh>
#include <elle/serialize/Polymorphic.hh>

#include <sstream>

struct A:
  elle::serialize::Serializable<>
{};

struct B:
  A,
  elle::serialize::SerializableMixin<B>
{ int i; };

ELLE_SERIALIZE_SIMPLE(B, archive, value, version)
{
  (void) version;
  archive & value.i;
}

int main()
{
  B b;
  b.i = 43;


    {
      std::stringstream ss;
        {
          elle::serialize::OutputBinaryArchive out(ss);
          A& a = b;
          out & a;
        }
        {
          B cpy;
          A& a = cpy;
          elle::serialize::InputBinaryArchive in(ss);
          in & a;
          assert(cpy.i == 43);
        }
    }

  std::cout << "tests done.\n";
}

