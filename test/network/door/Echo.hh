//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/door/Echo.hh
//
// created       julien quintard   [fri nov 27 19:37:50 2009]
// updated       julien quintard   [sun feb  7 01:36:22 2010]
//

#ifndef ELLE_TEST_NETWORK_ECHO_HH
#define ELLE_TEST_NETWORK_ECHO_HH

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/network/Network.hh>

#include <elle/test/network/door/Tag.hh>

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace network;

  namespace test
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this packet is used for notifying nodes.
    ///
    class Echo:
      public Message
    {
    public:
      //
      // constants
      //
      static const Tag		Identifier;

      //
      // methods
      //
      Status		Create(const String&);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      String		text;
    };

  }
}

#endif
