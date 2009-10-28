//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/channel/Test.cc
//
// created       julien quintard   [wed jan 28 11:22:24 2009]
// updated       julien quintard   [fri oct 16 05:34:00 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Test.hh"

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace channel;

  namespace test
  {

//
// ---------- methods ---------------------------------------------------------
//

    Status		Test::Main()
    {
      Bridge		bridge;

      /*
      channel.Open(AuthenticationActive, EncryptionActive);
      channel.Restrict(pubkey or id);
      channel.Send(destination, message);
      channel.Receive(source, message);
      channel.Close();
      */

      leave();
    }

  }
}

//
// ---------- main ------------------------------------------------------------
//

int			main()
{
  if (elle::test::Test::Main() == elle::misc::StatusError)
    {
      std::cerr << elle::misc::report << std::endl;

      return (1);
    }

  std::cout << "[success]" << std::endl;

  return (0);
}
