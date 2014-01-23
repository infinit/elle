#include <cassert>
#include <iostream>
#include <sstream>

#include <elle/Buffer.hh>
#include <elle/Exception.hh>
#include <elle/serialize/Base64Archive.hh>
#include <elle/serialize/BinaryArchive.hh>
#include <elle/serialize/HexadecimalArchive.hh>
#include <elle/serialize/JSONArchive.hxx>
#include <elle/test.hh>

using namespace elle::serialize;

template<template <ArchiveMode> class Archive>
void testLinear()
{
  std::stringstream ss;
    {
      Archive<ArchiveMode::Output> ar(ss);
      ar << named("pif", 12);
      ar << named("paf", std::string("Salut"));
      ar << named("a float", 12.5f);
      ar << 42.01;
      ar << named("wqw, ", 'c');
      ar << '1';
      ar << elle::Buffer((elle::Byte const*)"pif", 3);
    }

  //std::cout << ss.str() << std::endl;
    {
      Archive<ArchiveMode::Input> ar(ss);
      int int_;
      std::string string_;
      float float_;
      double double_;
      char char_;
      elle::Buffer buffer_;

      ar >> named("pif", int_);
      assert(int_ == 12);
      ar >> named("paf", string_);
      assert(string_ == std::string("Salut"));
      ar >> named("a float", float_);
      assert(float_ == 12.5f);
      ar >> double_;
      assert(double_ == 42.01);
      ar >> char_;
      assert(char_ == 'c');
      ar >> char_;
      assert(char_ == '1');
      ar >> buffer_;
      assert(std::string((char const*)buffer_.contents(), buffer_.size()) == "pif");
    }
}

struct A
{
  std::string a;
  std::string b;
  std::string c;
  float       float_;
  int         int_;
  short       short_;

  bool operator ==(A const& o) const
    {
      return (
              a == o.a
          &&  b == o.b
          &&  c == o.c
          &&  float_ == o.float_
          &&  int_ == o.int_
          &&  short_ == o.short_

      );
    }
};



namespace elle { namespace serialize {
  template<> struct Serializer<A>
  {
    template<typename Archive>
    static void Serialize(Archive& ar, A& a, unsigned int)
      {
        ar & named("a", a.a);
        ar & named("b", a.b);
        ar & named("c", a.c);
        ar & named("a float", a.float_);
        ar & named("an int", a.int_);
        ar & named("a short", a.short_);
      }
  };
}}

struct B
{
  int i, j;
  A a;
  bool operator == (B const& o) const
    {
      return i == o.i && j == o.j && a == o.a;
    }
};


namespace elle { namespace serialize {
  template<> struct Serializer<B>
  {
    template<typename Archive>
    static void Serialize(Archive& ar, B& b, unsigned int)
      {
        ar & named("i", b.i);
        ar & named("j", b.j);
        ar & named("a", b.a);
      }
  };
}}

template<template <ArchiveMode> class Archive> void testClass()
{
  std::stringstream ss;
  A el1{"salut", "pif", "paf", 42.3f, 32, 3000};
  A el2{"1", "2", "3", 77.5f, 24, 3001};
  B el3{12, 13, A{"4", "5", "6", 32.5f, 2000000000, 443}};
    {
      Archive<ArchiveMode::Output> ar(ss);
      ar & named("el1", el1);
      ar << named("el2", el2);
      ar << named("el3", el3);
    }
    {
      Archive<ArchiveMode::Input> ar(ss);
      A a;
      ar & named("el1", a);
      assert(a == el1);
      ar & named("el2", a);
      assert(a == el2);
      B b;
      ar & named("el3", b);
      assert(b == el3);
    }
}

// static
// void
// testJSON()
// {
//   A el1{"salut", "pif", "paf", 42.3f, 32, 3000};
//   A el2{"1", "2", "3", 77.5f, 24, 3001};
//   B el3{12, 13, A{"4", "5", "6", 32.5f, 2000000000, 443}};
//   std::stringstream ss;
//     {
//       OutputJSONArchive(ss, el1);
//       OutputJSONArchive(ss, el2);
//       OutputJSONArchive(ss, el3);
//     }
//     {
//       A a;
//       InputJSONArchive(ss, a);
//       assert(a == el1);
//       InputJSONArchive(ss, a);
//       assert(a == el2);
//       B b;
//       InputJSONArchive(ss, b);
//       assert(b == el3);
//     }
// }

#include <elle/format/hexadecimal.hh>

static
void
all_archive()
{
  /* XXX
  testLinear<BinaryArchive>();
  testLinear<Base64Archive>();
  testLinear<HexadecimalArchive>();

  testClass<BinaryArchive>();
  testClass<Base64Archive>();
  testClass<HexadecimalArchive>();

  testJSON();
  */

  // XXX[ce test doit absolument etre inclus]
  {
    std::stringstream ssoutput;
    BinaryArchive<ArchiveMode::Output> aroutput(ssoutput);

    elle::Buffer soutput((elle::Byte const*)"sucemoncul", 11);
    aroutput << soutput;
    elle::String string = ssoutput.str().substr(0, 15);

    std::cout << string << std::endl;

    std::stringstream ssinput(string);
    BinaryArchive<ArchiveMode::Input> arinput(ssinput);
    elle::Buffer sinput;
    BOOST_CHECK_THROW(arinput >> sinput, elle::Exception);
  }
  // XXX
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(all_archive));
}
