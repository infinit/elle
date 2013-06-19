
#define BOOST_TEST_MODULE Piece
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <elle/io/Piece.hh>

namespace elle
{
  namespace io
  {
    static
    std::ostream&
    operator << (std::ostream &out, elle::io::Piece const &p)
    {
      p.Dump();
      return out;
    }
  }
}

BOOST_AUTO_TEST_CASE(piece)
{
  elle::io::Piece p1("key", "value");
  elle::io::Piece p2("key", "value");

  BOOST_CHECK_EQUAL(p1, p2);
}
