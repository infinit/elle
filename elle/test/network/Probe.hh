//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/Probe.hh
//
// created       julien quintard   [fri nov 27 19:37:50 2009]
// updated       julien quintard   [mon nov 30 13:16:28 2009]
//

#ifndef ELLE_TEST_NETWORK_PROBE_HH
#define ELLE_TEST_NETWORK_PROBE_HH

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/network/Network.hh>

#include <elle/test/network/Tag.hh>

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
    class Probe:
      public Packet
    {
    public:
      //
      // constructors & destructors
      //
      Probe();

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
      String		name;
    };

  }
}

#endif
