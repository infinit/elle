#include <elle/serialize/construct.hh>
#include <elle/serialize/Serializer.hh>
#include <elle/serialize/BinaryArchive.hh>

#include <cassert>
#include <iostream>
#include <sstream>

static_assert(elle::serialize::InputBinaryArchive::mode ==
              elle::serialize::ArchiveMode::input, "");

static_assert(elle::serialize::OutputBinaryArchive::mode ==
              elle::serialize::ArchiveMode::output, "");
struct A
{
  int i;
  A(): i(42) {}
  ELLE_SERIALIZE_CONSTRUCT(A) {}

};
ELLE_SERIALIZE_SIMPLE(A, ar, value, version)
{
  enforce(version == 0);
  ar & value.i;
}

struct B:
  public A
{
  float f;
  B(): f(55) {}
  ELLE_SERIALIZE_CONSTRUCT(B, A) {}
};

ELLE_SERIALIZE_SIMPLE(B, ar, value, version)
{
  enforce(version == 0);
  ar & base_class<A>(value);
  ar & value.f;
}

int main()
{
  std::stringstream ss;
  {
    elle::serialize::OutputBinaryArchive out(ss);
    B b;
    assert(b.i == 42);
    assert(b.f == 55);
    out << b;
  }

  {
    elle::serialize::InputBinaryArchive in(ss);
    B b(in);
    assert(b.i == 42);
    assert(b.f == 55);
  }
  std::cout << "tests done.\n";
}

