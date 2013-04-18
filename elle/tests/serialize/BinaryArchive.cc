#include <iostream>
#include <sstream>
#include <cassert>


#include "elle/serialize/BinaryArchive.hh"

enum class E: int
{
  v1,
  v2,
  v3,
  v4,
};

enum OldE
{
  OldE_v1,
  OldE_v2,
  OldE_v3,
  OldE_v4,
};

int main()
{
  std::stringstream ss;

  {
    elle::serialize::OutputBinaryArchive ar{ss};

    E e = E::v1;
    ar << e << E::v2 << OldE_v3;
  }

  {
    elle::serialize::InputBinaryArchive ar{ss};
    E e;
    ar >> e;
    assert(e == E::v1);

    ar >> e;
    assert(e == E::v2);

    OldE oe;
    ar >> oe;
    assert(oe == OldE_v3);
  }

  std::cout << std::endl << "tests done." << std::endl;
  return 0;
}
