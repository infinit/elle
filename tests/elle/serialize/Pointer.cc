#include <elle/assert.hh>
#include <elle/serialize/BinaryArchive.hxx>
#include <elle/serialize/Pointer.hh>

#include <iostream>
#include <iomanip>
#include <sstream>

struct A
{
  int i;
  short j;
  std::string biet;
  A() {}
  A(int i, short j, std::string const& biet)
    : i(i), j(j), biet(biet)
  {}
   ~A() { std::cout <<"~A()\n"; }
};

ELLE_SERIALIZE_SIMPLE(A, archive, value, version)
{
  ELLE_ENFORCE(version == 0);
  archive & value.i;
  archive & value.j;
  archive & value.biet;
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
      std::cout << std::setw(2) << std::right << std::setfill('0')
                << std::hex<< (int)s[i];
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
    ELLE_ENFORCE(ptr != nullptr);
    ELLE_ENFORCE(ptr->i == 15);
    ELLE_ENFORCE(ptr->j == 16);
    ELLE_ENFORCE(ptr->biet == "MEGABIET is a string like any other");
    delete ptr;
  }


  {
    A* ptr = nullptr;
    elle::serialize::InputBinaryArchive in(ss);
    in >> elle::serialize::pointer(ptr);
    ELLE_ENFORCE(ptr != nullptr);
    ELLE_ENFORCE(ptr->i == 15);
    ELLE_ENFORCE(ptr->j == 16);
    ELLE_ENFORCE(ptr->biet == "MEGABIET is a string like any other");
    delete ptr;
  }

  {
    A const a{15, 16, "MEGABIET is a string like any other"};
    A const* a_ptr = &a;
    elle::serialize::OutputBinaryArchive out(ss);
    out << elle::serialize::pointer(a_ptr);
  }

  {
    A const a{15, 16, "MEGABIET is a string like any other"};
    elle::serialize::OutputBinaryArchive out(ss);
    out << elle::serialize::pointer(&a);
  }

  std::cout << "tests done.\n";
  return 0;
}
