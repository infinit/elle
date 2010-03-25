//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Header.hh
//
// created       julien quintard   [mon feb 22 23:48:40 2010]
// updated       julien quintard   [thu mar 25 16:15:33 2010]
//

#ifndef ELLE_NETWORK_HEADER_HH
#define ELLE_NETWORK_HEADER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/archive/Archive.hh>
#include <elle/concurrency/Concurrency.hh>

#include <elle/network/Tag.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace archive;
  using namespace concurrency;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Header:
      public Dumpable, public Archivable
    {
    public:
      //
      // constants
      //
      static const String		Name;

      //
      // methods
      //
      Status		Create(const Event&,
			       const Tag,
			       const Natural32);

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
      Event		event;
      Tag		tag;
      Natural32		size;
    };

  }
}

#endif
