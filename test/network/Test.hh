//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/Test.hh
//
// created       julien quintard   [wed jan 28 11:28:16 2009]
// updated       julien quintard   [sun nov 29 21:19:52 2009]
//

#ifndef ELLE_TEST_NETWORK_TEST_HH
#define ELLE_TEST_NETWORK_TEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/test/network/Node.hh>

#include <iostream>

#include <QCoreApplication>

namespace elle
{
  using namespace core;

  namespace test
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Test
    {
    public:
      //
      // methods
      //
      static Status	Main(int, char*[]);
    };

  }
}

#endif

