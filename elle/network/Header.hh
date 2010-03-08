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
// updated       julien quintard   [wed mar  3 14:00:29 2010]
//

#ifndef ELLE_NETWORK_HEADER_HH
#define ELLE_NETWORK_HEADER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/archive/Archive.hh>

#include <elle/network/Tag.hh>
#include <elle/network/Identifier.hh>

namespace elle
{
  using namespace archive;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Header:
      public Archivable
    {
    public:
      //
      // constants
      //
      static const String		Name;

      //
      // methods
      //
      Status		Create(const Identifier&,
			       const Tag,
			       const Natural32);

      //
      // interfaces
      //

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Identifier	identifier;
      Tag		tag;
      Natural32		size;
    };

  }
}

#endif
