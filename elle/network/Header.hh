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
// updated       julien quintard   [fri may 28 12:18:00 2010]
//

#ifndef ELLE_NETWORK_HEADER_HH
#define ELLE_NETWORK_HEADER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/archive/Archive.hh>

#include <elle/concurrency/Event.hh>

#include <elle/network/Tag.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
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
      public Object
    {
    public:
      //
      // constants
      //
      static const String		Name;

      //
      // constructors & destructors
      //
      Header();

      //
      // methods
      //
      Status		Create(const Event&,
			       const Tag,
			       const Natural32);

      //
      // interfaces
      //

      // object
      declare(Header);

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
