//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/ContentHashBlock.hh
//
// created       julien quintard   [tue feb 17 12:33:12 2009]
// updated       julien quintard   [fri aug 14 21:05:06 2009]
//

#ifndef ETOILE_CORE_CONTENTHASHBLOCK_HH
#define ETOILE_CORE_CONTENTHASHBLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Hole.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class ContentHashBlock:
      public hole::Block
    {
    public:
      //
      // methods
      //
      Status		Self(hole::Address&) const;
      Status		Validate(const hole::Address&) const;

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);
    };

  }
}

#endif
