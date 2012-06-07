
#include <iostream>
#include <iomanip>
#include <sstream>

#include <elle/serialize/BinaryArchive.hxx>

#include <elle/serialize/Pointer.hh>

struct A
{
  int i;
  short j;
  std::string biet;
  A(int i, short j, std::string const& biet)
    : i(i), j(j), biet(biet)
  {}
   ~A() { std::cout <<"~A()\n"; }
};

ELLE_SERIALIZE_SIMPLE(A, archive, value, version)
{
  assert(version == 0);
  archive & value.i;
  archive & value.j;
  archive & value.biet;
}

ELLE_SERIALIZE_LOAD_CONSTRUCT(A, archive, ptr)
{
  elle::serialize::ClassVersionType version(0);
  int i;
  short j;
  std::string biet;
  archive >> version >> i >> j >> biet;
  assert(version.version == 0);
  new (ptr) A(i, j, biet);
}

int main()
{
  std::stringstream ss;

  {
    A* a = new A{15, 16, "MEGABIET is a string like any other"};
    elle::serialize::OutputBinaryArchive out(ss);
    out << elle::serialize::alive_pointer(a);
    delete a;
  }
  {
    A* a = new A{15, 16, "MEGABIET is a string like any other"};
    elle::serialize::OutputBinaryArchive out(ss);
    out << elle::serialize::pointer(a);
    delete a;
  }

  std::string s = ss.str();
  for(size_t i=0; i < s.size() ; ++i)
    {
      std::cout << std::setw(2) << std::right << std::setfill('0') <<std::hex<< (int)s[i];
      if ((i + 1) % 16 == 0)
        std::cout << std::endl;
      else if (i && (i + 1) % 4 == 0)
        std::cout << ' ';
    }
  std::cout << std::endl;

  {
    A* ptr = nullptr;
    elle::serialize::InputBinaryArchive in(ss);
    in >> elle::serialize::alive_pointer(ptr);
    assert(ptr != nullptr);
    assert(ptr->i == 15);
    assert(ptr->j == 16);
    assert(ptr->biet == "MEGABIET is a string like any other");
    delete ptr;
  }


  {
    A* ptr = nullptr;
    elle::serialize::InputBinaryArchive in(ss);
    in >> elle::serialize::pointer(ptr);
    assert(ptr != nullptr);
    assert(ptr->i == 15);
    assert(ptr->j == 16);
    assert(ptr->biet == "MEGABIET is a string like any other");
    delete ptr;
  }

  std::cout << "tests done.\n";
  return 0;
}
