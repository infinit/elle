#include <iostream>

#include <elle/io/Piece.hh>
#include <elle/test.hh>

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

static
void
piece()
{
  elle::io::Piece p1("key", "value");
  elle::io::Piece p2("key", "value");

  BOOST_CHECK_EQUAL(p1, p2);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(piece));
}

